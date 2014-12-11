#-------------------------------------------------
#
# Project created by QtCreator 2014-11-13T17:13:36
#
#-------------------------------------------------

QT       += core gui serialport opengl

##mapy
#include(QMC.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ERTCudQt
TEMPLATE = app

#kompilacja c++11
CONFIG += c++11 \



SOURCES += main.cpp\
        mainwindow.cpp \
    bateriabar.cpp \
    loggingdevice.cpp \
    portmonitor.cpp \
    portsettingswindow.cpp \
    mapwindow.cpp

HEADERS  += mainwindow.h \
    bateriabar.h \
    loggingdevice.h \
    portmonitor.h \
    portsettingswindow.h \
    mapwindow.h


FORMS    += mainwindow.ui \
    PortSettingsWindow.ui \
    mapwindow.ui

#OTHER_FILES += QMC.pri \
#            src/QMapControl.pri
