PROJECT_ROOT = "../.."

! include(../../common.pri) {
  error( "Could not find the common.pri file!" )
}

INCLUDEPATH += "$$PROJECT_ROOT"

QT       += core gui

TARGET = tester1
TEMPLATE = lib
CONFIG += plugin

SOURCES += GenericPlugin.cpp

HEADERS += GenericPlugin.h
OTHER_FILES += tester1.json

