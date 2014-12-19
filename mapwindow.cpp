#include "mapwindow.h"
#include "ui_mapwindow.h"

using namespace qmapcontrol;

MapWindow::MapWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapWindow)
{
    ui->setupUi(this);
    //nowy obiekt widgetu map
    map = new QMapControl(QSizeF(400.0, 590.0));
    //ustawiam obiekt map window na środku okienka
    ui->verticalLayout->addWidget(map);
    //dodaję warstwę z obrazkami map
    map->addLayer(std::make_shared<LayerMapAdapter>("Warstwa z mapą", std::make_shared<MapAdapterOSM>()));
    //dodaję warstwę z przebiegiem trasy
    map->addLayer(pathLayer);
//    map->setMapFocusPoint(Poznan);

}

MapWindow::~MapWindow()
{
    delete map;
    delete ui;
}

void MapWindow::newGPSPosition(float longitude, float latitude)
{
    points.emplace_back(std::make_shared<GeometryPoint>(longitude,latitude));
    drawPath();
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
    //rysowanie linii
    std::shared_ptr<GeometryLineString> pathString(std::make_shared<GeometryLineString>(rawPoints));
    QPen pen(Qt::red);
    pathString->setPen(pen);
    pen.setWidth(5);
    pathLayer->addGeometry(pathString);
}
