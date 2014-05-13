! include(../common.pri) {
  error( "Could not find the common.pri file!" )
}

TEMPLATE = lib

CONFIG += staticlib
QT += widgets

HEADERS += \
    IConnector.h \
    IPlatform.h \
    Viewer.h \
    misc.h \
    IView.h \
    LinearMap.h \
    MyQApp.h \
    CallbackList.h \
    PluginManager.h \
    IPlugin.h

SOURCES += \
    Viewer.cpp \
    LinearMap.cpp \
    MyQApp.cpp \
    CallbackList.cpp \
    PluginManager.cpp

