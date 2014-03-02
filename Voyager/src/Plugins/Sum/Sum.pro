QT       += core gui

TARGET = Sum
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
