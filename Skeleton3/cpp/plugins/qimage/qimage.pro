PROJECT_ROOT = "../.."

! include(../../common.pri) {
  error( "Could not find the common.pri file!" )
}

INCLUDEPATH += "$$PROJECT_ROOT"

QT       += core gui

TARGET = qimage

TEMPLATE = lib
CONFIG += plugin

SOURCES += \
    QImagePlugin.cpp

HEADERS += \
    QImagePlugin.h

