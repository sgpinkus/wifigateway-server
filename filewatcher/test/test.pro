# Tests. Generates a single executable.

!include(common.pri) {
    error("Failed loading common.pri")
}
QT += testlib
QT -= gui
CXXFLAGS += -std=c++11
CONFIG += qtestlib debug
TEMPLATE = app
TARGET = 
DEPENDPATH += . .. ..
INCLUDEPATH += . ..
#LIBS += -lfilewatcher -L..

# Input
SOURCES += filewatchertest.cpp filewatcher.cpp
HEADERS += filewatchertest.h filewatcher.h
