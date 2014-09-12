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
    MainConfig.h \
    ScriptedCommandListener.h \
    PixelType.h \
    Slice.h \
    CoordinateFormatter.h \
    AxisInfo.h \
    CoordinateGridPlotter.h \
    PlotLabelGenerator.h \
    State/State.h \
    State/StateKey.h \
    State/StateLibrary.h \
    State/StateXmlRestorer.h \
    Data/DataAnimator.h \
    Data/DataController.h \
    Data/DataLoader.h \
    Data/DataSelection.h \
    Data/Region.h \
    Data/DataSource.h \
    Data/Region.h

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
    Data/DataAnimator.cpp \
    Data/DataController.cpp \
    Data/DataLoader.cpp \
    Data/DataSelection.cpp \
    Data/DataSource.cpp \
    Data/Region.cpp \
    ScriptedCommandListener.cpp \
    Slice.cpp \
    IImage.cpp \
    CoordinateFormatter.cpp \
    AxisInfo.cpp \
    CoordinateGridPlotter.cpp \
    PlotLabelGenerator.cpp


message( "common            PWD=$$PWD")
message( "common         IN_PWD=$$IN_PWD")
message( "common _PRO_FILE_PWD_=$$_PRO_FILE_PWD_")
message( "common        OUT_PWD=$$OUT_PWD")

#CONFIG += precompile_header
#PRECOMPILED_HEADER = stable.h
#QMAKE_CXXFLAGS += -H
