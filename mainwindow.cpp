#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    monitor = new PortMonitor(this);
    mapWindow = new MapWindow(this);
    //przyciski
    connect(ui->pushButtonConnect,SIGNAL(clicked()),monitor,SLOT(openSerialPort()));
    connect(ui->pushButtonPreferences,SIGNAL(clicked()),monitor->settings,SLOT(show()));
    connect(ui->pushButtonDisconnect,SIGNAL(clicked()),monitor,SLOT(closeSerialPort()));
    connect(ui->pushButtonCheckpoints,SIGNAL(clicked()),mapWindow,SLOT(chooseNewCheckPointsFile()));
    //sygnały wysyłane, przez monitor portu
    connect(monitor,SIGNAL(newDataArrived(QByteArray)),this,SLOT(showData(QByteArray)));
    connect(monitor,SIGNAL(newLeftVerticaTriggerValue(int)),ui->verticalSliderLeftVerticalTrigger,SLOT(setValue(int)));
    connect(monitor,SIGNAL(newLeftHorizontalTriggerValue(int)),ui->horizontalSliderLeftHorizontalTrigger,SLOT(setValue(int)));
    connect(monitor,SIGNAL(newRightVerticaTriggerValue(int)),ui->verticalSliderRightVerticalTrigger,SLOT(setValue(int)));
    connect(monitor,SIGNAL(newRightHorizontalTriggerValue(int)),ui->horizontalSliderRightHorizontalTrigger,SLOT(setValue(int)));
    connect(monitor,SIGNAL(newLeftTriggerString(QString)),ui->labelLeftTrigger,SLOT(setText(QString)));
    connect(monitor,SIGNAL(newRightTriggerString(QString)),ui->labelRightTrigger,SLOT(setText(QString)));
    connect(monitor,SIGNAL(newCompassValue(float)),mapWindow,SLOT(newCompassAngle(float)));
    //dostosowanie kontrolek
    ui->verticalSliderLeftVerticalTrigger->setMinimum(-100);
    ui->verticalSliderLeftVerticalTrigger->setMaximum(100);
    ui->verticalSliderRightVerticalTrigger->setMinimum(-100);
    ui->verticalSliderRightVerticalTrigger->setMaximum(100);
    ui->horizontalSliderLeftHorizontalTrigger->setMinimum(-100);
    ui->horizontalSliderLeftHorizontalTrigger->setMaximum(100);
    ui->horizontalSliderRightHorizontalTrigger->setMinimum(-100);
    ui->horizontalSliderRightHorizontalTrigger->setMaximum(100);

    connect(mapWindow,SIGNAL(destroyed()),this,SLOT(setMapButtonText()));
}




MainWindow::~MainWindow()
{
    delete monitor;
    delete ui;
    delete mapWindow;
}

void MainWindow::showData(QByteArray data)
{

    QString text("Ostatnia ramka: ");
    QString FrameStr(data.toHex());
    text.append(FrameStr);
    ui->plainTextEditLog->selectAll();
    ui->plainTextEditLog->insertPlainText(text);

}

void MainWindow::on_pushButtonMap_clicked()
{
    if(!mapWindow->isVisible()) mapWindow->show();
    else mapWindow->hide();
    setMapButtonText();

}


void MainWindow::setMapButtonText()
{
    if(!mapWindow->isVisible()) ui->pushButtonMap->setText("Pokaż mapę");
    else ui->pushButtonMap->setText("Ukryj Mapę");
}
