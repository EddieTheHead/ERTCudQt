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
#include <Point.h>
using namespace qmapcontrol;

class PortSettingsWindow;

class PortMonitor:public QObject
{
    Q_OBJECT
public:
    PortMonitor(QWidget *parent);
    ~PortMonitor();

    PortSettingsWindow *settings;
    bool getAutoReconnect() const;

public slots:
    void closeSerialPort();
    void setAutoReconnect(bool value);
signals:
    void newDataArrived(QByteArray);
    void newCompassValue(float);
    void newGPS(PointWorldCoord);
    void newBateryVoltage(float);
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
    void newLimitsState(uchar);
    void newAdcValue(int,int); //numer adc, wartość
    void newSonarFarLeftValue(int);
    void newSonarCenterLeftValue(int);
    void newSonarFarRightValue(int);
    void newSonarCentralRightValue(int);
    void newXAxisValue(int);
    void newYAxisValue(int);
    void newCameraPosition(int);
    void newToolPosition(int);
    void newMysteryServoValue(int);

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

    bool adc7Ready[2];
    bool adc2Ready[2];
    char tempAdc2[2];
    char tempAdc7[2];

    bool AutoReconnect;
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
    const int LowCompassByte = 12;
    const int HighCompassByte = 13;
    const int MysteryServoLowByte = 14;
    const int MysteryServoHighByte = 15;
    const int LowADC035Byte = 12;
    const int HighADC035Byte = 13;
    const int LowADC146Byte = 14;
    const int HighADC146Byte = 15;
    const int ADC27byte= 16;
    const int LimitsByte = 12;
    const int SonarFarLeftByte = 13;
    const int SonarCentralLeftByte = 14;
    const int SonarCentralRightByte = 15;
    const int SonarFarRightbyte = 16;
    const int XAxisHighByte = 16;
    const int XAxisLowByte = 15;
    const int YAxisHighByte = 14;
    const int YAxisLowByte = 13;
    const int ToolPositionHighByte = 13;
    const int ToolPositionLowByte = 12;
    const int CameraPositionHighByte = 15;
    const int CameraPositionLowByte = 14;

};

#endif // PORTMONITOR_H
