#include "portmonitor.h"
#include <QDebug>
#include <QTextStream>
PortMonitor::PortMonitor(QWidget *parent)
{
    this->parent=parent;

    port = new QSerialPort(this);
    settings = new PortSettingsWindow;

    connect(port,SIGNAL(error(QSerialPort::SerialPortError)), this,SLOT(handleError(QSerialPort::SerialPortError)));
    connect(port,SIGNAL(readyRead()),this,SLOT(readData()));

    errorTimer = new QTimer(this);
    errorTimer->setInterval(1000);
    errorTimer->setSingleShot(true);
    connect(errorTimer,SIGNAL(timeout()),this,SLOT(onErrorTimer()));

    logger = nullptr;
    loggerNatural = nullptr;

    LatReady = false;
    LonReady = false;
}

PortMonitor::~PortMonitor()
{
    //delete logger;
    delete errorTimer;
    delete port;
    delete settings;
}

void PortMonitor::openSerialPort()
{
    PortSettingsWindow::Settings p = settings->settings();
    //nazwa musi być ustawiona jeszcze przed otwarciem portu
    port->setPortName(p.name);

    if(port->open(QIODevice::ReadOnly))
    {
        //połączono
        port->setFlowControl(p.flowControl);
        port->setBaudRate(p.baudRate);
        port->setDataBits(p.dataBits);
        port->setParity(p.parity);
        port->setStopBits(p.stopBits);

        //Logger zapisujący ramki
        logger = new LoggingDevice(this,p.name);
        connect(this,SIGNAL(newDataArrived(QByteArray)),logger,SLOT(addLine(QByteArray)));

//        //Logger zapisujący informacje w przyjemniejszej dla człowieka formie
//        loggerNatural = new LoggingDevice(this,p.name,"Natural");
//        connect(this,SIGNAL(newControlsStateString(QString)),loggerNatural,SLOT(addLine(QString)));
        emit connectionEstablished();
        qDebug() << "connected";
    }
    else
    {
        //nie bangla
        emit connectionClosed();
        QMessageBox::critical(parent,"Błąd",port->errorString());
    }
    //errorTimer->start();
}

void PortMonitor::closeSerialPort()
{
    if(logger != nullptr)
    {
        logger->closeFile();
        delete logger;
    }

    if(loggerNatural != nullptr)
    {
        loggerNatural->closeFile();
        delete loggerNatural;
    }

    if(errorTimer != nullptr) errorTimer->stop();
    port->close();
}

void PortMonitor::readData()
{
    QIODevice *dev = qobject_cast<QIODevice *>(sender());

    while(1) {
        if (dev->bytesAvailable()<SizeOfFrame) return; // nie ma dostępnej pełnej ramki, trzeba czekać na więcej danych
        char data[SizeOfFrame];
        dev->peek(data, SizeOfFrame); // podpatrz dostępne dane
        int i=0;
        for (;i<SizeOfFrame; ++i)
        {
            if (strncmp(&data[i],syncWord,7) == 0) break;
        }
        if (i!=0) {
            dev->read(data, i); // te dane zostaną utracone - są pozostałością poprzedniej, niepełnej ramki
            continue;
        }
        dev->read(data, SizeOfFrame); //właściwe odczytanie ramki z portu
        //QByteArray array = dev->readAll();
        if (data[SizeOfFrame-1]==LastBytes2 && data[SizeOfFrame-2] == LastBytes1) //sprawdź czy 2 ostatnie bajty są zgodne z specyfikacją ramki
        {
            //dane dla loggera ramek i pola tekstowego
            emit newDataArrived(QByteArray(data,SizeOfFrame));            

            //8 i 9 napięcie baterii
            qDebug() << "data[8]" << QString::number(data[FirstRecieverBatteryByte],16) << "data[9]" << QString::number(data[FirstRecieverBatteryByte+1],16);
            qDebug() << "Wysyłana wartość baterii odbiornika:" << ((float) mergeBytes(data[FirstRecieverBatteryByte], data[FirstRecieverBatteryByte+1])*0.02);
            emit newRecieverBatteryValue((float) mergeBytes(data[FirstRecieverBatteryByte], data[FirstRecieverBatteryByte+1])*0.02);

            //RSSI
            emit newRssiTx((uchar) data[RssiTxByte]);
            emit newRssiRx((uchar) data[RssiRxbyte]);

            switch (int(data[FrameTypeByte])) {
            case 1:
                emit newBateryVoltage(calculateFloatFromTwoBytes(&data[FirstBatteryByte]));
                emit newRssiValue(calculateFloatFromTwoBytes(&data[FirstRssiValueByte]));
                emit newWorkingEnginesValue(data[WorkingEnginesByte]);
                break;
            case 2:
                if(data[ValidGPSByte])
                {
                    TempLon = charsToFolat(&data[FirstLongitudeByte]);
                    LonReady = true;
                }
                break;
            case 3:
                emit newNumberOfSatelites(data[NumberOfSatelitesByte]);
                if(data[ValidGPSByte])
                {
                    TempLat = charsToFolat(&data[FirstLatitudeudeByte]);
                    LatReady = true;
                }
                break;
            case 4:
                emit newGPSSignalQuality(data[GpsSignalQualityByte]);
                break;
            default:
                break;
            }

            //dane z GPSa
            if(LonReady && LatReady)
            {
                emit newGPS(QPointF(TempLon,TempLat));
                LatReady = false;
                LonReady = false;
            }
            //reset timera odmierzającego sekundę od ostatniej poprawnej ramki, aby wyświetlić ostrzeżenie o braku danych
            //errorTimer->start();
        }
    }
}

void PortMonitor::handleError(QSerialPort::SerialPortError error)
{
    if(error == QSerialPort::ResourceError)
    {
        QMessageBox::critical(parent,"Zepsułem się",port->errorString());
        closeSerialPort();
    }
}

void PortMonitor::onErrorTimer()
{
    QMessageBox::warning(parent,"Przekroczenie czasu","Od nadejścia ostatniej poprawnej ramki minęła ponad sekunda");
}

void PortMonitor::computeCompasValue(float lastArrivedValue)
{
    //Obiekt inercyjny pierwszego rzędu :)

    //dodaje na koniec kolejki nową wartość kompasu
    LastTenCompassValues.enqueue(lastArrivedValue);
    //usunięcie najstarszej wartości
    LastTenCompassValues.dequeue();
    //średnia z ostatnich dziesięciu pomiarów kompasu
    float mean = 0;
    for(const auto &i : LastTenCompassValues)
    {
        mean += i;
    }
    mean /= LastTenCompassValues.size();

    emit newCompassValue(mean);
}

union FloatingPointNumber
{
    float numeric;
    char byte[sizeof(float)];
};

float PortMonitor::charsToFolat(char *bytes)
{
    union FloatingPointNumber data;

    for (size_t i = 0; i < sizeof(float); ++i)
    {
        data.byte[i] = bytes[i];
    }

    return data.numeric;
}

QPointF PortMonitor::charTabsToQPointF(char *x, char *y)
{
    return QPointF(charsToFolat(x),charsToFolat(y));
}

float PortMonitor::calculateFloatFromTwoBytes(char *data)
{
    return (float) data[0] + (float) data[1] * 0.01;
}

qint16 PortMonitor::mergeBytes(char first, char second)
{
    qint16 a = (unsigned char) first;
    a = a << 8;
    qint16 b = (unsigned char) second;
    return a | b;
}

