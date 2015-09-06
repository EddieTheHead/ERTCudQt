#include "addcheckpointdialog.h"
#include "ui_addcheckpointdialog.h"
#include "degminsec.h"
#include <QRegExp>

AddCheckPointDialog::AddCheckPointDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddCheckPointDialog)
{
    ui->setupUi(this);
    AwaitingForCoords = false;
    ui->lineEdit_latitude->setText("52°24'08.2''N");
    ui->lineEdit_longitude->setText("16°57'07.0''E");
}

AddCheckPointDialog::~AddCheckPointDialog()
{
    delete ui;
}

void AddCheckPointDialog::on_buttonBox_accepted()
{
    QString name = ui->lineEdit_name->text();
    QRegExp latRx("(\\d{1,3})[°dh,\\s]{1,}(\\d{1,3})['m,\\s]{1,}(\\d{1,3}.\\d{1,})[s,'' ]{1,2}");
    latRx.exactMatch(ui->lineEdit_latitude->text());
    QStringList capturedLat = latRx.capturedTexts();
    if(capturedLat.size() != 4) return;
    DegMinSec latitude(capturedLat.at(1).toInt(),capturedLat.at(2).toInt(),capturedLat.at(3).toFloat());

    QRegExp lonRx("(\\d{1,3})[°dh,\\s]{1,}(\\d{1,3})['m,\\s]{1,}(\\d{1,3}.\\d{1,})[s,'' ]{1,2}");
    lonRx.exactMatch(ui->lineEdit_longitude->text());
    QStringList capturedLon = lonRx.capturedTexts();
    if(capturedLon.size() != 4) return;
    DegMinSec longitude(capturedLon.at(1).toInt(),capturedLon.at(2).toInt(),capturedLon.at(3).toFloat());

    emit newWayPointAdded(PointWorldCoord(longitude.toDecimalDegress(),latitude.toDecimalDegress()),name);
}

void AddCheckPointDialog::on_pushButton_clicked()
{
    this->hide();
    AwaitingForCoords = true;
}

void AddCheckPointDialog::onFromMap(QMouseEvent *, PointWorldCoord coord)
{

    emit newWayPointAdded(coord,ui->lineEdit_name->text());
    AwaitingForCoords = false;
}


