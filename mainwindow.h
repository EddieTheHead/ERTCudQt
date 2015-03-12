#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "portmonitor.h"
#include "bateriabar.h"
#include "mapwindow.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void newGPSData(float, float);
private:
    Ui::MainWindow *ui;
    PortMonitor *monitor;
    MapWindow *mapWindow;
private slots:
    void showData(QByteArray data);
    void on_pushButtonMap_clicked();
    void onFakeTrackTimer();

private:
    //fałszywy sygnał z danymi GPS do testów
    QTimer fakeTrackTimer;
    struct fakePoint{
        fakePoint(float n,float t){lon =n; lat =t;}
        float lon;
        float lat;
    };
    QList<fakePoint> fakeGpsPoints;
};

#endif // MAINWINDOW_H
