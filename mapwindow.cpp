#include "mapwindow.h"
#include "ui_mapwindow.h"

#include <GeometryLineString.h>
#include <GeometryPointCircle.h>
#include <GeometryPointImage.h>
#include <QMapControl.h>
#include <QDesktopWidget>
#include <iostream>

using namespace qmapcontrol;

MapWindow::MapWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapWindow)
{
    ui->setupUi(this);
    setWindowTitle("Mapa");
    setWindowFlags(Qt::Window);
    //zmiana rozmiaru i przesunięcie
    move(0,0);
    QRect rec = QApplication::desktop()->screenGeometry();
    resize(rec.width()/2 * 0.97,rec.height()*0.9);

    //nowy obiekt widgetu map
    QSize mapSize(size().width(),size().height() * 2/3);
    map = new QMapControl(mapSize);
    map->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    map->setMinimumSize(mapSize);
    ui->verticalLayout_Map->addWidget(map);

    map->enablePersistentCache();
    createLayers();

    map->setZoomMaximum(17);
    map->setZoom(17);

    //odświeżanie
    RefresDelay = 500;
    DrawTimer.setSingleShot(false);
    DrawTimer.setInterval(RefresDelay);
    DrawTimer.start();
    connect(&DrawTimer,SIGNAL(timeout()),this,SLOT(drawMap()));

    //spójrz na Polibudę
    PointWorldCoord PUT(16.950932,52.402205);
    ERC = std::make_shared<PointWorldCoord>(PointWorldCoord(20.452138, 50.790648));
    map->setMapFocusPoint(PUT);
    map->enableScalebar(true);
    //testowe
    pointerImage = QImage("pointer.png");

    connect(ui->checkBox_follow,SIGNAL(clicked(bool)),this,SLOT(setFollowingRower(bool)));
    ui->checkBox_follow->setChecked(true);
    connect(map,SIGNAL(mouseEventPressCoordinate(QMouseEvent*,PointWorldCoord)),this,SLOT(displayCoursorCoords(QMouseEvent*,PointWorldCoord)));


    //miejsce konkursu
    AreaDialog = new AreaSettingsDialog(this);
    connect(ui->pushButton_Area,SIGNAL(clicked()),AreaDialog,SLOT(show()));
    connect(AreaDialog,SIGNAL(onHide()),this,SLOT(drawArea()));
    connect(AreaDialog,SIGNAL(newAreaSelected()),this,SLOT(drawArea()));
    drawArea();
}

MapWindow::~MapWindow()
{
    DrawTimer.stop();
    delete map;
    delete ui;
}

void MapWindow::newGPSPosition(QPointF pos)
{
    float latitude = pos.y();
    float longitude = pos.x();

    qDebug() << "lat = " << latitude << "long = " << longitude;
    auto freshPoint = std::make_shared<GeometryPoint>(longitude,latitude);
    if (!CompetitionArea->touches(freshPoint.get(),map->getCurrentZoom()))
    {
        qDebug() << "Koordynaty poza dozwolonym polem ";
        return;
    }
    if(points.empty())
    {
        points.emplace_back(freshPoint);
        map->setMapFocusPoint(PointWorldCoord(points.back()->coord()));
        //currentPosition->setCoord(PointWorldCoord(longitude,latitude));
        //map->followGeometry(currentPosition);
    }
    else if(points.back()->coord().latitude() != latitude || points.back()->coord().longitude() != longitude)
    {
        points.emplace_back(freshPoint);
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

void MapWindow::onNewSattelitesNumber(int number)
{
    ui->label_numberOfSattelites->setText(QString("Liczba dostępnych satelitów: ") + QString::number(number));
}

void MapWindow::onNewGPSSignalQuality(int value)
{
    float percent = 1.0 / (float) value * 100.0;
    ui->progressBar_SignalQuality->setValue(percent);
    ui->label_SignalQuality->setText(QString("Jakość sygnału: ") + QString::number(value));
}

void MapWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    emit onHide();
    saveGeometry();
}

void MapWindow::displayCoursorCoords(QMouseEvent* event, PointWorldCoord coords)
{
    Q_UNUSED(event);
    ui->label_coords->setText("Położenie kursora: " + QString::number(coords.longitude()) + "x" + QString::number(coords.latitude()));
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
    if(!points.empty() && followingRower) map->setMapFocusPointAnimated(PointWorldCoord(points.back()->coord()));
    //if(!points.empty() && followingRower) map->setMapFocusPoint(PointWorldCoord(points.back()->coord()));
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

void MapWindow::createLayers()
{
    std::set<projection::EPSG> projections;
    projections.insert(projection::EPSG::SphericalMercator);
    //    map->addLayer(std::make_shared<LayerMapAdapter>("Warstwa z mapą OSM", std::make_shared<MapAdapterTile>(QUrl("http://tile.openstreetmap.org/%zoom/%x/%y.png"),
    //                                                                                                           projections, 0, 17, 0, false, parent))); //testowa warstwa z własnym zoomem

    pointerLayer=std::make_shared<LayerGeometry>("Pointer layer");
    map->addLayer(pointerLayer);

    map->addLayer(std::make_shared<LayerMapAdapter>("Warstwa z mapą Google", std::make_shared<MapAdapterGoogle>())); //testowa warstwa googlwa

   // map->addLayer(std::make_shared<LayerMapAdapter>("Warstwa z mapą OSM", std::make_shared<MapAdapterTile>(QUrl("http://c.tile.thunderforest.com/landscape/%zoom/%x/%y.png"), projections, 0, 19, 0, false, parent))); //testowa warstwa z własnym zoomem
    //dodaję warstwę z przebiegiem trasy
    pathLayer=std::make_shared<LayerGeometry>("Path layer");
    map->addLayer(pathLayer);

    areaLayer = std::make_shared<LayerGeometry>("Warstwa z granicami");
    map->addLayer(areaLayer);
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

void MapWindow::drawArea()
{
    CompetitionArea = AreaDialog->getPolygon();
    qDebug() << "Nowy obszar:";
    for(const auto &p:CompetitionArea->points()) qDebug() << "lon:" << QString("%1").arg(p.longitude(), 0, 'g', 5) << "lat:" << QString("%1").arg(p.latitude(), 0, 'g', 5);
    QPen pen(Qt::red);
    pen.setWidth(3);
    CompetitionArea->setPen(pen);
    areaLayer->clearGeometries();
    areaLayer->addGeometry(CompetitionArea);
}

