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

private:
    Ui::MainWindow *ui;
    PortMonitor *monitor;
    //MapWindow *mapWindow;
private slots:
    void showData(QByteArray data);
};

#endif // MAINWINDOW_H
