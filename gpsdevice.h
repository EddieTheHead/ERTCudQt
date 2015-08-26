#ifndef GPSDEVICE_H
#define GPSDEVICE_H

#include <QFileDialog>
#include <QMapControl.h>
#include <LayerMapAdapter.h>
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

using namespace qmapcontrol;


class GpsDevice:public QMapControl
{
    Q_OBJECT

public:
    explicit GpsDevice(QWidget *parent);
    GpsDevice(QSizeF mapSize,QWidget *parent);
    ~GpsDevice();
    bool getFollowingRower() const;
    std::shared_ptr<GeometryPolygon> getCompetitionArea() const;

public slots:
    void setCompetitionArea(const std::shared_ptr<GeometryPolygon> &value);
    void setFollowingRower(bool value);
    void newGPSPosition(QPointF pos);
    void newCompassAngle(float angle);
    void chooseNewCheckPointsFile();
    void drawArea();


private slots:
    void drawPath();
    void drawDirection(float lat, float longitude, float angle);
    void drawCheckpoints();
    void drawMap();
private:
    //void resizeEvent(QResizeEvent *event);
    bool readCheckpoints(QString fileName); // Funkcja czytująca punkty z pliku
    void createLayers();

    QTimer DrawTimer;
    int RefresDelay;
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
};

#endif // GPSDEVICE_H
