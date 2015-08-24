#include "areasettingsdialog.h"
#include "ui_areasettingsdialog.h"
#include <QDebug>

AreaSettingsDialog::AreaSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AreaSettingsDialog)
{
    ui->setupUi(this);
    restore();
    connect(ui->pushButton_restore,SIGNAL(clicked()),this,SLOT(restore()));
    connect(ui->pushButton_apply,SIGNAL(clicked()),this,SLOT(save()));
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(save()));
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(close()));
}

AreaSettingsDialog::~AreaSettingsDialog()
{
    delete ui;
}
std::shared_ptr<GeometryPolygon> AreaSettingsDialog::getPolygon() const
{
    return polygon;
}

void AreaSettingsDialog::save()
{
    qDebug() << "zapisuję";
    PointWorldCoord p1(ui->lineEdit_p1->text().split(",",QString::SkipEmptyParts).at(0).toDouble(),
                     ui->lineEdit_p1->text().split(",",QString::SkipEmptyParts).at(1).toDouble());
    PointWorldCoord p2(ui->lineEdit_p2->text().split(",",QString::SkipEmptyParts).at(0).toDouble(),
                     ui->lineEdit_p2->text().split(",",QString::SkipEmptyParts).at(1).toDouble());
    PointWorldCoord p3(ui->lineEdit_p3->text().split(",",QString::SkipEmptyParts).at(0).toDouble(),
                     ui->lineEdit_p3->text().split(",",QString::SkipEmptyParts).at(1).toDouble());
    PointWorldCoord p4(ui->lineEdit_p4->text().split(",",QString::SkipEmptyParts).at(0).toDouble(),
                     ui->lineEdit_p4->text().split(",",QString::SkipEmptyParts).at(1).toDouble());
    //qDebug() << "p1: " << p1.latitude() << p1.longitude();
    points.clear();
    points.push_back(p1);
    points.push_back(p2);
    points.push_back(p3);
    points.push_back(p4);
    //qDebug() << "pierwszy element w points: " << "lon:"<< points.at(0).longitude() << "lat: " << points.at(0).latitude() ;
    createPolygon();
    emit newAreaSelected();
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
    emit newAreaSelected();
}

void AreaSettingsDialog::createPolygon()
{
    polygon = std::make_shared<GeometryPolygon>(points);
}

void AreaSettingsDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    emit onHide();
    saveGeometry();
}



