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
    //PointWorldCoord Poznan;
//    map->setMapFocusPoint(Poznan);

}

MapWindow::~MapWindow()
{
    delete map;
    delete ui;
}
