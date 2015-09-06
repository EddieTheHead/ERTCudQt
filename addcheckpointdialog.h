#ifndef ADDCHECKPOINTDIALOG_H
#define ADDCHECKPOINTDIALOG_H

#include <QDialog>
#include <GeometryPoint.h>
#include <memory>

namespace Ui {
class AddCheckPointDialog;
}

using namespace qmapcontrol;

class AddCheckPointDialog : public QDialog
{
    Q_OBJECT
signals:
    void newWayPointAdded(PointWorldCoord,QString);

public:
    explicit AddCheckPointDialog(QWidget *parent = 0);
    ~AddCheckPointDialog();

private slots:
    void on_buttonBox_accepted();

    void on_pushButton_clicked();
    void onFromMap(QMouseEvent *, PointWorldCoord coord);
private:
    Ui::AddCheckPointDialog *ui;
    bool AwaitingForCoords;

};

#endif // ADDCHECKPOINTDIALOG_H
