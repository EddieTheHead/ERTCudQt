#-------------------------------------------------
#
# Project created by QtCreator 2014-11-13T17:13:36
#
#-------------------------------------------------

QT       += core gui serialport opengl


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

win32:CONFIG(release, debug|release): LIBS += -L$(QMAPCONTROL_RELEASE_DLL) -lqmapcontrol1
else:win32:CONFIG(debug, debug|release): LIBS += -L$(QMAPCONTROL_DEBUG_DLL) -lqmapcontrold1

unix:CONFIG(release, debug|release): LIBS += -L$(QMAPCONTROL_RELEASE_DLL) -lqmapcontrol1
else:unix:CONFIG(debug, debug|release): LIBS += -L$(QMAPCONTROL_DEBUG_DLL) -lqmapcontrold1

INCLUDEPATH += $(QMAPCONTROL_SRC)
DEPENDPATH += $(QMAPCONTROL_SRC)
