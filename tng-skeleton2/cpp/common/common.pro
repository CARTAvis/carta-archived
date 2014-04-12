! include(../common.pri) {
  error( "Could not find the common.pri file!" )
}

TEMPLATE = lib

CONFIG += staticlib

HEADERS += \
    IConnector.h \
    IPlatform.h \
    Viewer.h \
    misc.h \
    IView.h \
    LinearMap.h

SOURCES += \
    Viewer.cpp \
    LinearMap.cpp

