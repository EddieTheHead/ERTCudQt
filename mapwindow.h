#ifndef MAPWINDOW_H
#define MAPWINDOW_H

#include <QDialog>
#include "QMapControl.h"
#include "LayerMapAdapter.h"
#include <MapAdapterOSM.h>

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

private:
    Ui::MapWindow *ui;
    QMapControl *map;
};

#endif // MAPWINDOW_H
