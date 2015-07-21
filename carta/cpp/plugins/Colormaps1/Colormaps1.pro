! include(../../common.pri) {
  error( "Could not find the common.pri file!" )
}

#INCLUDEPATH += $$PROJECT_ROOT
#DEPENDPATH += $$PROJECT_ROOT

QT       += core gui

TARGET = plugin
TEMPLATE = lib
CONFIG += plugin

SOURCES += \
    Colormaps1.cpp

HEADERS += \
    Colormaps1.h

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

unix: LIBS += -L$$OUT_PWD/../../core/ -lcore
#INCLUDEPATH += $$PWD/../../core
DEPENDPATH += $$PWD/../../core

unix:macx {
    PRE_TARGETDEPS += $$OUT_PWD/../../core/libcore.dylib
}
else{
    PRE_TARGETDEPS += $$OUT_PWD/../../core/libcore.so
}

