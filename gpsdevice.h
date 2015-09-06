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
#include <fillinfodialog.h>

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

    bool getPointSelectMode() const;


public slots:
    void setCompetitionArea(const std::shared_ptr<GeometryPolygon> &value);
    void setFollowingRower(bool value);
    void newGPSPosition(const PointWorldCoord pos);
    void newCompassAngle(float angle);
    void chooseNewCheckPointsFile();
    void drawArea();
    void drawOffsetArea();
    void setPointSelectMode(bool value);
    void chooseNewMapFile();

private slots:
    void drawPath();
    void drawDirection(float lat, float longitude, float angle);
    void drawCheckpoints();
    void addCheckPoint(PointWorldCoord point, QString name);
    void onMousePressEvent(QMouseEvent* e, PointWorldCoord coords);
    //void onGeometryClicked(const Geometry *geo);
private:
    //void resizeEvent(QResizeEvent *event);
    bool readCheckpoints(QString fileName); // Funkcja czytująca punkty z pliku
    void createLayers();
    void calculateOffsetArea();
    void addMapImage();
    void calculateDiagonalEq(PointWorldCoord left, PointWorldCoord right , float *data);
    PointWorldCoord calculaSingleOffsetPoint(PointWorldCoord point, float *eq, float offset);

    int RefresDelay = 500;
    QTimer DrawTimer;
    std::shared_ptr<LayerGeometry> pathLayer;
    std::shared_ptr<LayerGeometry> pointerLayer;
    std::shared_ptr<LayerGeometry> areaLayer;
    std::shared_ptr<LayerMapAdapter> mapLayer;
    std::shared_ptr<LayerGeometry> mapPixmapLayer;
    std::shared_ptr<LayerGeometry> infoLayer;
    std::vector<std::shared_ptr<GeometryPoint>> points;
    std::vector<std::shared_ptr<GeometryPoint>> checkPointsList;
    QImage pointerImage;
    bool followingRower;
    std::shared_ptr<PointWorldCoord> ERC;
    std::shared_ptr<GeometryPolygon> CompetitionArea;
    std::shared_ptr<GeometryPolygon> OffsetArea;
    std::shared_ptr<LayerGeometry> checkPointslayer;
    bool PointSelectMode;
    std::shared_ptr<QPixmap> mapPixmap;

};

#endif // GPSDEVICE_H
