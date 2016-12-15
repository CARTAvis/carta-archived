! include(../../common.pri) {
  error( "Could not find the common.pri file!" )
}

QT       += core gui
TARGET = plugin
TEMPLATE = lib
CONFIG += plugin

SOURCES += \
    RegionRecordFactory.cpp \
    StatisticsCASA.cpp \
    StatisticsCASAImage.cpp \
    StatisticsCASARegion.cpp

HEADERS += \
    RegionRecordFactory.h \
    StatisticsCASA.h \
    StatisticsCASAImage.h \
    StatisticsCASARegion.h

casacoreLIBS += -L$${CASACOREDIR}/lib
casacoreLIBS += -lcasa_lattices -lcasa_tables -lcasa_scimath -lcasa_scimath_f -lcasa_mirlib
casacoreLIBS += -lcasa_casa -llapack -lblas -ldl
casacoreLIBS += -lcasa_images -lcasa_coordinates -lcasa_fits -lcasa_measures

LIBS += $${casacoreLIBS}
LIBS += -L$${WCSLIBDIR}/lib -lwcs
LIBS += -L$${CFITSIODIR}/lib -lcfitsio
LIBS += -L$${IMAGEANALYSISDIR}/lib -limageanalysis
LIBS += -L$$OUT_PWD/../../plugins/CasaImageLoader -lplugin
LIBS += -L$$OUT_PWD/../../core/ -lcore
LIBS += -L$$OUT_PWD/../../CartaLib/ -lCartaLib

INCLUDEPATH += $${CASACOREDIR}/include
INCLUDEPATH += $${CASACOREDIR}/include/casacore
INCLUDEPATH += $${WCSLIBDIR}/include
INCLUDEPATH += $${CFITSIODIR}/include
INCLUDEPATH += $${IMAGEANALYSISDIR}/include
warning( $$INCLUDEPATH )

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
