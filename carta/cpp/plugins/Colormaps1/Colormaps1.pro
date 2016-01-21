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
! include($$top_srcdir/cpp/copy_files.pri) {
  error( "Could not include $$top_srcdir/cpp/copy_files.pri file!" )
}

unix: LIBS += -L$$OUT_PWD/../../core/ -lcore
#INCLUDEPATH += $$PWD/../../core
DEPENDPATH += $$PWD/../../core

unix:macx {
    PRE_TARGETDEPS += $$OUT_PWD/../../core/libcore.dylib
}
else{
    PRE_TARGETDEPS += $$OUT_PWD/../../core/libcore.so
}

