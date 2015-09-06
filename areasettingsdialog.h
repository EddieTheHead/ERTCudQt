#ifndef AREASETTINGSDIALOG_H
#define AREASETTINGSDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QMapControl.h>
#include <GeometryPolygon.h>
#include <GeometryPoint.h>
#include <memory>

namespace Ui {
class AreaSettingsDialog;
}

using namespace qmapcontrol;

class AreaSettingsDialog : public QDialog
{
    Q_OBJECT
signals:
    void onHide();
    void areaChanged(std::shared_ptr<GeometryPolygon>);
public:
    explicit AreaSettingsDialog(QWidget *parent = 0);
    ~AreaSettingsDialog();

    std::shared_ptr<GeometryPolygon> getPolygon() const;
public slots:
    void onPointSelected(PointWorldCoord point);
private slots:
    void loadFromLineEdits();
    void restore();
    void createPolygon();
    void selectFromMap();
    void closeEvent(QCloseEvent *event);

    void on_pushButton_saveToFile_clicked();

private:
    void fillLineEdits();

    Ui::AreaSettingsDialog *ui;
    std::shared_ptr<GeometryPolygon> polygon;
    std::vector<PointWorldCoord> points;
    int PointToSelectOnMap;


};

#endif // AREASETTINGSDIALOG_H
