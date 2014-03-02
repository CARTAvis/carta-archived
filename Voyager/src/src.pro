TEMPLATE = subdirs
SUBDIRS += Viewer
SUBDIRS += Plugins

CONFIG += ordered

QMAKE_CXXFLAGS += -std=c++0x
QMAKE_LFLAGS += -std=c++0x
