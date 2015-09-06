#include "areasettingsdialog.h"
#include "ui_areasettingsdialog.h"
#include <QDebug>
#include <QRegExp>
#include "degminsec.h"
#include <QTextStream>
#include<QFileDialog>

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

    connect(ui->lineEdit_p1,SIGNAL(returnPressed()),ui->pushButton_ok,SLOT(setFocus()));
    connect(ui->lineEdit_p2,SIGNAL(returnPressed()),ui->pushButton_ok,SLOT(setFocus()));
    connect(ui->lineEdit_p3,SIGNAL(returnPressed()),ui->pushButton_ok,SLOT(setFocus()));
    connect(ui->lineEdit_p4,SIGNAL(returnPressed()),ui->pushButton_ok,SLOT(setFocus()));

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
    fillLineEdits();
    show();
    emit areaChanged(polygon);
}

void AreaSettingsDialog::loadFromLineEdits()
{
    qDebug() << "void AreaSettingsDialog::loadFromLineEdits()";
    QLineEdit *edit = qobject_cast<QLineEdit *>(sender());
    int position = 5;
    if(edit == ui->lineEdit_p1) position = 0;
    else if(edit == ui->lineEdit_p2) position = 1;
    else if(edit == ui->lineEdit_p3) position = 2;
    else if(edit == ui->lineEdit_p4) position = 3;
    else return;

    QRegExp rx("(\\d{1,3})[°dh,\\s]{1,}(\\d{1,3})['m,\\s]{1,}(\\d{1,3}.\\d{1,})[s,'' ]{1,2}[ x,]{1,}(\\d{1,3})[°dh,\\s]{1,}(\\d{1,3})['m,\\s]{1,}(\\d{1,3}.\\d{1,})[s,']{1,2}");
    rx.exactMatch(edit->text());
    QStringList captured = rx.capturedTexts();

    QPalette palette;

    if(captured.size() == 7 && rx.exactMatch(edit->text()))
    {
        qDebug() << "if(captured.size() == 7 && rx.exactMatch(edit->text())) true";
        DegMinSec longitude(captured.at(1).toInt(),captured.at(2).toInt(),captured.at(3).toFloat());
        DegMinSec latitude(captured.at(4).toInt(),captured.at(5).toInt(),captured.at(6).toFloat());
        qDebug() << "lat: " << latitude.toDMSString();
        qDebug() << "lon: " << longitude.toDMSString();

        points.at(position) = PointWorldCoord(longitude.toDecimalDegress(),latitude.toDecimalDegress());
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
    points.push_back(PointWorldCoord( 20.4617, 50.7831));
    points.push_back(PointWorldCoord( 20.4627, 50.7828));
    points.push_back(PointWorldCoord( 20.4614, 50.782));
    points.push_back(PointWorldCoord( 20.4603, 50.7824));

    ui->lineEdit_p1->setText(DegMinSec(points.at(0).longitude()).toDMSString() + " x " + DegMinSec(points.at(0).latitude()).toDMSString());
    ui->lineEdit_p2->setText(DegMinSec(points.at(1).longitude()).toDMSString() + " x " + DegMinSec(points.at(1).latitude()).toDMSString());
    ui->lineEdit_p3->setText(DegMinSec(points.at(2).longitude()).toDMSString() + " x " + DegMinSec(points.at(2).latitude()).toDMSString());
    ui->lineEdit_p4->setText(DegMinSec(points.at(3).longitude()).toDMSString() + " x " + DegMinSec(points.at(3).latitude()).toDMSString());
    createPolygon();
    emit areaChanged(polygon);
}

void AreaSettingsDialog::createPolygon()
{
    polygon = std::make_shared<GeometryPolygon>(points,0,19);
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
}

void AreaSettingsDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    emit onHide();
    saveGeometry();
}

void AreaSettingsDialog::fillLineEdits()
{
    ui->lineEdit_p1->setText(DegMinSec(points.at(0).longitude()).toDMSString() + "x" + DegMinSec(points.at(0).latitude()).toDMSString());
    ui->lineEdit_p2->setText(DegMinSec(points.at(1).longitude()).toDMSString() + "x" + DegMinSec(points.at(1).latitude()).toDMSString());
    ui->lineEdit_p3->setText(DegMinSec(points.at(2).longitude()).toDMSString() + "x" + DegMinSec(points.at(2).latitude()).toDMSString());
    ui->lineEdit_p4->setText(DegMinSec(points.at(3).longitude()).toDMSString() + "x" + DegMinSec(points.at(3).latitude()).toDMSString());
}




void AreaSettingsDialog::on_pushButton_saveToFile_clicked()
{
    QFile file(QFileDialog::getSaveFileName(this));
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text)) return;
    QTextStream out(&file);
    //out << ui->lineEdit_p1->text() << "\n" << ui->lineEdit_p2->text() << "\n" << ui->lineEdit_p3->text() << "\n" << ui->lineEdit_p4->text() << "\n" ;
    for(const auto &p : points)
    {
        out << p.latitude() << "x" << p.longitude() << "\n";
    }
    file.close();
}
