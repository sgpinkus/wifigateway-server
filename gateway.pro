#-------------------------------------------------
#
# Project created by QtCreator 2013-01-02T10:07:51
#
#-------------------------------------------------

!include(common.pri) {
  error("Failed loading common.pri")
}

QT       += core dbus
QT       -= gui
TEMPLATE = app
TARGET = wifigatewayserver
CONFIG   += console
CONFIG   -= app_bundle
INCLUDEPATH += .
LIBS += -lruncommand -lfilewatcher -Lfilewatcher/ -Lruncommand/

# Input
HEADERS += controller.h \
           controllerdbusadaptor.h \
           files.h \
           logger.h \
           quotabeacon.h \
           sessionexport.h

SOURCES += controller.cpp \
           controllerdbusadaptor.cpp \
           logger.cpp \
           main.cpp \
           quotabeacon.cpp

OTHER_FILES += \
    filewatcher/ \
    runcommand/ \
    script/gw_remove_host.sh \
    script/gw_init.sh \
    script/gw_functions.sh \
    script/gw_add_host.sh \
    wifigateway.pro
