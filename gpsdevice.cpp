#include "gpsdevice.h"

#include <GeometryLineString.h>
#include <GeometryPointCircle.h>
#include <GeometryPointImage.h>
#include <QMapControl.h>
#include <QDesktopWidget>
#include <iostream>
#include <QObject>
#include <QMessageBox>

using namespace qmapcontrol;


GpsDevice::GpsDevice(QWidget *parent) :
    QMapControl(parent->size(),parent)
{
    //nowy obiekt widgetu map
    QSize mapSize(size().width(),size().height() * 2/3);
//    map = new QMapControl(mapSize);
    this->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    this->setMinimumSize(mapSize);
//    ui->verticalLayout_this->addWidget(map);

    this->enablePersistentCache();
    createLayers();

    this->setZoomMaximum(17);
    this->setZoom(17);

    //odświeżanie
    RefresDelay = 500;
    DrawTimer.setSingleShot(false);
    DrawTimer.setInterval(RefresDelay);
    DrawTimer.start();
  //  QObject::connect(&DrawTimer,SIGNAL(timeout()),this,SLOT(drawMap()));

    //spójrz na Polibudę
    PointWorldCoord PUT(16.950932,52.402205);
    ERC = std::make_shared<PointWorldCoord>(PointWorldCoord(20.452138, 50.790648));
    this->setMapFocusPoint(PUT);
    this->enableScalebar(true);
    //testowe
    pointerImage = QImage("pointer.png");

//    connect(map,SIGNAL(mouseEventPressCoordinate(QMouseEvent*,PointWorldCoord)),this,SLOT(displayCoursorCoords(QMouseEvent*,PointWorldCoord)));

}


GpsDevice::~GpsDevice()
{
    DrawTimer.stop();
}

void GpsDevice::newGPSPosition(QPointF pos)
{
    float latitude = pos.y();
    float longitude = pos.x();

    qDebug() << "lat = " << latitude << "long = " << longitude;
    auto freshPoint = std::make_shared<GeometryPoint>(longitude,latitude);
    if (!CompetitionArea->touches(freshPoint.get(),this->getCurrentZoom()))
    {
        qDebug() << "Koordynaty poza dozwolonym polem ";
        return;
    }
    if(points.empty())
    {
        points.emplace_back(freshPoint);
        this->setMapFocusPoint(PointWorldCoord(points.back()->coord()));
        //currentPosition->setCoord(PointWorldCoord(longitude,latitude));
        //this->followGeometry(currentPosition);
    }
    else if(points.back()->coord().latitude() != latitude || points.back()->coord().longitude() != longitude)
    {
        points.emplace_back(freshPoint);
        //currentPosition->setCoord(PointWorldCoord(longitude,latitude));
    }
}

void GpsDevice::newCompassAngle(float angle)
{
    drawDirection(points.back()->coord().latitude(),points.back()->coord().longitude(),angle);
}

void GpsDevice::chooseNewCheckPointsFile()
{
    QString fileName = QFileDialog::getOpenFileName();
    if(!readCheckpoints(fileName)) QMessageBox::warning(this,"Błąd otwierania pliku","Nie udało się wczytać pliku check pointów");
}



void GpsDevice::displayCoursorCoords(QMouseEvent* event, PointWorldCoord coords)
{
    Q_UNUSED(event);
//    ui->label_coords->setText("Położenie kursora: " + QString::number(coords.longitude()) + "x" + QString::number(coords.latitude()));
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
    std::shared_ptr<GeometryLineString> pathString(std::make_shared<GeometryLineString>(rawPoints));
    QPen pen(Qt::red);
    pathString->setPen(pen);
    pen.setWidth(10);
    if(!points.empty() && followingRower) this->setMapFocusPointAnimated(PointWorldCoord(points.back()->coord()));
    pathLayer->addGeometry(pathString);
}

bool GpsDevice::readCheckpoints(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly))
    {
        return 0;
    }

    QTextStream in(&file);

    QPen dots_pen(QColor(0,255,0));
        dots_pen.setWidth(3);

    checkPointsList.clear();

    while(!in.atEnd())
    {
        QString line = in.readLine();
        QStringList fields = line.split("&", QString::SkipEmptyParts);

            checkPointsList.emplace_back(std::make_shared<GeometryPointCircle>(PointWorldCoord(fields[1].toFloat(),fields[2].toFloat() ), QSizeF(15.0, 15.0)));
            checkPointsList.back()->setPen(dots_pen);
            checkPointsList.back()->setMetadata(fields[0].toStdString(), "");//""Podzamcze 45, Checiny - ERC challenge"""");
    }
    file.close();
    return true;
}

void GpsDevice::createLayers()
{
    std::set<projection::EPSG> projections;
    projections.insert(projection::EPSG::SphericalMercator);
    //    this->addLayer(std::make_shared<LayerMapAdapter>("Warstwa z mapą OSM", std::make_shared<MapAdapterTile>(QUrl("http://tile.openstreetmap.org/%zoom/%x/%y.png"),
    //                                                                                                           projections, 0, 17, 0, false, parent))); //testowa warstwa z własnym zoomem
    pointerLayer=std::make_shared<LayerGeometry>("Pointer layer");
    this->addLayer(pointerLayer);
    this->addLayer(std::make_shared<LayerMapAdapter>("Warstwa z mapą Google", std::make_shared<MapAdapterGoogle>())); //testowa warstwa googlwa

   // this->addLayer(std::make_shared<LayerMapAdapter>("Warstwa z mapą OSM", std::make_shared<MapAdapterTile>(QUrl("http://c.tile.thunderforest.com/landscape/%zoom/%x/%y.png"), projections, 0, 19, 0, false, parent))); //testowa warstwa z własnym zoomem
    //dodaję warstwę z przebiegiem trasy
    pathLayer=std::make_shared<LayerGeometry>("Path layer");
    this->addLayer(pathLayer);

    areaLayer = std::make_shared<LayerGeometry>("Warstwa z granicami");
    this->addLayer(areaLayer);
}
std::shared_ptr<GeometryPolygon> GpsDevice::getCompetitionArea() const
{
    return CompetitionArea;
}

void GpsDevice::setCompetitionArea(const std::shared_ptr<GeometryPolygon> &value)
{
    CompetitionArea = value;
}

bool GpsDevice::getFollowingRower() const
{
    return followingRower;
}

void GpsDevice::setFollowingRower(bool value)
{
    followingRower = value;
}

void GpsDevice::drawDirection(float latitude, float longitude, float angle = 0)
{
    PointWorldCoord pointCoord(latitude,longitude);

    if(pointerImage.isNull())
    {
        qDebug()<<"Nie ma obrazka";
        return;
    }
    QTransform transMatrix;
    transMatrix.scale(0.1,0.1);
    transMatrix.rotate(angle);
    QImage rotatedImage = pointerImage.transformed(transMatrix);

    auto roverOrientation=std::make_shared<GeometryPointImage>(pointCoord, QPixmap::fromImage(rotatedImage));
    pointerLayer->addGeometry(roverOrientation);

}

void GpsDevice::drawCheckpoints()
{
    //dodaję warstwę z punktami
    std::shared_ptr<LayerGeometry> dots(std::make_shared<LayerGeometry>("Custom Layer"));
    this->addLayer(dots);

    if(!readCheckpoints("E:\\checkPoints.txt"))
    {
        qDebug()<<"Nie wczytano punktów";
    }

    QPen dots_pen(QColor(0,255,0));
    dots_pen.setWidth(3);

    for(const auto& point : checkPointsList)
    {
        // Also add the point to the custom layer.
        dots->addGeometry(point);
    }
}

void GpsDevice::drawMap()
{
    drawPath();
}

void GpsDevice::drawArea()
{
    qDebug() << "Nowy obszar:";
    for(const auto &p:CompetitionArea->points()) qDebug() << "lon:" << QString("%1").arg(p.longitude(), 0, 'g', 5) << "lat:" << QString("%1").arg(p.latitude(), 0, 'g', 5);
    QPen pen(Qt::red);
    pen.setWidth(3);
    CompetitionArea->setPen(pen);
    areaLayer->clearGeometries();
    areaLayer->addGeometry(CompetitionArea);
}

