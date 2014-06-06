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
    IPlugin.h \
    Nullable.h \
    IImage.h \
    Globals.h \
    Algorithms/Graphs/TopoSort.h \
    stable.h \
    CmdLine.h \
    MainConfig.h

SOURCES += \
    Viewer.cpp \
    LinearMap.cpp \
    MyQApp.cpp \
    CallbackList.cpp \
    PluginManager.cpp \
    Globals.cpp \
    Algorithms/Graphs/TopoSort.cpp \
    CmdLine.cpp \
    MainConfig.cpp


message( "common            PWD=$$PWD")
message( "common         IN_PWD=$$IN_PWD")
message( "common _PRO_FILE_PWD_=$$_PRO_FILE_PWD_")
message( "common        OUT_PWD=$$OUT_PWD")

#CONFIG += precompile_header
#PRECOMPILED_HEADER = stable.h
#QMAKE_CXXFLAGS += -H
