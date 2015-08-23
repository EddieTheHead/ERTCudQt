#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "portmonitor.h"
#include "bateriabar.h"
#include "mapwindow.h"
#include "workindicator.h"

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
    void setMapButtonText();
    void onNewBateryVoltage(float value);
    void onNewReceiverBateryVoltage(float value);
    void onNewRssiValue(float value);
    void onNewScalesEx(int value);
    void onNewScalesDrill(int value);
    void onNewEnginesState(bool working);

};

#endif // MAINWINDOW_H
