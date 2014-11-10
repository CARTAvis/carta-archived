! include(../common.pri) {
  error( "Could not find the common.pri file!" )
}

TEMPLATE = lib

CONFIG += staticlib
QT += widgets network
QT += xml

HEADERS += \
    IConnector.h \
    IPlatform.h \
    Viewer.h \
    IView.h \
    LinearMap.h \
    MyQApp.h \
    CallbackList.h \
    PluginManager.h \
    Globals.h \
    Algorithms/RawView2QImageConverter.h \
    stable.h \
    CmdLine.h \
    MainConfig.h \
    ScriptedCommandListener.h \
    State/State.h \
    State/StateKey.h \
    State/StateLibrary.h \
    State/StateXmlRestorer.h \
    TestView.h \
    Data/DataAnimator.h \
    Data/DataController.h \
    Data/DataLoader.h \
    Data/DataSelection.h \
    Data/Region.h \
    Data/DataSource.h \
    Data/Region.h \
    GrayColormap.h \
    ColormapsOptimization.h

SOURCES += \
    Viewer.cpp \
    LinearMap.cpp \
    MyQApp.cpp \
    CallbackList.cpp \
    PluginManager.cpp \
    Globals.cpp \
    Algorithms/Graphs/TopoSort.cpp \
    CmdLine.cpp \
    MainConfig.cpp \
    State/State.cpp \
    State/StateLibrary.cpp \
    State/StateXmlRestorer.cpp \
    TestView.cpp \
    Data/DataAnimator.cpp \
    Data/DataController.cpp \
    Data/DataLoader.cpp \
    Data/DataSelection.cpp \
    Data/DataSource.cpp \
    Data/Region.cpp \
    ScriptedCommandListener.cpp \
    GrayColormap.cpp \
    ColormapsOptimization.cpp


#message( "common            PWD=$$PWD")
#message( "common         IN_PWD=$$IN_PWD")
#message( "common _PRO_FILE_PWD_=$$_PRO_FILE_PWD_")
#message( "common        OUT_PWD=$$OUT_PWD")

#CONFIG += precompile_header
#PRECOMPILED_HEADER = stable.h
#QMAKE_CXXFLAGS += -H

INCLUDEPATH += ../../../ThirdParty/rapidjson/include

DEPENDPATH += $$PROJECT_ROOT/CartaLib
