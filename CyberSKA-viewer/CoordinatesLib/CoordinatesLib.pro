! include(../common.pri) {
  error( "Could not find the common.pri file!" )
}

MY_ROOT = $$PWD/../

TARGET    = CoordinatesLib
TEMPLATE  = lib
CONFIG   += staticlib
QT       += core gui network

SOURCES  += grfdriver.cpp \
            SkyGridPlotter.cpp \
    WcsHero.cpp \
    UnitAnalyzer.cpp

INCLUDEPATH += $${ASTLIBDIR}/include

#INCLUDEPATH += $$PWD/../FitsViewerLib
#DEPENDPATH += $$PWD/../FitsViewerLib

INCLUDEPATH += $${MY_ROOT}
DEPENDPATH += $${MY_ROOT}

PRE_TARGETDEPS += $$OUT_PWD/../FitsViewerLib/libFitsViewerLib.a

HEADERS += \
    SkyGridPlotter.h \
    grfdriver.h \
    WcsHero.h \
    UnitAnalyzer.h
