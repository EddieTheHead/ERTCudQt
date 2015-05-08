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
    pathLayer=std::make_shared<LayerGeometry>("Base layer");
    map->addLayer(pathLayer);

    setWindowTitle("Mapa");
    //spójrz na Polibudę
    PointWorldCoord PUT(16.950932,52.402205);
     PointWorldCoord ERC(20.452138, 50.790648);
    map->setMapFocusPoint(ERC);
    map->setZoom(14);

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
            drawDirection(16.950932,52.402205);
}

MapWindow::~MapWindow()
{
    delete map;
    delete ui;
}

void MapWindow::newGPSPosition(float longitude, float latitude)
{
    if(points.back()->coord().latitude() != latitude || points.back()->coord().longitude() != longitude)
    {
        points.emplace_back(std::make_shared<GeometryPoint>(longitude,latitude));
        drawPath();
        map->setMapFocusPoint(PointWorldCoord(points.back()->coord()));
    }
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
    pen.setWidth(5);

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

void MapWindow::drawDirection(float latitude, float longitude)
{
    PointWorldCoord pointCoord(latitude,longitude);
    QImage pointerImage("pointer.png");
    if(pointerImage.isNull())
        qDebug()<<"Nie ma obrazka";


    auto roverOrientation=std::make_shared<GeometryPointImage>(pointCoord, QPixmap::fromImage(pointerImage));
    pathLayer->addGeometry(roverOrientation);



}
