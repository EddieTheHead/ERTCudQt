#ifndef MAPWINDOW_H
#define MAPWINDOW_H

#include <QDialog>
#include <QFileDialog>

#include "QMapControl.h"
#include "LayerMapAdapter.h"
#include <MapAdapterOSM.h>
#include <MapAdapterGoogle.h>
#include <GeometryLineString.h>
#include <LayerGeometry.h>
#include <GeometryPoint.h>
#include <QPointF>
#include <QTimer>

#include <QMessageBox>

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
    void newGPSPosition(QPointF pos);
    void newCompassAngle(float angle);
    void chooseNewCheckPointsFile();
private slots:
    void drawPath();
    void drawDirection(float latitude, float longitude, float angle);
    void drawCheckpoints();
    void drawMap();

private:
    QTimer DrawTimer;
    int RefresDelay;
    bool readCheckpoints(QString fileName); // Funkcja czytująca punkty z pliku
    Ui::MapWindow *ui;
    QMapControl *map;
    std::shared_ptr<LayerGeometry> pathLayer;
    std::shared_ptr<LayerGeometry> pointerLayer;
    std::vector<std::shared_ptr<GeometryPoint>> points;
    std::vector<std::shared_ptr<GeometryPoint>> checkPointsList;
    QImage pointerImage;
    std::shared_ptr<GeometryPoint> currentPosition;
};


#endif // MAPWINDOW_H
