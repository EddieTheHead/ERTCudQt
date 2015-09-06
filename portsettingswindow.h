#ifndef PORTSETTINGSWINDOW_H
#define PORTSETTINGSWINDOW_H

#include <QWidget>
#include <QSerialPort>
#include <QDialog>

QT_USE_NAMESPACE

namespace Ui
{
    class PortSettingsWindow;
}

class QIntValidator;

class PortSettingsWindow : public QDialog
{
    Q_OBJECT
public:
    explicit PortSettingsWindow(QWidget *parent = 0);
    ~PortSettingsWindow();
    struct Settings
    {
        QString name;
        qint32 baudRate; //prędkość transmisji
        QString stringBaudRate;
        QSerialPort::StopBits stopBits; //bity stopu
        QString stringStopBits;
        QSerialPort::DataBits dataBits; //ile będzie bitów danych
        QString stringDataBits;
        QSerialPort::Parity parity;     //bit synchronizacji
        QString stringParity;
        QSerialPort::FlowControl flowControl; //sposób kontroli transmisji
        QString stringFlowControl;
        bool localEchoEnabled;
    };
    Settings settings() const;

private slots:
    void showPortInfo(int idx);
    void apply();
    void checkCustomBaudRatePolicy(int idx);
    void fillPortsInfo();

private:
    void fillPortsParameters();
    void updateSettings();

    Ui::PortSettingsWindow *ui;
    Settings currentSettings;
    QIntValidator *intValidator;
};

#endif // PORTSETTINGSWINDOW_H
