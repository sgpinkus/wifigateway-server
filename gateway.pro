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
TARGET = wifigateway-server
CONFIG   += console
CONFIG   -= app_bundle
INCLUDEPATH += .
LIBS += -lruncommand -lfilewatcher -Lfilewatcher/ -Lruncommand/
PRE_TARGETDEPS += runcommand/libruncommand.a filewatcher/libfilewatcher.a

# Input
HEADERS += controller.h \
    controllerdbusadaptor.h \
    files.h \
    quotabeacon.h \
    sessionexport.h \
    gatewaycoreapplication.h

SOURCES += controller.cpp \
    controllerdbusadaptor.cpp \
    main.cpp \
    quotabeacon.cpp \
    gatewaycoreapplication.cpp

OTHER_FILES += \
    filewatcher/ \
    runcommand/ \
    script/gw_remove_host.sh \
    script/gw_init.sh \
    script/gw_functions.sh \
    script/gw_add_host.sh \
    wifigateway-server.pro
