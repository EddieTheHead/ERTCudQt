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
    void newAreaSelected();
public:
    explicit AreaSettingsDialog(QWidget *parent = 0);
    ~AreaSettingsDialog();

    std::shared_ptr<GeometryPolygon> getPolygon() const;

private slots:
    void save();
    void restore();
    void createPolygon();
    void closeEvent(QCloseEvent *event);

private:
    Ui::AreaSettingsDialog *ui;
    std::shared_ptr<GeometryPolygon> polygon;
    std::vector<PointWorldCoord> points;


};

#endif // AREASETTINGSDIALOG_H
