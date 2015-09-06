/*Na podstawie kodu Denisa Shioenkova i Laszko Papp http://qt-project.org/doc/qt-5/qtserialport-terminal-settingsdialog-cpp.html */

#include "portsettingswindow.h"
#include "ui_PortSettingsWindow.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QIntValidator>
#include <QLineEdit>

QT_USE_NAMESPACE

PortSettingsWindow::PortSettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PortSettingsWindow)
{
    ui->setupUi(this);

    intValidator = new QIntValidator(0,40000000,this); //ograniczy zakres możliwych do wprowadzenia prędkośći
    ui->comboBox_baudRate->setInsertPolicy(QComboBox::NoInsert);

    connect(ui->pushButton_apply,SIGNAL(clicked()),this,SLOT(apply()));
    connect(ui->comboBox_Port,SIGNAL(currentIndexChanged(int)),this,SLOT(showPortInfo(int)));
    connect(ui->comboBox_baudRate,SIGNAL(currentIndexChanged(int)),this,SLOT(checkCustomBaudRatePolicy(int)));
    connect(ui->pushButton_refresh,SIGNAL(clicked()),this,SLOT(fillPortsInfo()));
    fillPortsParameters();
    fillPortsInfo();

    updateSettings();
}

PortSettingsWindow::~PortSettingsWindow()
{
    delete ui;
    delete intValidator;
}

PortSettingsWindow::Settings PortSettingsWindow::settings() const
{
    return currentSettings;
}

void PortSettingsWindow::showPortInfo(int idx)
{
    if(idx != -1)
    {
        QStringList list = ui->comboBox_Port->itemData(idx).toStringList();
        ui->label_description->setText(QString("Opis: %1").arg(list.at(1)));
        ui->label_manufacturer->setText(QString("Producent: %1").arg(list.at(2)));
        ui->label_serial->setText(QString("Numer seryjny: %1").arg(list.at(3)));
        ui->label_location->setText(QString("Lokalizacja: %1").arg(list.at(4)));
        ui->label_vendorId->setText(QString("ID producenta: %1").arg(list.at(5)));
        ui->label_productId->setText(QString("ID produktu: %1").arg(list.at(6)));
    }
}

void PortSettingsWindow::apply()
{
    updateSettings();
    hide();
}

void PortSettingsWindow::checkCustomBaudRatePolicy(int idx)
{
    bool isCustomBaudRate = !ui->comboBox_baudRate->itemData(idx).isValid();
    ui->comboBox_baudRate->setEditable(isCustomBaudRate);
    if(isCustomBaudRate)
    {
        ui->comboBox_baudRate->clearEditText();
        QLineEdit *lineEdit = ui->comboBox_baudRate->lineEdit();
        lineEdit->setValidator(intValidator); //nie wprowadzimy więcej niż 40 000 000
    }
}

void PortSettingsWindow::fillPortsParameters()
{
    //wypełnia comboboxy z opcjami transmisji
    ui->comboBox_baudRate->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->comboBox_baudRate->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->comboBox_baudRate->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->comboBox_baudRate->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->comboBox_baudRate->addItem(QStringLiteral("1000000"),1000000);
    ui->comboBox_baudRate->addItem(QStringLiteral("Custom"));
    ui->comboBox_baudRate->setCurrentIndex(4); //z automatu ustawiam 1000000, bo tak nadaje pilot

    ui->comboBox_dataBits->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->comboBox_dataBits->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->comboBox_dataBits->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->comboBox_dataBits->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->comboBox_dataBits->setCurrentIndex(3); //z automatu ustawiam 8 bitową ramkę

    ui->comboBox_parity->addItem(QStringLiteral("None"), QSerialPort::NoParity);
    ui->comboBox_parity->addItem(QStringLiteral("Even"), QSerialPort::EvenParity);
    ui->comboBox_parity->addItem(QStringLiteral("Odd"), QSerialPort::OddParity);
    ui->comboBox_parity->addItem(QStringLiteral("Mark"), QSerialPort::MarkParity);
    ui->comboBox_parity->addItem(QStringLiteral("Space"), QSerialPort::SpaceParity);

    ui->comboBox_stopBits->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->comboBox_stopBits->addItem(QStringLiteral("1.5"), QSerialPort::OneAndHalfStop); //dostępne tylko na widowsie
#endif
    ui->comboBox_stopBits->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->comboBox_flowControl->addItem(QStringLiteral("None"), QSerialPort::NoFlowControl);
    ui->comboBox_flowControl->addItem(QStringLiteral("RTS/CTS"), QSerialPort::HardwareControl);
    ui->comboBox_flowControl->addItem(QStringLiteral("XON/XOFF"), QSerialPort::SoftwareControl);
}

void PortSettingsWindow::fillPortsInfo()
{
    ui->comboBox_Port->clear();
    static const QString blankString = QObject::tr("N/A");
    QString description;
    QString manufacturer;
    QString serialNumber;
    //wyszukuje w systemie dostępne porty i wypełnia nimi comboboxa Port, żeby można było wybrać dobry

    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.productIdentifier();
        list << info.portName()
             << (!description.isEmpty() ? description: blankString)
             << (!manufacturer.isEmpty() ? manufacturer: blankString)
             << (!serialNumber.isEmpty() ? serialNumber: blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(),16): blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(),16): blankString);
        ui->comboBox_Port->addItem(list.first(),list);
    }
}

void PortSettingsWindow::updateSettings()
{
    //nazwa
    currentSettings.name = ui->comboBox_Port->currentText();
    //prędkość
    if(ui->comboBox_baudRate->currentIndex() == 4) //własna prędkość wpisana do lineEdita
    {
        currentSettings.baudRate = ui->comboBox_baudRate->currentText().toInt();
    }
    else
    {
        currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
                ui->comboBox_baudRate->itemData(ui->comboBox_baudRate->currentIndex()).toInt());
    }
    currentSettings.stringBaudRate = QString::number(currentSettings.baudRate);
    //bity danych
    currentSettings.dataBits = static_cast<QSerialPort::DataBits>(
                ui->comboBox_dataBits->itemData(ui->comboBox_dataBits->currentIndex()).toInt());
    currentSettings.stringDataBits = ui->comboBox_dataBits->currentText();
    //bity synchronizacji
    currentSettings.parity = static_cast<QSerialPort::Parity>(
                ui->comboBox_parity->itemData(ui->comboBox_parity->currentIndex()).toInt());
    currentSettings.stringParity = ui->comboBox_parity->currentText();
    //bity stopu
    currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
                ui->comboBox_stopBits->itemData(ui->comboBox_stopBits->currentIndex()).toInt());
    currentSettings.stringStopBits = ui->comboBox_stopBits->currentText();
    //kontrola przepływu
    currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
                ui->comboBox_flowControl->itemData(ui->comboBox_flowControl->currentIndex()).toInt());
    currentSettings.stringFlowControl = ui->comboBox_flowControl->currentText();
    //echo
    currentSettings.localEchoEnabled = ui->checkBox_echo->isChecked();
}


