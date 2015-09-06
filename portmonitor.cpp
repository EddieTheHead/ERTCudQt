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
    GPSValid = false;
    adc2Ready[0] = false;
    adc2Ready[1] = false;
    adc7Ready[0] = false;
    adc7Ready[1] = false;
    AutoReconnect = true;
}

PortMonitor::~PortMonitor()
{
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
//        logger = std::unique_ptr<LoggingDevice>(new LoggingDevice(this,p.name));
//        connect(this,SIGNAL(newDataArrived(QByteArray)),logger.get(),SLOT(addLine(QByteArray)));

//        //Logger zapisujący informacje w przyjemniejszej dla człowieka formie
//        loggerNatural = new LoggingDevice(this,p.name,"Natural");
//        connect(this,SIGNAL(newControlsStateString(QString)),loggerNatural,SLOT(addLine(QString)));
        emit connectionEstablished();
        qDebug() << "connected";
    }
    else
    {
        //nie bangla
        //QMessageBox::critical(parent,"Błąd",port->errorString());
    }
    errorTimer->start();
}

void PortMonitor::closeSerialPort()
{
    if(loggerNatural != nullptr)
    {
        loggerNatural->closeFile();
        delete loggerNatural;
    }

    if(errorTimer != nullptr) errorTimer->stop();
    port->close();
    emit connectionClosed();
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
        if (data[SizeOfFrame-1]==LastBytes2 && data[SizeOfFrame-2] == LastBytes1) //sprawdź czy 2 ostatnie bajty są zgodne z specyfikacją ramki
        {
            //dane dla loggera ramek i pola tekstowego
            emit newDataArrived(QByteArray(data,SizeOfFrame));

            //8 i 9 napięcie baterii
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
                emit newGPSDataValid(data[ValidGPSByte]);
                GPSValid = data[ValidGPSByte];
                if(GPSValid)
                {
                    TempLon = charsToFolat(&data[FirstLongitudeByte]);
                    LonReady = true;
                }
                break;
            case 3:
                emit newNumberOfSatelites(data[NumberOfSatelitesByte]);
                if(GPSValid)
                {
                    TempLat = charsToFolat(&data[FirstLatitudeudeByte]);
                    LatReady = true;
                }
                break;
            case 4:
                emit newGPSSignalQuality(data[GpsSignalQualityByte]);
                emit newSonarFarLeftValue((uint) data[SonarFarLeftByte]);
                emit newSonarCenterLeftValue((uint) data[SonarCentralLeftByte]);
                emit newSonarCentralRightValue((uint) data[SonarCentralRightByte]);
                emit newSonarFarRightValue((uint) data[SonarFarRightbyte]);
                break;
            case 5:
                emit newLimitsState((uchar) data[LimitsByte]);
                emit newXAxisValue(mergeBytes(data[XAxisHighByte],data[XAxisLowByte]));
                emit newYAxisValue(mergeBytes(data[YAxisHighByte],data[YAxisLowByte]));
                break;
            case 6:
                //TODO: położenie narzędzia
                emit newCameraPosition(mergeBytes(data[CameraPositionHighByte],data[CameraPositionLowByte]));
                emit newToolPosition(mergeBytes(data[ToolPositionHighByte],data[ToolPositionLowByte]));
                tempAdc7[0] = data[ADC27byte];
                adc7Ready[0] = true;
                break;
            case 7:
                tempAdc2[0] = data[ADC27byte];
                adc2Ready[0] = true;
                emit newAdcValue(0,mergeBytes(data[HighADC035Byte],data[LowADC035Byte]));
                emit newAdcValue(1,mergeBytes(data[HighADC146Byte],data[LowADC146Byte]));
                break;
            case 8:
                tempAdc7[1] = data[ADC27byte];
                adc7Ready[1] = true;
                emit newAdcValue(3,mergeBytes(data[HighADC035Byte],data[LowADC035Byte]));
                emit newAdcValue(4,mergeBytes(data[HighADC146Byte],data[LowADC146Byte]));
                break;
            case 9:
                tempAdc7[1] = data[ADC27byte];
                adc7Ready[1] = true;
                emit newAdcValue(5,mergeBytes(data[HighADC035Byte],data[LowADC035Byte]));
                emit newAdcValue(6,mergeBytes(data[HighADC146Byte],data[LowADC146Byte]));
                break;
            case 10:
                emit newCompassValue((-1 * (int) mergeBytes(data[HighCompassByte],data[LowCompassByte]) +390)%360);
                emit newMysteryServoValue((int) mergeBytes(data[MysteryServoHighByte],data[MysteryServoLowByte]));
                qDebug() <<  (int) mergeBytes(data[MysteryServoHighByte],data[MysteryServoLowByte]);
                break;
            default:
                break;
            }
            //dane z GPSa
            if(LonReady && LatReady )
            {
               // qDebug() << "emit newGPS(qmapcontrol::PointWorldCoord(TempLon,TempLat));";
                emit newGPS(qmapcontrol::PointWorldCoord(TempLon,TempLat));
//                emit newGPS(qmapcontrol::PointWorldCoord(TempLat,TempLon));
                LatReady = false;
                LonReady = false;
            }
            //adc 7 i 2
            if(adc2Ready[0] && adc2Ready[1])
            {
                emit newAdcValue(2,mergeBytes(tempAdc2[0],tempAdc2[1]));
            }
            if(adc7Ready[0] && adc7Ready[1])
            {
                emit newAdcValue(7,mergeBytes(tempAdc7[0],tempAdc7[1]));
            }

            //reset timera odmierzającego sekundę od ostatniej poprawnej ramki, aby wyświetlić ostrzeżenie o braku danych
            errorTimer->start();
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
    //QMessageBox::warning(parent,"Przekroczenie czasu","Od nadejścia ostatniej poprawnej ramki minęła ponad sekunda");
    closeSerialPort();
    if(AutoReconnect) openSerialPort();
}

void PortMonitor::computeCompasValue(float lastArrivedValue)
{
    //Obiekt inercyjny pierwszego rzędu :)

    //dodaje na koniec kolejki nową wartość kompasu
    LastTenCompassValues.enqueue(lastArrivedValue);
    //średnia z ostatnich dziesięciu pomiarów kompasu
    float mean = 0;
    for(const auto &i : LastTenCompassValues)
    {
        mean += i;
    }
    mean /= LastTenCompassValues.size();
    if (LastTenCompassValues.size() > 10) LastTenCompassValues.dequeue();

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
bool PortMonitor::getAutoReconnect() const
{
    return AutoReconnect;
}

void PortMonitor::setAutoReconnect(bool value)
{
    AutoReconnect = value;
}


