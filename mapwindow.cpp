#include "mapwindow.h"
#include "ui_mapwindow.h"

#include <GeometryLineString.h>
#include <GeometryPointCircle.h>
#include <LayerMapAdapter.h>
#include <GeometryPointImage.h>

using namespace qmapcontrol;

MapWindow::MapWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapWindow)
{
    ui->setupUi(this);
    //nowy obiekt widgetu map
    map = new QMapControl(QSizeF(950.0, 540.0));
    //ustawiam obiekt map window na środku okienka
    ui->verticalLayout->addWidget(map);
    map->enablePersistentCache();
    //dodaję warstwę z obrazkami map
    map->addLayer(std::make_shared<LayerMapAdapter>("Warstwa z mapą Open Street Map", std::make_shared<MapAdapterOSM>()));

    //map->addLayer(std::make_shared<LayerMapAdapter>("Warstwa z mapą Google", std::make_shared<MapAdapterGoogle>())); //testowa warstwa googlwa
    //dodaję warstwę z przebiegiem trasy
    pathLayer=std::make_shared<LayerGeometry>("Path layer");
    map->addLayer(pathLayer);

    pointerLayer=std::make_shared<LayerGeometry>("Pointer layer");
    map->addLayer(pointerLayer);

    setWindowTitle("Mapa");

    RefresDelay = 500;
    DrawTimer.setSingleShot(false);
    DrawTimer.setInterval(RefresDelay);
    DrawTimer.start();
    connect(&DrawTimer,SIGNAL(timeout()),this,SLOT(drawMap()));

    //spójrz na Polibudę
    PointWorldCoord PUT(16.950932,52.402205);
    PointWorldCoord ERC(20.452138, 50.790648);
    map->setMapFocusPoint(PUT);
    map->setZoom(14);

    //testowe
    pointerImage = QImage("pointer.png");

    map->setZoom(17);
}

MapWindow::~MapWindow()
{
    delete map;
    delete ui;
}

void MapWindow::newGPSPosition(QPointF pos)
{
    float latitude = pos.y();
    float longitude = pos.x();
    qDebug() << "lat = " << latitude << "long = " << longitude;
    if(points.empty())
    {
        points.emplace_back(std::make_shared<GeometryPoint>(longitude,latitude));
        map->setMapFocusPoint(PointWorldCoord(points.back()->coord()));
        //currentPosition->setCoord(PointWorldCoord(longitude,latitude));
        //map->followGeometry(currentPosition);
    }
    else if(points.back()->coord().latitude() != latitude || points.back()->coord().longitude() != longitude)
    {
        points.emplace_back(std::make_shared<GeometryPoint>(longitude,latitude));
        //currentPosition->setCoord(PointWorldCoord(longitude,latitude));
    }
}

void MapWindow::newCompassAngle(float angle)
{
    drawDirection(points.back()->coord().latitude(),points.back()->coord().longitude(),angle);
}

void MapWindow::chooseNewCheckPointsFile()
{
    QString fileName = QFileDialog::getOpenFileName();
    if(!readCheckpoints(fileName)) QMessageBox::warning(this,"Błąd otwierania pliku","Nie udało się wczytać pliku check pointów");
}

void MapWindow::closeEvent(QCloseEvent *event)
{
    emit onHide();
}

void MapWindow::drawPath()
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
    if(!points.empty() && followingRower) map->setMapFocusPoint(PointWorldCoord(points.back()->coord()));
    pathLayer->addGeometry(pathString);
}

bool MapWindow::readCheckpoints(QString fileName)
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
    return 1;
}
bool MapWindow::getFollowingRower() const
{
    return followingRower;
}

void MapWindow::setFollowingRower(bool value)
{
    followingRower = value;
}


void MapWindow::drawDirection(float latitude, float longitude, float angle = 0)
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

void MapWindow::drawCheckpoints()
{
    //dodaję warstwę z punktami
    std::shared_ptr<LayerGeometry> dots(std::make_shared<LayerGeometry>("Custom Layer"));
    map->addLayer(dots);

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

void MapWindow::drawMap()
{
    drawPath();
   //if(!points.empty()) map->followGeometry(points.back());

}
