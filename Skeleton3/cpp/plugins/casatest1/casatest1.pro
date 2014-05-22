PROJECT_ROOT = "../.."

! include(../../common.pri) {
  error( "Could not find the common.pri file!" )
}

INCLUDEPATH += "$$PROJECT_ROOT"

QT       += core gui

TARGET = casatest1
#QMAKE_EXTENSION_SHLIB = cartaplugin

TEMPLATE = lib
CONFIG += plugin

SOURCES += \
    CasaTest1.cpp

HEADERS += \
    CasaTest1.h

CASACOREDIR=$$(HOME)/Software/casacore-1.5.0-shared
WCSLIBDIR=$$(HOME)/Software/wcslib-4.23-shared
CFITSIODIR=$$(HOME)/Software/cfitsio3360shared

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
