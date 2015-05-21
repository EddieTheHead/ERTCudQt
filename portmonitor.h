#ifndef PORTMONITOR_H
#define PORTMONITOR_H

#include <QtSerialPort>
#include <QMessageBox>
#include "portsettingswindow.h"
#include <QIODevice>
#include <QTimer>
#include "loggingdevice.h"

class PortSettingsWindow;

class PortMonitor:public QObject
{
    Q_OBJECT
public:
    PortMonitor(QWidget *parent);
    ~PortMonitor();

    PortSettingsWindow *settings;
    int getSizeOfFrame() const;
    void setSizeOfFrame(int value);

public slots:
    void closeSerialPort();
signals:
    void newDataArrived(QByteArray);

    void newLeftVerticaTriggerValue(int);
    void newLeftHorizontalTriggerValue(int);
    void newLeftTriggerString(QString);

    void newRightVerticaTriggerValue(int);
    void newRightHorizontalTriggerValue(int);
    void newRightTriggerString(QString);
    void newCompassValue(float);
    void newGPS(float,float);

    void newControlsStateString(QString);
private slots:
    void openSerialPort();
    void readData();
    void handleError(QSerialPort::SerialPortError error);
    void onErrorTimer();
private:
    void computeCompasValue(float lastArrivedValue);
    QTimer *errorTimer;
    qint16 mergeBytes(char first, char second);
    QSerialPort *port;
    QWidget *parent;
    int SizeOfFrame; //wielkość ramki
    char FirstByte;  //pierwszy bajt ramki
    char LastBytes1; //przedostatni z bajtów ramki
    char LastBytes2; //ostatni z bajtów ramki
    LoggingDevice *logger;
    LoggingDevice *loggerNatural;
    QQueue<float> LastTenCompassValues;
};

#endif // PORTMONITOR_H
