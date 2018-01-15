! include(../../common.pri) {
  error( "Could not find the common.pri file!" )
}

QT       += core gui
TARGET = plugin
TEMPLATE = lib
CONFIG += plugin

SOURCES += \
    CasaImageLoader.cpp \
    CCImage.cpp \
    CCMetaDataInterface.cpp \
    CCRawView.cpp \
    CCCoordinateFormatter.cpp

HEADERS += \
    CasaImageLoader.h \
    CCImage.h \
    CCMetaDataInterface.h \
    CCRawView.h \
    CCCoordinateFormatter.h

casacoreLIBS += -L$${CASACOREDIR}/lib
casacoreLIBS += -lcasa_lattices -lcasa_tables -lcasa_scimath -lcasa_scimath_f -lcasa_mirlib
casacoreLIBS += -lcasa_casa -llapack -lblas -ldl
casacoreLIBS += -lcasa_images -lcasa_coordinates -lcasa_fits -lcasa_measures

LIBS += $${casacoreLIBS}
LIBS += -L$${WCSLIBDIR}/lib -lwcs
LIBS += -L$${CFITSIODIR}/lib -lcfitsio
LIBS += -L$$OUT_PWD/../../core/ -lcore
LIBS += -L$$OUT_PWD/../../CartaLib/ -lCartaLib

INCLUDEPATH += $${CASACOREDIR}/include
INCLUDEPATH += $${WCSLIBDIR}/include
INCLUDEPATH += $${CFITSIODIR}/include
#INCLUDEPATH += $$PWD/../../core
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
}
else{
    PRE_TARGETDEPS += $$OUT_PWD/../../core/libcore.so
}

## will improve mac later
unix:!macx {

  # the below line seems not effect
  QMAKE_RPATHDIR=$$OUT_PWD/../../../../../CARTAvis-externals/ThirdParty/casa/trunk/linux/lib

  # if we do not setup QMAKE_RPATHDIR+=$${WCSLIBDIR}/lib,
  # CasaImageLoader will have two duplicate linking to wcslib.
  # case1: both are "can not found out"
  # case2: use @ORIGIN for wcslib, one of them can be found out, but the result is the same (plugin not work)
  # QMAKE_RPATHDIR+=$ORIGIN/../../../../CARTAvis-externals/ThirdParty/wcslib/lib
  # the previous solution is to setup LD_LIBRARY_PATH

  QMAKE_RPATHDIR+=$${WCSLIBDIR}/lib

}
else {

}
