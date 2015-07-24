#include "loggingdevice.h"


LoggingDevice::LoggingDevice(QObject *parent)
{
    ready = false;
    this->parent = parent;
    file = nullptr;
    FileNameModifier = "";
    PortName = "";
    createNewFile();
}

LoggingDevice::LoggingDevice(QObject *parent, QString portName)
{
    ready = false;
    this->parent = parent;
    file = nullptr;
    FileNameModifier = "";
    PortName = portName;
    createNewFile();
}

LoggingDevice::LoggingDevice(QObject *parent, QString portName, QString nameModifier)
{
    ready = false;
    this->parent = parent;
    file = nullptr;
    FileNameModifier = nameModifier;
    PortName = portName;
    createNewFile();
}

LoggingDevice::~LoggingDevice()
{
     delete file;
}

void LoggingDevice::createNewFile()
{
    //jeśli w folderze z programem nie istnieje folder "logs" stwórz go
    if(!QDir("logs/").exists())
    {
        QDir currentDir;
        currentDir.mkdir("logs");
    }
    //pobierz aktualną datę i czas
    QString date = QDate::currentDate().toString();
    QString time = QTime::currentTime().toString();
    //kropki i dwukropki są niedozwolone w nazwach plików windowsa, więc podmieniam je na myślniki
    date.replace(".","-");
    time.replace(":","-");
    QString CurrentFileString = "logs/Log-" + FileNameModifier + "-" + date + "-" + time + ".log";
    delete file;
    file = new QFile(CurrentFileString);
    if(file->open(QIODevice::ReadWrite | QIODevice::Text))
    {
        ready = true;
        QTextStream FileStream(file);
        FileStream << QString(date + " " + time + " " + PortName + "\n");
    }
    else QMessageBox::warning(0,"Bląd","Nie udało się utworzyć pliku logu.");
}

void LoggingDevice::closeFile()
{
    if(file != nullptr) file->close();
    ready = false;
}

void LoggingDevice::addLine(QByteArray data)
{
    if(ready)
    {
        QTextStream FileStream(file);
        FileStream << QString(data.toHex()) << "\n";
    }
}

void LoggingDevice::addLine(QString data)
{
    if(ready)
    {
        QTextStream FileStream(file);
        FileStream << data << "\n";
    }
}

bool LoggingDevice::getReady() const
{
    return ready;
}

void LoggingDevice::setReady(bool value)
{
    ready = value;
}

