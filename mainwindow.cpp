#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include <QPointF>
#include "mapwidget.h"
#include "addcheckpointdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    monitor = new PortMonitor(this);
    mapWidget = new GpsDevice(this);
    ui->widget_map->layout()->addWidget(mapWidget);
    setWindowTitle("CUD 2015");

    log = new DialogLog(this);
    //przyciski
    connect(ui->pushButtonConnect,SIGNAL(clicked()),monitor,SLOT(openSerialPort()));
    connect(ui->pushButtonPreferences,SIGNAL(clicked()),monitor->settings,SLOT(show()));
    connect(ui->pushButtonDisconnect,SIGNAL(clicked()),monitor,SLOT(closeSerialPort()));
    connect(ui->pushButtonCheckpoints,SIGNAL(clicked()),mapWidget,SLOT(chooseNewCheckPointsFile()));
    connect(ui->pushButton_fullscreen,SIGNAL(clicked()),this,SLOT(showFullScreen()));
    connect(ui->pushButton_layerImage,SIGNAL(clicked()),mapWidget,SLOT(chooseNewMapFile()));
    connect(ui->pushButtonLog,SIGNAL(clicked()),log,SLOT(show()));
    connect(ui->pushButton_addCheckPoint,SIGNAL(clicked(bool)),mapWidget,SLOT(setPointSelectMode(bool)));
    //sygnały wysyłane, przez monitor portu
    connect(monitor,SIGNAL(newDataArrived(QByteArray)),log,SLOT(appendFrame(QByteArray)));
    connect(monitor,SIGNAL(newCompassValue(float)),mapWidget,SLOT(newCompassAngle(float)));
    connect(monitor,SIGNAL(newGPS(PointWorldCoord)),mapWidget,SLOT(newGPSPosition(PointWorldCoord)));
    connect(monitor,SIGNAL(connectionEstablished()),ui->ConnctionIndicator,SLOT(setGreen()));
    connect(monitor,SIGNAL(connectionClosed()),ui->ConnctionIndicator,SLOT(setRed()));
    connect(monitor,SIGNAL(newRssiRx(int)),ui->progressBar_Rx,SLOT(setValue(int)));
    connect(monitor,SIGNAL(newRssiTx(int)),ui->progressBar_Tx,SLOT(setValue(int)));
    connect(monitor,SIGNAL(newBateryVoltage(float)),this,SLOT(onNewBateryVoltage(float)));
    connect(monitor,SIGNAL(newRssiValue(float)),this,SLOT(onNewRssiValue(float)));
    connect(monitor,SIGNAL(newNumberOfSatelites(int)),this,SLOT(onNewSattelitesNumber(int)));
    connect(monitor,SIGNAL(newGPSSignalQuality(int)),this,SLOT(onNewGPSSignalQuality(int)));
    connect(monitor,SIGNAL(newWorkingEnginesValue(bool)),this,SLOT(onNewEnginesState(bool)));
    connect(monitor,SIGNAL(newRecieverBatteryValue(float)),this,SLOT(onNewReceiverBateryVoltage(float)));
    connect(monitor,SIGNAL(newNumberOfSatelites(int)),ui->progressBar_numberOfSatelittes,SLOT(setValue(int)));
    connect(monitor,SIGNAL(newGPSDataValid(bool)),ui->WorkIndicator_GPS,SLOT(setState(bool)));
    connect(monitor,SIGNAL(newRssiRx(int)),this,SLOT(onNewRssiRx(int)));
    connect(monitor,SIGNAL(newRssiTx(int)),this,SLOT(onNewRssiTx(int)));
    connect(monitor,SIGNAL(newLimitsState(uchar)),this,SLOT(onNewLimitsState(uchar)));
    connect(monitor,SIGNAL(newAdcValue(int,int)),this,SLOT(onNewAdcValue(int,int)));
    connect(monitor,SIGNAL(newSonarFarLeftValue(int)),ui->progressBar_sonarFarLeft,SLOT(setValue(int)));
    connect(monitor,SIGNAL(newSonarCenterLeftValue(int)),ui->progressBar_sonarLeft,SLOT(setValue(int)));
    connect(monitor,SIGNAL(newSonarCentralRightValue(int)),ui->progressBar_SonarRight,SLOT(setValue(int)));
    connect(monitor,SIGNAL(newSonarFarRightValue(int)),ui->progressBar_sonarFarRight,SLOT(setValue(int)));
    connect(monitor,SIGNAL(newXAxisValue(int)),ui->progressBar_servoXAxis,SLOT(setValue(int)));
    connect(monitor,SIGNAL(newYAxisValue(int)),ui->progressBar_servoYAxis,SLOT(setValue(int)));
    connect(monitor,SIGNAL(newCameraPosition(int)),ui->progressBar_servoCam,SLOT(setValue(int)));
    connect(monitor,SIGNAL(newToolPosition(int)),ui->progressBar_servoTool,SLOT(setValue(int)));
    connect(monitor,SIGNAL(newCompassValue(float)),ui->widget_compass,SLOT(setAngle(float)));
    connect(monitor,SIGNAL(newCompassValue(float)),this,SLOT(onNewCompassValue(float)));
    connect(monitor,SIGNAL(newMysteryServoValue(int)),ui->progressBar_servoMystery,SLOT(setValue(int)));
    //dostosowanie kontrolek
    ui->bateriabar->setMaximum(30);
    ui->progressBar_Rx->setMaximum(127);
    ui->progressBar_Tx->setMaximum(127);
    ui->progressBar_RobotRSSI->setMaximum(310);
    ui->checkBox_follow->setChecked(false);
    ui->bateriabar_receiver->setMaximum(13.5f);
    ui->bateriabar_receiver->setMin(9.0f);
    ui->bateriabar_receiver->setUpperGreenLimit(12.8);
    ui->bateriabar_receiver->setLowerGreenLimit(10);
    ui->bateriabar_receiver->setYeallowLimit(10.5);
    ui->progressBar_numberOfSatelittes->setMaximum(12);
    ui->progressBar_sonarFarLeft->setMaximum(255);
    ui->progressBar_sonarLeft->setMaximum(255);
    ui->progressBar_SonarRight->setMaximum(255);
    ui->progressBar_sonarFarRight->setMaximum(255);
    ui->progressBar_servoXAxis->setMaximum(4500);
    ui->progressBar_servoXAxis->setMinimum(2000);
    ui->progressBar_servoYAxis->setMaximum(4500);
    ui->progressBar_servoYAxis->setMinimum(2000);
    ui->progressBar_servoCam->setMaximum(4500);
    ui->progressBar_servoCam->setMinimum(2000);
    ui->progressBar_servoTool->setMaximum(4500);
    ui->progressBar_servoTool->setMinimum(2000);
    ui->progressBar_servoMystery->setMinimum(2000);
    ui->progressBar_servoMystery->setMaximum(4500);
    ui->progressBar_adc1->setMaximum(1024);
    ui->progressBar_adc2->setMaximum(1024);
    ui->progressBar_adc3->setMaximum(1024);
    ui->progressBar_adc4->setMaximum(1024);
    ui->progressBar_adc5->setMaximum(1024);
    ui->progressBar_adc6->setMaximum(1024);
    ui->progressBar_adc7->setMaximum(1024);
    ui->checkBox_autoReconnect->setChecked(true);

    //wysyłane przez widgety
    connect(ui->checkBox_follow,SIGNAL(clicked(bool)),mapWidget,SLOT(setFollowingRower(bool)));
    //miejsce konkursu
    AreaDialog = new AreaSettingsDialog(this);
    mapWidget->setCompetitionArea(AreaDialog->getPolygon());
    connect(ui->pushButton_area,SIGNAL(clicked()),AreaDialog,SLOT(show()));
    connect(AreaDialog,SIGNAL(areaChanged(std::shared_ptr<GeometryPolygon>)),mapWidget,SLOT(setCompetitionArea(std::shared_ptr<GeometryPolygon>)));
    connect(this,SIGNAL(pointSelected(PointWorldCoord)),AreaDialog,SLOT(onPointSelected(PointWorldCoord)));
    mapWidget->drawArea();
    //mysza
    connect(mapWidget,SIGNAL(mouseEventMoveCoordinate(QMouseEvent*,PointWorldCoord,PointWorldCoord)),
            this,SLOT(displayMouseCoords(QMouseEvent*,PointWorldCoord,PointWorldCoord)));
    connect(mapWidget,SIGNAL(mouseEventPressCoordinate(QMouseEvent*,PointWorldCoord)),this,SLOT(onMapPointSelected(QMouseEvent*,PointWorldCoord)));

    //zmiana rozmiaru i przesunięcie
//    QRect rec = QApplication::desktop()->screenGeometry();
//    move(rec.width()/2,0);
//    resize(rec.width()/2,rec.height()*0.9);
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
    onNewRssiRx(100);
    onNewRssiTx(30);
    onNewLimitsState(2);
    onNewBateryVoltage(0);
//    mapWidget->newGPSPosition(PointWorldCoord(53,14));
    mapWidget->newGPSPosition(PointWorldCoord(16.951143,52.401470));
//    mapWidget->newCompassAngle(90);
    showMaximized();

}


MainWindow::~MainWindow()
{
    delete monitor;
    delete ui;
    delete mapWidget;
    delete log;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        showMaximized();
        break;
    default:
        break;
    }
}


void MainWindow::onNewBateryVoltage(float value)
{
    ui->bateriabar->setValue(value);
    QString text("Rover:\n" + QString::number(value,'f',2) + "V");
    QPalette palette;
    if(value < 21.6 || value > 28.2 ) palette.setColor(QPalette::Foreground,Qt::red);
    else palette.setColor(QPalette::Foreground,Qt::black);
    ui->label_Batery->setPalette(palette);
    ui->label_Batery->setText(text);
}

void MainWindow::onNewRssiValue(float value)
{
    ui->label_RobotRSSI->setText(QString("Rover:\n") + QString::number(value,'f',2));
    ui->progressBar_RobotRSSI->setValue(value * 100);
}

void MainWindow::onNewScalesEx(int value)
{
    Q_UNUSED(value);
//    float alfa = 1; //uzupełnić przelicznik i ustawić w konstruktorze maximum
//    float target = 100;
//    float percent = value / target * 100.00;
//    float weight = alfa*value;
//    QString text("Ex:\n" + QString::number(weight,'f',2) + "g");
//    QPalette palette;
//    if(percent >= 100) palette.setColor(QPalette::Foreground,Qt::green);
//    else palette.setColor(QPalette::Foreground,Qt::black);
//    ui->label_scales_Ex->setPalette(palette);
//    ui->label_scales_Ex->setText(text);
//    if(percent > 100) percent =100;
//    ui->progressBar_Scales_Ex->setValue(percent);
}

void MainWindow::onNewScalesDrill(int value)
{
    Q_UNUSED(value);
//    float alfa = 1; //uzupełnić przelicznik i ustawić w konstruktorze maximum
//    float target = 100;
//    float percent = value / target * 100.00;
//    float weight = alfa*value;
//    QString text("Drill:\n" + QString::number(weight,'f',2) + "g");
//    QPalette palette;
//    if(percent >= 100) palette.setColor(QPalette::Foreground,Qt::green);
//    else palette.setColor(QPalette::Foreground,Qt::black);
//    ui->label_scales_drill->setPalette(palette);
//    ui->label_scales_drill->setText(text);
//    if(percent > 100) percent =100;
//    ui->progressBar_Scales_drill->setValue(percent);
}

void MainWindow::onNewEnginesState(bool working)
{
    if(working)
    {
        ui->workIndicator_Engine->setGreen();
        ui->label_Engine->setText("Engines online");
    }
    else
    {
        ui->workIndicator_Engine->setRed();
        ui->label_Engine->setText("Engines offline");
    }
}

void MainWindow::onNewReceiverBateryVoltage(float value)
{
//    float percent = (value-ui->bateriabar->getMin()) / ui->bateriabar->getMax() * 100.00;
    ui->bateriabar_receiver->setValue(value);
    QString text("Transmitter:\n" + QString::number(value,'f',2) + "V");
    QPalette palette;
    if(value < ui->bateriabar_receiver->getLowerGreenLimit() || value > ui->bateriabar_receiver->getUpperGreenLimit() ) palette.setColor(QPalette::Foreground,Qt::red);
    else palette.setColor(QPalette::Foreground,Qt::black);
    ui->label_receiver->setPalette(palette);
    ui->label_receiver->setText(text);
}

void MainWindow::onNewSattelitesNumber(int number)
{
    ui->label_numberOfSattelites->setText(QString("Avalible sattelites::\n") + QString::number(number,'g',2));
}

void MainWindow::onNewGPSSignalQuality(int value)
{
    float percent = 100 - value;
    ui->progressBar_signalQuality->setValue(percent);
    ui->label_signalQuality->setText(QString("Signal quality:\n") + QString::number(value,'g',2));
}

void MainWindow::displayMouseCoords(QMouseEvent*e, PointWorldCoord press,PointWorldCoord mov)
{
    Q_UNUSED(e);
    Q_UNUSED(press);

    if(ui->comboBox_mousePositionDisplayMode->currentText() == "DMS")
    {
        DegMinSec lat(mov.latitude());
        DegMinSec lon(mov.longitude());

        ui->label_mousePosition->setText(QString("lon= ")
                                         + lon.toDMSString()
                                         + QString(" lat = ")
                                         + lat.toDMSString());
    }

    else ui->label_mousePosition->setText(QString("Pouse position: lat= ")
                                     + QString::number(mov.latitude())
                                     + QString(" long = ")
                                          + QString::number(mov.longitude()));
}


void MainWindow::onMapPointSelected(QMouseEvent *e, PointWorldCoord point)
{
    Q_UNUSED(e);
    emit pointSelected(point);
}

void MainWindow::onNewRssiRx(int value)
{
    ui->label_Rx->setText(QString("Rx:\n") + QString::number(value,'g',3));
}

void MainWindow::onNewRssiTx(int value)
{
    ui->label_Tx->setText(QString("Tx:\n") + QString::number(value,'g',3));
}

void MainWindow::onNewLimitsState(uchar value)
{
    //trzy ostatnie bity
    const static int First = 1;
    const static int Second = 1<<1;
    const static int Thrid = 1<<2;

    bool state = !(value & First);
    ui->workindicator_limit1->setState(state);
    state = !(value & Second);
    ui->workindicator_limit2->setState(state);
    state = !(value & Thrid);
    ui->workindicator_limit3->setState(state);
}

void MainWindow::onNewAdcValue(int adc, int value)
{

    switch (adc) {
    case 1:
        ui->progressBar_adc1->setValue(value);
        break;
    case 2:
        ui->progressBar_adc2->setValue(value);
        break;
    case 3:
        ui->progressBar_adc3->setValue(value);
        break;
    case 4:
        ui->progressBar_adc4->setValue(value);
        break;
    case 5:
    {
        float Voltage = 0.03880514 * (float) value + 0.02098332;
        ui->label_ADC5Value->setText("ADC5: " + QString::number(value) + " :" + QString::number(Voltage,'f',2) + "V");
        ui->progressBar_adc5->setValue(value);
        break;
    }
    case 6:
        ui->progressBar_adc6->setValue(value);
        break;
    case 7:
        ui->progressBar_adc7->setValue(value);
        break;
    default:
        break;
    }
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    QSizeF WidgetSize = ui->widget_map->size();
    mapWidget->setViewportSize(WidgetSize);
}

void MainWindow::on_pushButton_addCheckPoint_clicked()
{
   AddCheckPointDialog *dial = new AddCheckPointDialog(this);
   dial->setAttribute(Qt::WA_DeleteOnClose);
   connect(dial,SIGNAL(newWayPointAdded(PointWorldCoord,QString)),mapWidget,SLOT(addCheckPoint(PointWorldCoord,QString)));
   dial->show();
   connect(mapWidget,SIGNAL(mouseEventPressCoordinate(QMouseEvent*,PointWorldCoord)),dial,SLOT(onFromMap(QMouseEvent*,PointWorldCoord)));
}

void MainWindow::onNewCompassValue(float value)
{
    ui->label_Angle->setText("Compass: " + QString::number( value));
}


