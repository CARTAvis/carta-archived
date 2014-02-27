! include(../common.pri) {
  error( "Could not find the common.pri file!" )
}

TEMPLATE = lib
CONFIG += static
QT += core network gui

# INCLUDEPATH += $${MONGOINC}

INCLUDEPATH += $${CASACOREDIR}/include/casacore
INCLUDEPATH += $${WCSLIBDIR}/include
INCLUDEPATH += $${CFITSIODIR}/include


SOURCES = common.cpp \
        ConsoleColors.cpp \
    FitsViewerServer.cpp \
    FitsParser.cpp \
    ColormapFunction.cpp \
    ProfileService.cpp \
    RaiCache.cpp \
    RaiCacher.cpp \
    HistogramInfo.cpp \
    NFSSafe.cpp \
    CachedImageReader.cpp \
    RaiCacherService.cpp \
    AFile.cpp \
    RegionStatsService.cpp \
    Gaussian2dFitService.cpp \
    Optimization/LevMar.cpp \
    WcsHelper.cpp \
    Optimization/Gauss2d.cpp \
    Gaussian1dFitService.cpp \
    Optimization/Gauss1d.cpp \
    Optimization/PolynomialFitter1D.cpp \
    LinearMap1D.cpp \
    IGraphLabeler.cpp


HEADERS += \
    common.h \
    ConsoleColors.h \
    FitsViewerServer.h \
    FitsParser.h \
    ColormapFunction.h \
    ProfileService.h \
    RaiCache.h \
    RaiCacher.h \
    HistogramInfo.h \
    NFSSafe.h \
    CachedImageReader.h \
    RaiCacherService.h \
    AFile.h \
    RegionStatsService.h \
    WcsHelper.h \
    Gaussian2dFitService.h \
    Optimization/LevMar.h \
    Optimization/Gauss2d.h \
    Optimization/TAGauss2dFitter.h \
    Optimization/LBTAGauss2dFitter.h \
    Optimization/LMGaussFitter2d.h \
    Optimization/HeuristicGauss2dFitter.h \
    Gaussian1dFitService.h \
    Optimization/LMGaussFitter1d.h \
    Optimization/LBTAGauss1dFitter.h \
    Optimization/HeuristicGauss1dFitter.h \
    Optimization/Gauss1d.h \
    Optimization/PolynomialFitter1D.h \
    LinearMap1D.h \
    IGraphLabeler.h

#LIBS += -L$$OUT_PWD/../CoordinatesLib/ -lCoordinatesLib

#INCLUDEPATH += $$PWD/../CoordinatesLib
#DEPENDPATH += $$PWD/../CoordinatesLib

#PRE_TARGETDEPS += $$OUT_PWD/../CoordinatesLib/libCoordinatesLib.a

