! include(../common.pri) {
  error( "Could not find the common.pri file!" )
}

QT += core \
    gui \
    network \
    script

DEFINES += NDEBUG
#QMAKE_CXXFLAGS += -Wno-unknown-pragmassss
QMAKE_CXXFLAGS_WARN_ON += -Wno-unknown-pragmas

#CONFIG -= warn_off
#CONFIG += warn_on

TARGET = DesktopFitsViewer
TEMPLATE = app
LIBS += -L../FitsViewerLib \
    -lFitsViewerLib
DEPENDPATH += ../FitsViewerLib . ..

#LIBS += -L../lib \
#LIBS += -L$${PUREWEBSDKDIR}/lib/C++ \
#    -lCSI.Standard64 \
#    -lCSI.Typeless64 \
#    -lCSI.PureWeb.ImageProcessing64 \
#    -lCSI.PureWeb.StateManager64 \
#    $${PUREWEBSDKDIR}/lib/C++/libicuuc.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libicui18n.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libicudata.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libicuio.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libicule.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libiculx.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libicutu.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libjpeg.so.62

#LIBS += -L$${PUREWEBSDKDIR}/lib/C++ \
#    -lCSI.Standard64 \
#    -lCSI.Typeless64 \
#    -lCSI.PureWeb.ImageProcessing64 \
#    -lCSI.PureWeb.StateManager64 \
#    $${PUREWEBSDKDIR}/lib/C++/libicuuc.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libicui18n.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libicudata.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libicuio.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libicule.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libiculx.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libicutu.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libjpeg.so.62

#LIBS += \
#    $${PUREWEBSDKDIR}/lib/C++/libCSI.Standard64.so \
#    $${PUREWEBSDKDIR}/lib/C++/libCSI.Typeless64.so \
#    $${PUREWEBSDKDIR}/lib/C++/libCSI.PureWeb.ImageProcessing64.so \
#    $${PUREWEBSDKDIR}/lib/C++/libCSI.PureWeb.StateManager64.so \
#    $${PUREWEBSDKDIR}/lib/C++/libicuuc.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libicui18n.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libicudata.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libicuio.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libicule.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libiculx.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libicutu.so.46 \
#    $${PUREWEBSDKDIR}/lib/C++/libjpeg.so.62

LIBS += \
    $${PUREWEBSDKDIR}/Redistributable/Libs/C++/lib/libCSI.Standard64.so \
    $${PUREWEBSDKDIR}/Redistributable/Libs/C++/lib/libCSI.Typeless64.so \
    $${PUREWEBSDKDIR}/Redistributable/Libs/C++/lib/libCSI.PureWeb.ImageProcessing64.so \
    $${PUREWEBSDKDIR}/Redistributable/Libs/C++/lib/libCSI.PureWeb.StateManager64.so \
    $${PUREWEBSDKDIR}/Redistributable/Libs/C++/lib/libicuuc.so.46 \
    $${PUREWEBSDKDIR}/Redistributable/Libs/C++/lib/libicui18n.so.46 \
    $${PUREWEBSDKDIR}/Redistributable/Libs/C++/lib/libicudata.so.46 \
    $${PUREWEBSDKDIR}/Redistributable/Libs/C++/lib/libicuio.so.46 \
    $${PUREWEBSDKDIR}/Redistributable/Libs/C++/lib/libicule.so.46 \
    $${PUREWEBSDKDIR}/Redistributable/Libs/C++/lib/libiculx.so.46 \
    $${PUREWEBSDKDIR}/Redistributable/Libs/C++/lib/libicutu.so.46 \
    $${PUREWEBSDKDIR}/Redistributable/Libs/C++/lib/libjpeg.so.62

LIBS += $${casacoreLIBS}

LIBS += -L$$OUT_PWD/../CoordinatesLib/ -lCoordinatesLib
LIBS += $${astlibLIBS}
INCLUDEPATH += $$PWD/..
INCLUDEPATH += $${ASTLIBDIR}/include
DEPENDPATH += $$PWD/../CoordinatesLib
DEPENDPATH += $$PWD/..
PRE_TARGETDEPS += $$OUT_PWD/../CoordinatesLib/libCoordinatesLib.a

LIBS += $${gslLIBS}



POST_TARGETDEPS += ../FitsViewerLib/libFitsViewerLib.a
#INCLUDEPATH += ../../../../../../SDK/C++
#INCLUDEPATH += $${PUREWEBSDKDIR}/include
INCLUDEPATH += $${PUREWEBSDKDIR}/Redistributable/Libs/C++/include
DEPENDPATH += ../FitsViewerLib .


# link with electric fence
# LIBS += -lefence


SOURCES += main.cpp \
    MainWindow.cpp \
    GridOptionsDialog.cpp \
    AboutDialog.cpp \
    MovieDialog.cpp \
    FitsHeaderDialog.cpp \
    ScreenshotDialog.cpp \
    ChangeLogDialog.cpp \
    HelpDialog.cpp \
    RegionStatsDialog.cpp \
    GaussianDialog.cpp \
    ColormapDialog.cpp \
    HistogramDialog.cpp \
    CanvasWidget.cpp \
    FvController.cpp \
    TimeoutMonitor.cpp \
    DebuggingDialog.cpp \
    FileBrowserService.cpp \
    Gauss1DFitterController.cpp \
    ProfileController.cpp \
    StartupParameters.cpp \
    GlobalSettings.cpp \
    QUProfileController.cpp \
    PureWeb.cpp \
    GlobalState.cpp

HEADERS += MainWindow.h \
    GridOptionsDialog.h \
    AboutDialog.h \
    MovieDialog.h \
    FitsHeaderDialog.h \
    ScreenshotDialog.h \
    ChangeLogDialog.h \
    HelpDialog.h \
    RegionStatsDialog.h \
    GaussianDialog.h \
    ColormapDialog.h \
    HistogramDialog.h \
    CanvasWidget.h \
    FvController.h \
    PureWeb.h \
    PureWeb.h \
    TimeoutMonitor.h \
    DebuggingDialog.h \
    FileBrowserService.h \
    Gauss1DFitterController.h \
    ProfileController.h \
    StartupParameters.h \
    GlobalSettings.h \
    QUProfileController.h \
    GlobalState.h

FORMS += MainWindow.ui \
    GridOptionsDialog.ui \
    AboutDialog.ui \
    MovieDialog.ui \
    FitsHeaderDialog.ui \
    ScreenshotDialog.ui \
    ChangeLogDialog.ui \
    HelpDialog.ui \
    RegionStatsDialog.ui \
    GaussianDialog.ui \
    ColormapDialog.ui \
    HistogramDialog.ui \
    DebuggingDialog.ui \
    GaussFit1d.ui

RESOURCES += icons.qrc



