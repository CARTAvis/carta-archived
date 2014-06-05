PROJECT_ROOT = "../.."

! include(../../common.pri) {
  error( "Could not find the common.pri file!" )
}

INCLUDEPATH += "$$PROJECT_ROOT"

QT       += core gui

TARGET = clock1
TEMPLATE = lib
CONFIG += plugin

SOURCES += \
    Clock1Plugin.cpp

HEADERS += \
    Clock1Plugin.h

