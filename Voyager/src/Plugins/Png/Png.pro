QT       += core gui

TARGET = Png
TEMPLATE = lib
CONFIG += plugin

#DESTDIR = $$[QT_INSTALL_PLUGINS]/generic

SOURCES += VoyPlug.cpp

HEADERS += VoyPlug.h
OTHER_FILES += VoyagerViewer.json

#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}

QMAKE_CXXFLAGS += -std=c++0x
QMAKE_LFLAGS += -std=c++0x
