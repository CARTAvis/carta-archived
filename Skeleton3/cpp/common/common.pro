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
    State/ObjectManager.h \
    State/StateInterface.h \
    State/UtilState.h \
    ImageView.h \
    Data/Animator/Animator.h \
    Data/Animator/AnimatorType.h \
    Data/Animator/AnimationTypes.h \
    Data/ChannelUnits.h \
    Data/Clips.h \
    Data/Colormap/Colormap.h \
    Data/Colormap/Colormaps.h \
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
    Data/Snapshot/ISnapshotsImplementation.h \
    Data/Snapshot/Snapshots.h \
    Data/Snapshot/Snapshot.h \
    Data/Snapshot/SnapshotsFile.h \
    Data/Statistics.h \
    Data/Colormap/TransformsData.h \
    Data/Colormap/TransformsImage.h \
    Data/Util.h \
    GrayColormap.h \
    Data/ViewManager.h \
    Data/ViewPlugins.h \
    GrayColormap.h \
    HackViewer.h \
    ImageRenderService.h \
    Histogram/HistogramGenerator.h \
    Histogram/HistogramSelection.h \
    Histogram/HistogramPlot.h \
    ScriptTester.h \
    ScriptFacade.h \
    Hacks/ImageViewController.h \
    Hacks/MainModel.h \
    Algorithms/quantileAlgorithms.h \
    ILayeredView.h

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
    State/UtilState.cpp \
    ImageView.cpp \
    Data/Animator/Animator.cpp \
    Data/Animator/AnimatorType.cpp \
    Data/Animator/AnimationTypes.cpp \
    Data/ChannelUnits.cpp \
    Data/Clips.cpp \
    Data/Colormap/Colormap.cpp \
    Data/Colormap/Colormaps.cpp \
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
    Data/Snapshot/Snapshots.cpp \
    Data/Snapshot/Snapshot.cpp \
    Data/Snapshot/SnapshotsFile.cpp \
    Data/Statistics.cpp \
    Data/Colormap/TransformsData.cpp \
    Data/Colormap/TransformsImage.cpp \
    Data/Util.cpp \
    Data/ViewManager.cpp \
    Data/ViewPlugins.cpp \
    ScriptedCommandListener.cpp \
    GrayColormap.cpp \
    HackViewer.cpp \
    Algorithms/RawView2QImageConverter.cpp \
    Histogram/HistogramGenerator.cpp \
    Histogram/HistogramSelection.cpp \
    Histogram/HistogramPlot.cpp \
    ScriptFacade.cpp \
    ScriptTester.cpp \
    ImageRenderService.cpp \
    Hacks/ImageViewController.cpp \
    Hacks/MainModel.cpp \
    Algorithms/quantileAlgorithms.cpp \
    ILayeredView.cpp


#message( "common            PWD=$$PWD")
#message( "common         IN_PWD=$$IN_PWD")
#message( "common _PRO_FILE_PWD_=$$_PRO_FILE_PWD_")
#message( "common        OUT_PWD=$$OUT_PWD")

#CONFIG += precompile_header
#PRECOMPILED_HEADER = stable.h
#QMAKE_CXXFLAGS += -H

INCLUDEPATH += $$absolute_path(../../../ThirdParty/rapidjson/include)

#INCLUDEPATH += ../../../ThirdParty/qwt/include
#LIBS += -L../../../ThirdParty/qwt/lib -lqwt

QWT_ROOT = $$absolute_path("../../../ThirdParty/qwt")
INCLUDEPATH += $$QWT_ROOT/include
unix:macx {
	QMAKE_LFLAGS += '-F$$QWT_ROOT/lib'
	LIBS +=-L../CartaLib -lCartaLib -framework qwt
}
else{
	QMAKE_LFLAGS += '-Wl,-rpath,\'$$QWT_ROOT/lib\''
	LIBS +=-L../CartaLib -lCartaLib -L$$QWT_ROOT/lib -lqwt
}

DEPENDPATH += $$PROJECT_ROOT/CartaLib
