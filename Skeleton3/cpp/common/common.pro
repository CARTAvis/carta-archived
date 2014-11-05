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
    Algorithms/RawView2QImageConverter.h \
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
    State/ObjectManager.h \
    State/StateInterface.h \
    State/StateReader.h \
    State/StateWriter.h \
    ImageView.h \
    Data/Animator.h \
    Data/AnimatorType.h \
    Data/Controller.h \
    Data/DataLoader.h \
    Data/Selection.h \
    Data/Layout.h \
    Data/Region.h \
    Data/DataSource.h \
    Data/Region.h \
    Data/Util.h \
    Data/ViewManager.h \
    Data/ViewPlugins.h

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
    State/ObjectManager.cpp\
    State/StateInterface.cpp \
    State/StateReader.cpp \
    State/StateWriter.cpp \
    ImageView.cpp \
    Data/Animator.cpp \
    Data/AnimatorType.cpp \
    Data/Controller.cpp \
    Data/DataLoader.cpp \
    Data/Selection.cpp \
    Data/DataSource.cpp \
    Data/Layout.cpp \
    Data/Region.cpp \
    Data/Util.cpp \
    Data/ViewManager.cpp \
    Data/ViewPlugins.cpp \
    ScriptedCommandListener.cpp \
    Slice.cpp \
    IImage.cpp \
    CoordinateFormatter.cpp \
    AxisInfo.cpp \
    CoordinateGridPlotter.cpp \
    PlotLabelGenerator.cpp \
    PixelType.cpp \


#message( "common            PWD=$$PWD")
#message( "common         IN_PWD=$$IN_PWD")
#message( "common _PRO_FILE_PWD_=$$_PRO_FILE_PWD_")
#message( "common        OUT_PWD=$$OUT_PWD")

#CONFIG += precompile_header
#PRECOMPILED_HEADER = stable.h
#QMAKE_CXXFLAGS += -H

INCLUDEPATH += ../../../ThirdParty/rapidjson/include

DEPENDPATH += $$PROJECT_ROOT/CartaLib
