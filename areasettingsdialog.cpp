#include "areasettingsdialog.h"
#include "ui_areasettingsdialog.h"
#include <QDebug>
#include <QRegExp>

AreaSettingsDialog::AreaSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AreaSettingsDialog)
{
    ui->setupUi(this);
    restore();
    connect(ui->pushButton_restore,SIGNAL(clicked()),this,SLOT(restore()));
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->pushButton_p1,SIGNAL(clicked()),this,SLOT(selectFromMap()));
    connect(ui->pushButton_p2,SIGNAL(clicked()),this,SLOT(selectFromMap()));
    connect(ui->pushButton_p3,SIGNAL(clicked()),this,SLOT(selectFromMap()));
    connect(ui->pushButton_p4,SIGNAL(clicked()),this,SLOT(selectFromMap()));
    connect(ui->lineEdit_p1,SIGNAL(editingFinished()),this,SLOT(loadFromLineEdits()));
    connect(ui->lineEdit_p2,SIGNAL(editingFinished()),this,SLOT(loadFromLineEdits()));
    connect(ui->lineEdit_p3,SIGNAL(editingFinished()),this,SLOT(loadFromLineEdits()));
    connect(ui->lineEdit_p4,SIGNAL(editingFinished()),this,SLOT(loadFromLineEdits()));

    PointToSelectOnMap = 0;
}

AreaSettingsDialog::~AreaSettingsDialog()
{
    delete ui;
}
std::shared_ptr<GeometryPolygon> AreaSettingsDialog::getPolygon() const
{
    return polygon;
}

void AreaSettingsDialog::onPointSelected(PointWorldCoord point)
{
    if(!PointToSelectOnMap) return;
    points.at(PointToSelectOnMap-1) = point;
    PointToSelectOnMap = 0;
    createPolygon();
    show();
    emit areaChanged(polygon);
}

void AreaSettingsDialog::loadFromLineEdits()
{
    qDebug() << "void AreaSettingsDialog::loadFromLineEdits()";
    QLineEdit *edit = qobject_cast<QLineEdit *>(sender());
    int position = 5;
    if(edit == ui->lineEdit_p1) position =0;
    else if(edit == ui->lineEdit_p2) position = 1;
    else if(edit == ui->lineEdit_p3) position = 2;
    else if(edit == ui->lineEdit_p4) position = 3;

    QRegExp rx("(\\d{1,3}\\.\\d{1,})[,|\\s]{1,}(\\d{1,3}\\.\\d{1,})");
    rx.exactMatch(edit->text());
    QStringList captured = rx.capturedTexts();

    QPalette palette;
    if(captured.size() == 3 && rx.exactMatch(edit->text()))
    {
        double latitude = captured.at(1).toDouble();
        qDebug() << captured.at(0);
        double longitude = captured.at(2).toDouble();
        qDebug() << captured.at(1);
        qDebug() << captured.at(2);
        //if(longitude > latitude) std::swap(latitude,longitude);
        PointWorldCoord(longitude,latitude);
        points.at(position) = PointWorldCoord(longitude,latitude);
        createPolygon();
        emit areaChanged(polygon);
        palette.setColor(QPalette::Foreground,Qt::black);;
    }
    else palette.setColor(QPalette::Foreground,Qt::red);
    edit->setPalette(palette);
}

void AreaSettingsDialog::restore()
{
    points.clear();
    points.push_back(PointWorldCoord(16.950447,52.402715));
    points.push_back(PointWorldCoord(16.953567,52.401908));
    points.push_back(PointWorldCoord(16.952194,52.400003));
    points.push_back(PointWorldCoord(16.948782,52.400933));

    ui->lineEdit_p1->setText("16.950447,52.402715");
    ui->lineEdit_p2->setText("16.953567,52.401908");
    ui->lineEdit_p3->setText("16.952194,52.400003");
    ui->lineEdit_p4->setText("16.948782,52.400933");
    createPolygon();
    emit areaChanged(polygon);
}

void AreaSettingsDialog::createPolygon()
{
    polygon = std::make_shared<GeometryPolygon>(points);
}

void AreaSettingsDialog::selectFromMap()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if(button == ui->pushButton_p1) PointToSelectOnMap = 1;
    else if(button == ui->pushButton_p2) PointToSelectOnMap = 2;
    else if(button == ui->pushButton_p3) PointToSelectOnMap = 3;
    else if(button == ui->pushButton_p4) PointToSelectOnMap = 4;
    else return;
    if(PointToSelectOnMap)
    {
        hide();
    }
    fillLineEdits();
}

void AreaSettingsDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    emit onHide();
    saveGeometry();
}

void AreaSettingsDialog::fillLineEdits()
{
    ui->lineEdit_p1->setText(QString::number(points.at(0).longitude(),'f',7) + "," + QString::number(points.at(0).latitude(),'f',7));
    ui->lineEdit_p2->setText(QString::number(points.at(1).longitude(),'f',7) + "," + QString::number(points.at(1).latitude(),'f',7));
    ui->lineEdit_p3->setText(QString::number(points.at(2).longitude(),'f',7) + "," + QString::number(points.at(2).latitude(),'f',7));
    ui->lineEdit_p4->setText(QString::number(points.at(3).longitude(),'f',7) + "," + QString::number(points.at(3).latitude(),'f',7));
}



