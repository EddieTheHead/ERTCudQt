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

    logger = NULL;
    loggerNatural = NULL;

    //z dokumentacji od Kaluby
    FirstByte = 0x54;
    LastBytes1 = 0x0D;
    LastBytes2 = 0x0A;
    SizeOfFrame = 22;

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

        //Logger zapisujący informacje w przyjemniejszej dla człowieka formie
        loggerNatural = new LoggingDevice(this,p.name,"Natural");
        connect(this,SIGNAL(newControlsStateString(QString)),loggerNatural,SLOT(addLine(QString)));

        qDebug() << "connected";
    }
    else
    {
        //nie bangla
        QMessageBox::critical(parent,"Blad",port->errorString());
    }
    errorTimer->start();
}

void PortMonitor::closeSerialPort()
{
    if(logger != NULL) logger->closeFile();
    if(loggerNatural != NULL) loggerNatural->closeFile();
    delete logger;
    delete loggerNatural;
    errorTimer->stop();
    port->close();
}

void PortMonitor::readData()
{
    QIODevice *dev = port; //qobject_cast<QIODevice *>(sender());

    while(1) {
        if (dev->bytesAvailable()<SizeOfFrame) return; // nie ma dostępnej pełnej ramki, trzeba czekać na więcej danych
        char data[SizeOfFrame];
        dev->peek(data, SizeOfFrame); // podpatrz dostępne dane
        int i=0;
        for (;i<SizeOfFrame; ++i)
        {
            if (data[i]==FirstByte) break; // znajdź początek ramki
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
            //lewy joystick pionowy
            qint16 leftVerticalTrigger = mergeBytes(data[4],data[3]);
            emit newlLeftVerticaTriggerValue(leftVerticalTrigger);
            //lewy joystick poziomy

            //reset timera odmierzającego sekundę od ostatniej poprawnej ramki, aby wyświetlić ostrzeżenie o raku danych
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
    QMessageBox::warning(parent,"Przekroczenie czasu","Od nadejścia ostatniej poprawnej ramki minęła ponad sekunda");
}

qint16 PortMonitor::mergeBytes(char first, char second)
{
    qint16 a = (unsigned char) first;
    a = a << 8;
    qint16 b = (unsigned char) second;
    return a | b;
}
int PortMonitor::getSizeOfFrame() const
{
    return SizeOfFrame;
}

void PortMonitor::setSizeOfFrame(int value)
{
    SizeOfFrame = value;
}

