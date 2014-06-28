# Tests. Generates a single executable.

!include(common.pri) {
    error("Failed loading common.pri")
}
QT += testlib
QT -= gui
CONFIG += qtestlib debug
TEMPLATE = app
TARGET = 
DEPENDPATH += . .. ..
INCLUDEPATH += . ..
#LIBS += -lruncommand -L..

# Input
SOURCES += runcommandtest.cpp runcommand.cpp
HEADERS += runcommandtest.h runcommand.h
