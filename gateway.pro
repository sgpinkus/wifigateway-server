#-------------------------------------------------
#
# Project created by QtCreator 2013-01-02T10:07:51
#
#-------------------------------------------------

QT       += core dbus
QT       -= gui

TARGET = gateway
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    controller.cpp \
    controllerdbusadaptor.cpp \
    runcommand/runcommand.cpp \
    logger.cpp \
    filewatcher/filewatcher.cpp \
    quotabeacon.cpp \
    runcommand/test/runcommandtest.cpp

HEADERS += \
    controller.h \
    controllerdbusadaptor.h \
    runcommand/runcommand.h \
    logger.h \
    files.h \
    sessionexport.h \
    filewatcher/filewatcher.h \
    quotabeacon.h

OTHER_FILES += \
    script/gw_remove_host.sh \
    script/gw_init.sh \
    script/gw_functions.sh \
    script/gw_add_host.sh \
    wifigateway.pro
