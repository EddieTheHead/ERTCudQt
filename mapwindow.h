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
#include <QWindow>
#include <QMessageBox>
#include <GeometryPolygon.h>
#include <areasettingsdialog.h>

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
    bool getFollowingRower() const;
signals:
    void onHide();

public slots:
    void setFollowingRower(bool value);
    void newGPSPosition(QPointF pos);
    void newCompassAngle(float angle);
    void chooseNewCheckPointsFile();
    void onNewSattelitesNumber(int number);
    void onNewGPSSignalQuality(int value);

private slots:
    void drawPath();
    void drawDirection(float latitude, float longitude, float angle);
    void drawCheckpoints();
    void drawMap();
    void drawArea();
    void closeEvent(QCloseEvent *event);
    void displayCoursorCoords(QMouseEvent *event, PointWorldCoord coords);
private:
    bool readCheckpoints(QString fileName); // Funkcja czytująca punkty z pliku
    void createLayers();
    QTimer DrawTimer;
    int RefresDelay;
    Ui::MapWindow *ui;
    QMapControl *map;
    std::shared_ptr<LayerGeometry> pathLayer;
    std::shared_ptr<LayerGeometry> pointerLayer;
    std::shared_ptr<LayerGeometry> areaLayer;
    std::vector<std::shared_ptr<GeometryPoint>> points;
    std::vector<std::shared_ptr<GeometryPoint>> checkPointsList;
    QImage pointerImage;
    std::shared_ptr<GeometryPoint> currentPosition;
    bool followingRower;
    std::shared_ptr<PointWorldCoord> ERC;
    std::shared_ptr<GeometryPolygon> CompetitionArea;
    AreaSettingsDialog *AreaDialog;
};


#endif // MAPWINDOW_H
