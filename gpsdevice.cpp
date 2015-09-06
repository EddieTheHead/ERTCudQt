#include "gpsdevice.h"

#include <GeometryLineString.h>
#include <GeometryPointCircle.h>
#include <GeometryPointImage.h>
#include <QMapControl.h>
#include <QDesktopWidget>
#include <iostream>
#include <QObject>
#include <QMessageBox>
#include <MapAdapterWMS.h>
#include <QMapControl.h>
#include <memory>
#include <QLayout>
#include <QApplication>
#include <QLabel>
#include <GeometryWidget.h>
#include <chrono>
#include <degminsec.h>

using namespace qmapcontrol;


GpsDevice::GpsDevice(QWidget *parent) :
    QMapControl(parent)
{

    this->enablePersistentCache(std::chrono::minutes(0),QDir::currentPath() + QDir::separator() + "QMapControl.cache" );

    this->setZoomMaximum(19);
    this->setZoom(19);
    createLayers();

//    odświeżanie
    RefresDelay = 500;
    DrawTimer.setSingleShot(false);
    DrawTimer.setInterval(RefresDelay);
    DrawTimer.start();
    QObject::connect(&DrawTimer,SIGNAL(timeout()),this,SLOT(drawPath()));

    //spójrz na Polibudę
    PointWorldCoord ERC(20.461741,50.782566);
    this->setMapFocusPoint(ERC);
    this->enableScalebar(true);
    //testowe
    pointerImage = QImage("://Pointer.png");
    followingRower = false;
    connect(this,SIGNAL(mouseEventPressCoordinate(QMouseEvent*,PointWorldCoord)),SLOT(onMousePressEvent(QMouseEvent*,PointWorldCoord)));
    //connect(this,SIGNAL(geometryClicked(const Geometry*)),this,SLOT(onGeometryClicked(const Geometry*)));
    PointSelectMode = false;

    setBackgroundColour(Qt::white);

    readCheckpoints("://way.txt");

//    addCheckPoint(PointWorldCoord(DegMinSec(20,27,43.29576).toDecimalDegress(),DegMinSec(50,46,57.15351).toDecimalDegress()),"1A");
    addCheckPoint(PointWorldCoord(DegMinSec(20,27,41.75322).toDecimalDegress(),DegMinSec(50,46,57.46213).toDecimalDegress()),"1B");
    addCheckPoint(PointWorldCoord(DegMinSec(20,27,40.88582).toDecimalDegress(),DegMinSec(50,46,56.34093).toDecimalDegress()),"2");
    addCheckPoint(PointWorldCoord(DegMinSec(20,27,42.88572).toDecimalDegress(),DegMinSec(50,46,57.52350).toDecimalDegress()),"3");;
    drawCheckpoints();
}


GpsDevice::~GpsDevice()
{
}

void GpsDevice::newGPSPosition(PointWorldCoord pos)
{
    qDebug() << "lat = " << pos.latitude() << "long = " << pos.longitude();
    auto freshPoint = std::make_shared<GeometryPoint>(pos);

    if (!OffsetArea->touches(freshPoint.get(),this->getCurrentZoom()))
    {
        qDebug() << "Koordynaty poza dozwolonym polem ";
        return;
    }
    if(points.empty())
    {
        points.emplace_back(freshPoint);
        this->setMapFocusPoint(pos);
    }
    else if(pos != points.back()->coord())
    {
        points.emplace_back(freshPoint);
    }
    if(!points.empty() && followingRower) this->setMapFocusPointAnimated(pos);
}

void GpsDevice::newCompassAngle(float angle)
{
    if(!points.empty()) drawDirection(points.back()->coord().latitude(),points.back()->coord().longitude(),angle);
}

void GpsDevice::chooseNewCheckPointsFile()
{
    QString fileName = QFileDialog::getOpenFileName();
    if(!readCheckpoints(fileName)) QMessageBox::warning(this,"Błąd otwierania pliku","Nie udało się wczytać pliku check pointów");
}

void GpsDevice::drawPath()
{
    pathLayer->clearGeometries();
    //rysowanie punktów
    std::vector<PointWorldCoord> rawPoints;
    for(const auto& point : points)
    {
        rawPoints.push_back(point->coord());
        pathLayer->addGeometry(point);
    }
    //rysowanie linii przechodzącej przez te punkty
    auto pathString = std::make_shared<GeometryLineString>(rawPoints,0,19);
//    if(points.size() < 2) return;
//    std::vector<PointWorldCoord> twoLastPoints;
//    twoLastPoints.push_back(points.at(points.size() - 2)->coord());
//    twoLastPoints.push_back(points.back()->coord());
//    auto pathString = std::make_shared<GeometryLineString>(twoLastPoints,0,19);
    QPen pen(Qt::red);
    pen.setWidth(2);
    pathString->setPen(pen);
    pathLayer->addGeometry(pathString);
}

bool GpsDevice::readCheckpoints(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "!file.open()";
        return 0;
    }

    QTextStream in(&file);

    //checkPointsList.clear();

    while(!in.atEnd())
    {
        QString line = in.readLine();

        QRegExp rx("(\\d{1,3})[°dh,\\s]{1,}(\\d{1,3})['m,\\s]{1,}(\\d{1,3}.\\d{1,})[s,'' ]{1,2}[ x,]{1,}(\\d{1,3})[°dh,\\s]{1,}(\\d{1,3})['m,\\s]{1,}(\\d{1,3}.\\d{1,})[s,']{1,2}");
        rx.exactMatch(line);
        QStringList captured = rx.capturedTexts();
        qDebug() << "line" + line;
        if(captured.size() == 7 )
        {
            qDebug() << "if(captured.size() == 7) true";
            for(const auto &s: captured) qDebug() << s;
            DegMinSec longitude(captured.at(1).toInt(),captured.at(2).toInt(),captured.at(3).toFloat());
            DegMinSec latitude(captured.at(4).toInt(),captured.at(5).toInt(),captured.at(6).toFloat());
            qDebug() << "z pliku";
            qDebug() << longitude.toDecimalDegress();
            qDebug() << latitude.toDecimalDegress();
            PointWorldCoord point(longitude.toDecimalDegress(),latitude.toDecimalDegress());

            addCheckPoint(point,"");
        }
    }
    file.close();
    drawCheckpoints();
    return true;
}

void GpsDevice::createLayers()
{
    std::set<projection::EPSG> projections;
    projections.insert(projection::EPSG::SphericalMercator);
    mapLayer = std::make_shared<LayerMapAdapter>("Warstwa z mapą OSM",
                                                 std::make_shared<MapAdapterTile>(QUrl(""),
                                                 projections, 0, 19, 0, false));
    this->addLayer(mapLayer); //testowa warstwa z własnym zoomem
    this->addLayer(std::make_shared<LayerMapAdapter>("Warstwa z mapą Google", std::make_shared<MapAdapterGoogle>(MapAdapterGoogle::GoogleLayerType::SATELLITE))); //testowa warstwa googlwa
    //obrazek z mapą
    mapPixmapLayer = std::make_shared<LayerGeometry>("Map pixmap",0,19);
    this->addLayer(mapPixmapLayer);

    checkPointslayer = std::make_shared<LayerGeometry>("Warstwa Checkpointów",0,19);
    this->addLayer(checkPointslayer);
    pointerLayer=std::make_shared<LayerGeometry>("Pointer layer",0,19);
    this->addLayer(pointerLayer);

    //dodaję warstwę z przebiegiem trasy
    pathLayer=std::make_shared<LayerGeometry>("Path layer",0,19);
    this->addLayer(pathLayer);

    areaLayer = std::make_shared<LayerGeometry>("Warstwa z granicami",0,19);
    this->addLayer(areaLayer);

    infoLayer = std::make_shared<LayerGeometry>("Warstwa info",0,19);
    this->addLayer(infoLayer);
}

void GpsDevice::calculateOffsetArea()
{
    std::vector<PointWorldCoord> polygonPoints = CompetitionArea->points();
    std::vector<PointWorldCoord> offsetPoints;
    const qreal offset = 0.0001;
    offsetPoints.push_back(PointWorldCoord(polygonPoints[0].longitude() - offset,polygonPoints[0].latitude() + offset));
    offsetPoints.push_back(PointWorldCoord(polygonPoints[1].longitude() + offset,polygonPoints[1].latitude() + offset));
    offsetPoints.push_back(PointWorldCoord(polygonPoints[2].longitude() + offset,polygonPoints[2].latitude() - offset));
    offsetPoints.push_back(PointWorldCoord(polygonPoints[3].longitude() - offset,polygonPoints[3].latitude() - offset));

//    float eq1[2];
//    calculateDiagonalEq(polygonPoints[0],polygonPoints[2],eq1);
//    float eq2[2];
//    calculateDiagonalEq(polygonPoints[1],polygonPoints[3],eq2);
//    offsetPoints.push_back(calculaSingleOffsetPoint(polygonPoints[0],eq1,-offset));
//    offsetPoints.push_back(calculaSingleOffsetPoint(polygonPoints[1],eq2,offset));
//    offsetPoints.push_back(calculaSingleOffsetPoint(polygonPoints[2],eq1,offset));
//    offsetPoints.push_back(calculaSingleOffsetPoint(polygonPoints[3],eq2,-offset));

    OffsetArea = std::make_shared<GeometryPolygon>(offsetPoints,0,19);
}

void GpsDevice::addMapImage()
{
    mapPixmapLayer->clearGeometries();
    auto mapGeometry = std::make_shared<GeometryPointImage>(PointWorldCoord(0,0),mapPixmap,0,19);
    mapPixmapLayer->addGeometry(mapGeometry);
}

void GpsDevice::calculateDiagonalEq(PointWorldCoord left, PointWorldCoord right,float* data)
{
    float a = (left.longitude() - right.longitude())
              / (left.latitude() - right.latitude());
    float b = left.longitude() - left.latitude() * a;
    data[0] = a;
    data[1] = b;
}

PointWorldCoord GpsDevice::calculaSingleOffsetPoint(PointWorldCoord point, float *eq,float offset)
{
    float longitude = point.longitude() + offset;
    float latitude = longitude * eq[0] + eq[1];
    return PointWorldCoord(longitude,latitude);
}



void GpsDevice::chooseNewMapFile()
{
    QString filename = QFileDialog::getOpenFileName(this,"Choose new map image file",QApplication::applicationDirPath());
    mapPixmap = std::make_shared<QPixmap>(QPixmap::fromImage(QImage(filename)));
    if(mapPixmap->isNull()) return;
    else addMapImage();
}
bool GpsDevice::getPointSelectMode() const
{
    return PointSelectMode;
}

void GpsDevice::setPointSelectMode(bool value)
{
    PointSelectMode = value;
}

std::shared_ptr<GeometryPolygon> GpsDevice::getCompetitionArea() const
{
    return CompetitionArea;
}

void GpsDevice::setCompetitionArea(const std::shared_ptr<GeometryPolygon> &value)
{
    CompetitionArea = value;
    drawArea();
}

bool GpsDevice::getFollowingRower() const
{
    return followingRower;
}

void GpsDevice::setFollowingRower(bool value)
{
    followingRower = value;
}

void GpsDevice::drawDirection(float latitude,float longitude, float angle = 0)
{
    PointWorldCoord pointCoord(longitude,latitude);

    if(pointerImage.isNull())
    {
        qDebug()<<"Nie ma obrazka";
        return;
    }

    QTransform transMatrix;
    transMatrix.scale(0.1,0.1);
    transMatrix.rotate(angle);
    QImage rotatedImage = pointerImage.transformed(transMatrix);

    auto roverOrientation=std::make_shared<GeometryPointImage>(pointCoord, QPixmap::fromImage(rotatedImage),0,19);
    pointerLayer->clearGeometries();
    pointerLayer->addGeometry(roverOrientation);
}

void GpsDevice::drawCheckpoints()
{
    if(checkPointsList.empty()) requestRedraw();
    checkPointslayer->clearGeometries();
    for(const auto& point : checkPointsList)
    {
        // Also add the point to the custom layer.
        checkPointslayer->addGeometry(point);

        QLabel *infoLabel = new QLabel(this);
        QString info =  point->metadata(("name")).toString();
        qDebug() << "info wybranego punktu" << info;
        infoLabel->setText(info);
        infoLabel->setGeometry(0,0,100,100);
        auto infoGeometry = std::make_shared<GeometryWidget>(point->coord(),infoLabel,0,19);
        infoLayer->addGeometry(infoGeometry);
        infoGeometry->setVisible(true);
    }
    if(checkPointsList.size() >= 3)
    {
        std::vector<PointWorldCoord> line;
        line.push_back(PointWorldCoord(DegMinSec(20,27,40.88582).toDecimalDegress(),DegMinSec(50,46,56.34093).toDecimalDegress()));
        line.push_back(PointWorldCoord(DegMinSec(20,27,42.88572).toDecimalDegress(),DegMinSec(50,46,57.52350).toDecimalDegress()));


        auto lineString = std::make_shared<GeometryLineString>(line,0,19);
        QPen pen(Qt::blue);
        pen.setWidth(2);
        lineString->setPen(pen);
        checkPointslayer->addGeometry(lineString);
    }
}



void GpsDevice::addCheckPoint(PointWorldCoord point, QString name = QString(""))
{
    QPen pen(QColor(0,255,0));
    pen.setWidth(3);
    checkPointsList.emplace_back(std::make_shared<GeometryPointCircle>(point, QSizeF(15.0, 15.0),0,19));
    checkPointsList.back()->setPen(pen);
    checkPointsList.back()->setMetadata(std::string("name"),name);
    checkPointsList.back()->setMetadataDisplayed("name");
    drawCheckpoints();
}

void GpsDevice::onMousePressEvent(QMouseEvent *e, PointWorldCoord coords)
{
    if(e->buttons() & Qt::MouseButton::LeftButton)
    {
        for(const auto &p : checkPointsList)
        {
            auto freshPoint = std::make_shared<GeometryPoint>(coords,0,19);
            if(p->touches(freshPoint.get(),this->getCurrentZoom()))
            {
                QLabel *infoLabel = new QLabel(this);
                QString info =  freshPoint->metadata(("name")).toString() + "\n";
                qDebug() << "info wybranego punktu" << info;
                infoLabel->setText(info);
                infoLabel->setGeometry(0,0,200,100);
                auto infoGeometry = std::make_shared<GeometryWidget>(p->coord(),infoLabel,0,19);
                infoLayer->addGeometry(infoGeometry);
                infoGeometry->setVisible(true);
            }
        }
    }
    else if(e->buttons() & Qt::MouseButton::RightButton && PointSelectMode)
    {
//        for(uint i =0; i < checkPointsList.size(); ++i)
//        {
//            auto freshPoint = std::make_shared<GeometryPoint>(coords);
//            if(checkPointsList[i]->touches(freshPoint.get(),getCurrentZoom()))
//            {
//                checkPointsList.erase(checkPointsList.begin() + i);
//            }
//        }
    }
    drawCheckpoints();
}

//void GpsDevice::onGeometryClicked(const Geometry *geo)
//{
//    QMessageBox::information(this,"test",geo->metadata("name").toString(),QMessageBox::Ok);
//}

void GpsDevice::drawArea()
{
    qDebug() << "Nowy obszar:";
    for(const auto &p:CompetitionArea->points()) qDebug() << "lon:" << QString("%1").arg(p.longitude(), 0, 'g', 5) << "lat:" << QString("%1").arg(p.latitude(), 0, 'g', 5);
    QPen pen(Qt::green);
    pen.setWidth(2);
    CompetitionArea->setPen(pen);
    areaLayer->clearGeometries();
    areaLayer->addGeometry(CompetitionArea);
    calculateOffsetArea();
    drawOffsetArea();
}

void GpsDevice::drawOffsetArea()
{
    QPen pen(Qt::red);
    pen.setWidth(2);
    QBrush brush(Qt::transparent);
    OffsetArea->setBrush(brush);
    OffsetArea->setPen(pen);
    areaLayer->addGeometry(OffsetArea);
}

