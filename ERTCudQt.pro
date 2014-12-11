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
 #   $(QMAPCONTROL_SRC)/QMapControl.h


FORMS    += mainwindow.ui \
    PortSettingsWindow.ui \
    mapwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$(QMAPCONTROL_RELEASE_DLL) -lqmapcontrol1
else:win32:CONFIG(debug, debug|release): LIBS += -L$(QMAPCONTROL_DEBUG_DLL) -lqmapcontrold1
#else:unix: LIBS += -L$$PWD/../../QMapControl/build-QMapControl-Desktop_Qt_5_2_1_MinGW_32bit-Debug/src/QMapControl/lib/ -lqmapcontrold1

#INCLUDEPATH += $$PWD/$(QMAPCONTROL_SRC)
INCLUDEPATH += $(QMAPCONTROL_SRC)
#DEPENDPATH += $$PWD/$(QMAPCONTROL_SRC)
DEPENDPATH += $(QMAPCONTROL_SRC)
