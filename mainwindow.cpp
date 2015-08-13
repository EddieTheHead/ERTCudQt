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
    connect(monitor,SIGNAL(newBateryVoltage(float)),this,SLOT(onNewBateryVoltage(float)));
    //dostosowanie kontrolek
    ui->bateriabar->setMaximum(30);
    //zmiana rozmiaru i przesunięcie
    QRect rec = QApplication::desktop()->screenGeometry();
    move(rec.width()/2,0);
    resize(rec.width()/2,rec.height()*0.9);
    mapWindow->show();
    ui->pushButtonMap->setText("Ukryj Mapę");
    //testy
    onNewBateryVoltage(18);
    onNewScalesEx(101);
    onNewScalesDrill(40);
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
    else
    {
        mapWindow->hide();
        ui->pushButtonMap->setText("Pokaż Mapę");
    }
}


void MainWindow::setMapButtonText()
{
    ui->pushButtonMap->setText("Pokaż mapę");
}

void MainWindow::onNewBateryVoltage(float value)
{
//    float percent = (value-ui->bateriabar->getMin()) / ui->bateriabar->getMax() * 100.00;
    ui->bateriabar->setValue(value);
    QString text("Bateria:" + QString::number(value) + "V");
    QPalette palette;
    if(value < 21.6 || value > 28.2 ) palette.setColor(QPalette::Foreground,Qt::red);
    else palette.setColor(QPalette::Foreground,Qt::black);
    ui->label_Batery->setPalette(palette);
    ui->label_Batery->setText(text);

}

void MainWindow::onNewScalesEx(int value)
{
    float alfa = 1; //uzupełnić przelicznik i ustawić w konstruktorze maximum
    float target = 100;
    float percent = value / target * 100.00;
    float weight = alfa*value;
    QString text("Waga Ex: " + QString::number(weight) + "g");
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
    QString text("Waga Drill: " + QString::number(weight) + "g");
    QPalette palette;
    if(percent >= 100) palette.setColor(QPalette::Foreground,Qt::green);
    else palette.setColor(QPalette::Foreground,Qt::black);
    ui->label_scales_drill->setPalette(palette);
    ui->label_scales_drill->setText(text);
    if(percent > 100) percent =100;
    ui->progressBar_Scales_drill->setValue(percent);
}

