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
copy_files.name = copy large files
copy_files.input = MYFILES
# change datafiles to a directory you want to put the files to
copy_files.output = $${OUT_PWD}/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_files.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_files.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_files

unix:macx {
    PRE_TARGETDEPS += $$OUT_PWD/../../core/libcore.dylib
}
else{
    PRE_TARGETDEPS += $$OUT_PWD/../../core/libcore.so
}

