#ifndef MAPWINDOW_H
#define MAPWINDOW_H

#include <QDialog>

#include "QMapControl.h"
#include "LayerMapAdapter.h"
#include <MapAdapterOSM.h>
#include <MapAdapterGoogle.h>
#include <GeometryLineString.h>
#include <LayerGeometry.h>
#include <GeometryPoint.h>

using namespace qmapcontrol;

namespace Ui {
class MapWindow;
}

class MapWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MapWindow(QWidget *parent = 0);
    ~MapWindow();
public slots:
    void newGPSPosition(float latitude, float longitude);
private slots:
    void drawPath();
private:
    Ui::MapWindow *ui;
    QMapControl *map;
    std::shared_ptr<LayerGeometry> pathLayer;
    std::vector<std::shared_ptr<GeometryPoint>> points;
};

#endif // MAPWINDOW_H
