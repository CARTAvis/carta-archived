! include(../../common.pri) {
  error( "Could not find the common.pri file!" )
}

QT       += core gui
TARGET = plugin
TEMPLATE = lib
CONFIG += plugin

SOURCES += \
    Fitter1D.cpp \
    Gaussian1dFitService.cpp \
    Gauss1d.cpp \
    PolynomialFitter1D.cpp \
    LevMar.cpp

HEADERS += \
    Fitter1D.h \
    Gaussian1dFitService.h \
    Gauss1d.h \
    HeuristicGauss1dFitter.h \
    PolynomialFitter1D.h \
    LBTAGauss1dFitter.h \
    LMGaussFitter1d.h \
    LevMar.h

GSLROOTDIR=/usr/local

INCLUDEPATH += $${GSLROOTDIR}/include
LIBS += -L$$GSLROOTDIR/lib -lgsl
LIBS += -L$$OUT_PWD/../../core/ -lcore
LIBS += -L$$OUT_PWD/../../CartaLib/ -lCartaLib

DEPENDPATH += $$PWD/../../core

OTHER_FILES += \
    plugin.json

# copy json to build directory
MYFILES = plugin.json
! include($$top_srcdir/cpp/copy_files.pri) {
  error( "Could not include $$top_srcdir/cpp/copy_files.pri file!" )
}

unix:macx {
    PRE_TARGETDEPS += $$OUT_PWD/../../core/libcore.dylib
    QMAKE_LFLAGS += -undefined dynamic_lookup
}
else{
    PRE_TARGETDEPS += $$OUT_PWD/../../core/libcore.so
}


