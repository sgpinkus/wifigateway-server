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

# Input
HEADERS += controller.h \
           controllerdbusadaptor.h \
           files.h \
           gateway.h \
           logger.h \
           quotabeacon.h \
           session.h \
           sessionexport.h

SOURCES += controller.cpp \
           controllerdbusadaptor.cpp \
           logger.cpp \
           main.cpp \
           quotabeacon.cpp \
           session.cpp \
           sessionexport.cpp

OTHER_FILES += \
    filewatcher/ \
    runcommand/ \
    script/gw_remove_host.sh \
    script/gw_init.sh \
    script/gw_functions.sh \
    script/gw_add_host.sh \
    wifigateway.pro
