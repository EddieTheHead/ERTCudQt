#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include <QPointF>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    monitor = new PortMonitor(this);
    mapWidget = new GpsDevice(ui->widget);

//    mapWidget = ui->widget;
    setWindowTitle("CUD 2015");
    //przyciski
    connect(ui->pushButtonConnect,SIGNAL(clicked()),monitor,SLOT(openSerialPort()));
    connect(ui->pushButtonPreferences,SIGNAL(clicked()),monitor->settings,SLOT(show()));
    connect(ui->pushButtonDisconnect,SIGNAL(clicked()),monitor,SLOT(closeSerialPort()));
    connect(ui->pushButtonCheckpoints,SIGNAL(clicked()),mapWidget,SLOT(chooseNewCheckPointsFile()));
    connect(ui->pushButtonDisconnect,SIGNAL(clicked()),ui->ConnctionIndicator,SLOT(setRed()));
    connect(ui->pushButton_fullscreen,SIGNAL(clicked()),this,SLOT(showFullScreen()));
    //sygnały wysyłane, przez monitor portu
    connect(monitor,SIGNAL(newCompassValue(float)),mapWidget,SLOT(newCompassAngle(float)));
    connect(monitor,SIGNAL(newGPS(QPointF)),mapWidget,SLOT(newGPSPosition(QPointF)));
    connect(monitor,SIGNAL(connectionEstablished()),ui->ConnctionIndicator,SLOT(setGreen()));
    connect(monitor,SIGNAL(connectionClosed()),ui->ConnctionIndicator,SLOT(setRed()));
    connect(monitor,SIGNAL(newDataArrived(QByteArray)),this,SLOT(showData(QByteArray)));
    connect(monitor,SIGNAL(newRssiRx(int)),ui->progressBar_Rx,SLOT(setValue(int)));
    connect(monitor,SIGNAL(newRssiTx(int)),ui->progressBar_Tx,SLOT(setValue(int)));
    connect(monitor,SIGNAL(newBateryVoltage(float)),this,SLOT(onNewBateryVoltage(float)));
    connect(monitor,SIGNAL(newRssiValue(float)),this,SLOT(onNewRssiValue(float)));
    connect(monitor,SIGNAL(newNumberOfSatelites(int)),this,SLOT(onNewSattelitesNumber(int)));
    connect(monitor,SIGNAL(newGPSSignalQuality(int)),this,SLOT(onNewGPSSignalQuality(int)));
    connect(monitor,SIGNAL(newWorkingEnginesValue(bool)),this,SLOT(onNewEnginesState(bool)));
    connect(monitor,SIGNAL(newRecieverBatteryValue(float)),this,SLOT(onNewReceiverBateryVoltage(float)));
    //dostosowanie kontrolek
    ui->bateriabar->setMaximum(30);
    ui->progressBar_Rx->setMaximum(127);
    ui->progressBar_Tx->setMaximum(127);
    ui->progressBar_RobotRSSI->setMaximum(310);
    ui->checkBox_follow->setChecked(true);
    ui->bateriabar_receiver->setMaximum(13.5f);
    ui->bateriabar_receiver->setMin(9.0f);
    ui->bateriabar_receiver->setUpperGreenLimit(12.8);
    ui->bateriabar_receiver->setLowerGreenLimit(10);
    ui->bateriabar_receiver->setYeallowLimit(10.5);


    //miejsce konkursu
    AreaDialog = new AreaSettingsDialog(this);
    mapWidget->setCompetitionArea(AreaDialog->getPolygon());
    connect(ui->checkBox_follow,SIGNAL(clicked(bool)),mapWidget,SLOT(setFollowingRower(bool)));
    connect(ui->pushButton_area,SIGNAL(clicked()),AreaDialog,SLOT(show()));
    connect(AreaDialog,SIGNAL(onHide()),mapWidget,SLOT(drawArea()));
    connect(AreaDialog,SIGNAL(newAreaSelected()),mapWidget,SLOT(drawArea()));

    //zmiana rozmiaru i przesunięcie
    QRect rec = QApplication::desktop()->screenGeometry();
    move(rec.width()/2,0);
    resize(rec.width()/2,rec.height()*0.9);
    //testy
    ui->ConnctionIndicator->setRed();
    onNewBateryVoltage(21);
    onNewReceiverBateryVoltage(11);
    onNewScalesEx(101);
    onNewScalesDrill(40);
    onNewRssiValue(100);
    onNewSattelitesNumber(10);
    onNewGPSSignalQuality(50);
    onNewEnginesState(false);
    mapWidget->newGPSPosition(QPointF(53,14));
    mapWidget->newGPSPosition(QPointF(16.951143,52.401470));

    QTimer::singleShot(500, this, SLOT(showFullScreen()));
}


MainWindow::~MainWindow()
{
    delete monitor;
    delete ui;
    delete mapWidget;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        showMaximized();
        break;
    default:
        MainWindow::keyPressEvent(event);
        break;
    }
}

void MainWindow::showData(QByteArray data)
{

    //QString text("Ostatnia ramka: ");
    QString FrameStr(data.toHex());
    for(int i = 1; i< FrameStr.size(); ++i)
    {
        if(i % 3)
        {
            i++;
            FrameStr.insert(i," ");
        }
    }
 //   text.append(FrameStr);
 //   ui->plainTextEditLog->selectAll();
    ui->plainTextEditLog->insertPlainText(FrameStr + QString("\n"));
}


void MainWindow::onNewBateryVoltage(float value)
{
    ui->bateriabar->setValue(value);
    QString text("Robot: " + QString::number(value) + "V");
    QPalette palette;
    if(value < 21.6 || value > 28.2 ) palette.setColor(QPalette::Foreground,Qt::red);
    else palette.setColor(QPalette::Foreground,Qt::black);
    ui->label_Batery->setPalette(palette);
    ui->label_Batery->setText(text);

}

void MainWindow::onNewRssiValue(float value)
{
    //ui->groupBox_RSSI->setTitle(QString("RSSI ") + QString::number(value));
    ui->progressBar_RobotRSSI->setValue(value * 100);
}

void MainWindow::onNewScalesEx(int value)
{
    float alfa = 1; //uzupełnić przelicznik i ustawić w konstruktorze maximum
    float target = 100;
    float percent = value / target * 100.00;
    float weight = alfa*value;
    QString text("Ex: " + QString::number(weight) + "g");
    QPalette palette;
    if(percent >= 100) palette.setColor(QPalette::Foreground,Qt::green);
    else palette.setColor(QPalette::Foreground,Qt::black);
    ui->label_scales_Ex->setPalette(palette);
    ui->label_scales_Ex->setText(text);
    if(percent > 100) percent =100;
    ui->progressBar_Scales_Ex->setValue(percent);

}

void MainWindow::onNewScalesDrill(int value)
{
    float alfa = 1; //uzupełnić przelicznik i ustawić w konstruktorze maximum
    float target = 100;
    float percent = value / target * 100.00;
    float weight = alfa*value;
    QString text("Drill: " + QString::number(weight) + "g");
    QPalette palette;
    if(percent >= 100) palette.setColor(QPalette::Foreground,Qt::green);
    else palette.setColor(QPalette::Foreground,Qt::black);
    ui->label_scales_drill->setPalette(palette);
    ui->label_scales_drill->setText(text);
    if(percent > 100) percent =100;
    ui->progressBar_Scales_drill->setValue(percent);
}

void MainWindow::onNewEnginesState(bool working)
{
    if(working)
    {
        ui->workIndicator_Engine->setGreen();
        ui->label_Engine->setText("Silniki załączone");
    }
    else
    {
        ui->workIndicator_Engine->setRed();
        ui->label_Engine->setText("Silniki wyłączone");
    }
}

void MainWindow::onNewReceiverBateryVoltage(float value)
{
//    float percent = (value-ui->bateriabar->getMin()) / ui->bateriabar->getMax() * 100.00;
    ui->bateriabar_receiver->setValue(value);
    QString text("Nadajnik: " + QString::number(value) + "V");
    QPalette palette;
    if(value < ui->bateriabar_receiver->getLowerGreenLimit() || value > ui->bateriabar_receiver->getUpperGreenLimit() ) palette.setColor(QPalette::Foreground,Qt::red);
    else palette.setColor(QPalette::Foreground,Qt::black);
    ui->label_receiver->setPalette(palette);
    ui->label_receiver->setText(text);
}

void MainWindow::onNewSattelitesNumber(int number)
{
    ui->label_numberOfSattelites->setText(QString("Liczba dostępnych satelitów: ") + QString::number(number));
}

void MainWindow::onNewGPSSignalQuality(int value)
{
    float percent = 1.0 / (float) value * 100.0;
    ui->progressBar_signalQuality->setValue(percent);
    ui->label_signalQuality->setText(QString("Jakość sygnału: ") + QString::number(value));
}
