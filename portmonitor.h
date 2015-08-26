#ifndef PORTMONITOR_H
#define PORTMONITOR_H

#include <QtSerialPort>
#include <QMessageBox>
#include "portsettingswindow.h"
#include <QIODevice>
#include <QTimer>
#include "loggingdevice.h"
#include <QPointF>
#include <memory>

class PortSettingsWindow;

class PortMonitor:public QObject
{
    Q_OBJECT
public:
    PortMonitor(QWidget *parent);
    ~PortMonitor();

    PortSettingsWindow *settings;
public slots:
    void closeSerialPort();
signals:
    void newDataArrived(QByteArray);
    void newCompassValue(float);
    void newGPS(QPointF);
    void newBateryVoltage(float);
    void newControlsStateString(QString);
    void newScalesDrill(int);
    void newScalesEx(int);
    void newRssiRx(int);
    void newRssiTx(int);
    void newRssiValue(float);
    void newWorkingEnginesValue(bool);
    void newNumberOfSatelites(int);
    void newGPSSignalQuality(int);
    void newRecieverBatteryValue(float);
    void connectionEstablished();
    void connectionClosed();
    void newGPSDataValid(bool);

private slots:
    void openSerialPort();
    void readData();
    void handleError(QSerialPort::SerialPortError error);
    void onErrorTimer();
private:
    void computeCompasValue(float lastArrivedValue);
    float charsToFolat(char *bytes);
    QPointF charTabsToQPointF(char* x, char* y);
    float calculateFloatFromTwoBytes(char* data);
    qint16 mergeBytes(char first, char second);

    float TempLat;
    float TempLon;
    bool LatReady;
    bool LonReady;
    bool GPSValid;

    QTimer *errorTimer;
    QSerialPort *port;
    QWidget *parent;
    std::unique_ptr<LoggingDevice> logger;
    LoggingDevice *loggerNatural;
    QQueue<float> LastTenCompassValues;

    //z dokumentacji od Kaluby
    const char LastBytes1 = 0x0D;
    const char LastBytes2 = 0x0A;
    const char* syncWord = "CUD2015";
    const int SizeOfFrame = 19;
    const int FirstRecieverBatteryByte = 7;
    const int RssiTxByte = 9;
    const int RssiRxbyte = 10;
    const int FrameTypeByte = 11;
    const int FirstBatteryByte = 12;
    const int WorkingEnginesByte = 14;
    const int FirstRssiValueByte = 15;
    const int ValidGPSByte = 12;
    const int NumberOfSatelitesByte = 12;
    const int GpsSignalQualityByte = 12;
    const int FirstLongitudeByte = 13;
    const int FirstLatitudeudeByte = 13;

};

#endif // PORTMONITOR_H
