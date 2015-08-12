#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    monitor = new PortMonitor(this);
    mapWindow = new MapWindow(this);

    setWindowTitle("CUD 2015");
    //przyciski
    connect(ui->pushButtonConnect,SIGNAL(clicked()),monitor,SLOT(openSerialPort()));
    connect(ui->pushButtonPreferences,SIGNAL(clicked()),monitor->settings,SLOT(show()));
    connect(ui->pushButtonDisconnect,SIGNAL(clicked()),monitor,SLOT(closeSerialPort()));
    connect(ui->pushButtonCheckpoints,SIGNAL(clicked()),mapWindow,SLOT(chooseNewCheckPointsFile()));
    connect(mapWindow,SIGNAL(onHide()),this,SLOT(setMapButtonText()));
    //sygnały wysyłane, przez monitor portu
    connect(monitor,SIGNAL(newDataArrived(QByteArray)),this,SLOT(showData(QByteArray)));

    connect(monitor,SIGNAL(newCompassValue(float)),mapWindow,SLOT(newCompassAngle(float)));
    connect(monitor,SIGNAL(newGPS(QPointF)),mapWindow,SLOT(newGPSPosition(QPointF)));
    //dostosowanie kontrolek
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
    if(!mapWindow->isVisible())
    {
        mapWindow->show();
        ui->pushButtonMap->setText("Ukryj Mapę");
    }
    else mapWindow->hide();
}


void MainWindow::setMapButtonText()
{
    ui->pushButtonMap->setText("Pokaż mapę");
}

