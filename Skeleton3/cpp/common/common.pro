! include(../common.pri) {
  error( "Could not find the common.pri file!" )
}

TEMPLATE = lib

###CONFIG += staticlib
QT += widgets network
QT += xml

HEADERS += \
    IConnector.h \
    IPlatform.h \
    Viewer.h \
    IView.h \
    MyQApp.h \
    CallbackList.h \
    PluginManager.h \
    Globals.h \
    Algorithms/RawView2QImageConverter.h \
    stable.h \
    CmdLine.h \
    MainConfig.h \
    ScriptedCommandListener.h \
    State/ObjectManager.h \
    State/StateInterface.h \
    State/StateReader.h \
    State/StateWriter.h \
    ImageView.h \
    Data/Animator.h \
    Data/AnimatorType.h \
    Data/Clips.h \
    Data/Colormap.h \
    Data/Colormaps.h \
    Data/Controller.h \
    Data/DataLoader.h \
    Data/ErrorReport.h \
    Data/ErrorManager.h \
    Data/Histogram.h \
    Data/IColoredView.h \
    Data/ILinkable.h \
    Data/Selection.h \
    Data/Layout.h \
    Data/LinkableImpl.h \
    Data/Preferences.h \
    Data/Region.h \
    Data/DataSource.h \
    Data/Region.h \
    Data/RegionRectangle.h \
    Data/Statistics.h \
    Data/TransformsData.h \
    Data/TransformsImage.h \
    Data/Util.h \
    GrayColormap.h \
    ColormapsOptimization.h \
    Data/ViewManager.h \
    Data/ViewPlugins.h \
    GrayColormap.h \
    ImageRenderService.h \
    Algorithms/quantileAlgorithms.h \
    Hacks/HackViewer.h \
    Hacks/ImageViewController.h \
    Hacks/MainModel.h \
    Hacks/ILayeredView.h \
    Hacks/IVectorGraphicsView.h \
    Hacks/WcsGridOptionsController.h

SOURCES += \
    Viewer.cpp \
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
    Data/Clips.cpp \
    Data/Colormap.cpp \
    Data/Colormaps.cpp \
    Data/Controller.cpp \
    Data/DataLoader.cpp \
    Data/ErrorReport.cpp \
    Data/ErrorManager.cpp \
    Data/Histogram.cpp \
    Data/LinkableImpl.cpp \
    Data/Selection.cpp \
    Data/DataSource.cpp \
    Data/Layout.cpp \
    Data/Preferences.cpp \
    Data/Region.cpp \
    Data/RegionRectangle.cpp \
    Data/Statistics.cpp \
    Data/TransformsData.cpp \
    Data/TransformsImage.cpp \
    Data/Util.cpp \
    Data/ViewManager.cpp \
    Data/ViewPlugins.cpp \
    ScriptedCommandListener.cpp \
    GrayColormap.cpp \
    Algorithms/RawView2QImageConverter.cpp \
    HistogramGenerator.cpp \
    ScriptFacade.cpp \
    ScriptTester.cpp \
    ImageRenderService.cpp \
    Algorithms/quantileAlgorithms.cpp \
    Hacks/HackViewer.cpp \
    Hacks/ImageViewController.cpp \
    Hacks/MainModel.cpp \
    Hacks/ILayeredView.cpp \
    Hacks/IVectorGraphicsView.cpp \
    Hacks/WcsGridOptionsController.cpp


#message( "common            PWD=$$PWD")
#message( "common         IN_PWD=$$IN_PWD")
#message( "common _PRO_FILE_PWD_=$$_PRO_FILE_PWD_")
#message( "common        OUT_PWD=$$OUT_PWD")

#CONFIG += precompile_header
#PRECOMPILED_HEADER = stable.h
#QMAKE_CXXFLAGS += -H

INCLUDEPATH += ../../../ThirdParty/rapidjson/include 

#INCLUDEPATH += ../../../ThirdParty/qwt/include
#LIBS += -L../../../ThirdParty/qwt/lib -lqwt
#INCLUDEPATH += /home/pfederl/Software/qwt-6.1.2-qt-5.3.2/include
#LIBS += -L/home/pfederl/Software/qwt-6.1.2-qt-5.3.2/lib -lqwt

#QWT_ROOT = /home/pfederl/Software/qwt-6.1.2-qt-5.3.2
QWT_ROOT = $$absolute_path("../../../ThirdParty/qwt")
INCLUDEPATH += $$QWT_ROOT/include
LIBS += -L$$QWT_ROOT/lib -lqwt
QMAKE_LFLAGS += '-Wl,-rpath,\'$$QWT_ROOT/lib\''


DEPENDPATH += $$PROJECT_ROOT/CartaLib
