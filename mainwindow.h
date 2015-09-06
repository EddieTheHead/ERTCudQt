#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "portmonitor.h"
#include "bateriabar.h"
#include "workindicator.h"
#include "areasettingsdialog.h"
#include "gpsdevice.h"
#include "degminsec.h"
#include "dialoglog.h"

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
    void pointSelected(PointWorldCoord);
private:
    Ui::MainWindow *ui;
    PortMonitor *monitor;
    GpsDevice *mapWidget;
    AreaSettingsDialog *AreaDialog;
    DialogLog *log;

private slots:
    void resizeEvent(QResizeEvent *e);
    void keyPressEvent(QKeyEvent *event);
    void onNewBateryVoltage(float value);
    void onNewReceiverBateryVoltage(float value);
    void onNewRssiValue(float value);
    void onNewScalesEx(int value);
    void onNewScalesDrill(int value);
    void onNewEnginesState(bool working);
    void onNewSattelitesNumber(int number);
    void onNewGPSSignalQuality(int value);
    void displayMouseCoords(QMouseEvent *e, PointWorldCoord press, PointWorldCoord mov);
    void onMapPointSelected(QMouseEvent *e,PointWorldCoord point);
    void onNewRssiRx(int value);
    void onNewRssiTx(int value);
    void onNewLimitsState(uchar value);
    void onNewAdcValue(int adc,int value);
    void on_pushButton_addCheckPoint_clicked();
    void onNewCompassValue(float value);
};

#endif // MAINWINDOW_H
