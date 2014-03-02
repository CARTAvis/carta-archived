QT       += core

QT       -= gui

TARGET = Voyager
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

QMAKE_CXXFLAGS += -std=c++0x
QMAKE_LFLAGS += -std=c++0x

HEADERS += \
    VoyagerPluginInterface.h

CASACOREDIR=$$(HOME)/Software/casacore-1.5.0-static
WCSLIBDIR=$$(HOME)/Software/wcslib-4.15
CFITSIODIR=$$(HOME)/Software/cfitsio-3.310

casacoreLIBS += -L$${CASACOREDIR}/lib
casacoreLIBS += -lcasa_images -lcasa_components -lcasa_coordinates -lcasa_fits -lcasa_measures
casacoreLIBS += -lcasa_lattices -lcasa_tables -lcasa_scimath -lcasa_scimath_f -lcasa_mirlib
casacoreLIBS += -lcasa_casa -llapack -lblas -lgfortran -ldl
casacoreLIBS += -L$${WCSLIBDIR}/lib -lwcs
casacoreLIBS += -L$${CFITSIODIR}/lib -lcfitsio

INCLUDEPATH += $${CASACOREDIR}/include/casacore
INCLUDEPATH += $${WCSLIBDIR}/include
INCLUDEPATH += $${CFITSIODIR}/include

LIBS += $${casacoreLIBS}
