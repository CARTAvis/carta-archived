! include(../common.pri) {
  error( "Could not find the common.pri file!" )
}

QT      +=  core
HEADERS += catch.h

SOURCES += \
    TopoSortTest.cpp \
    mainTester.cpp \
    SliceTester.cpp

#CONFIG += precompile_header
#PRECOMPILED_HEADER = catch.h
#QMAKE_CXXFLAGS += -H

unix: LIBS += -L$$OUT_PWD/../common/ -lcommon
unix: PRE_TARGETDEPS += $$OUT_PWD/../common/libcommon.a
DEPENDPATH += $$PROJECT_ROOT/common

unix: LIBS += -L$$OUT_PWD/../CartaLib/ -lCartaLib
unix: PRE_TARGETDEPS += $$OUT_PWD/../CartaLib/libCartaLib.so
DEPENDPATH += $$PROJECT_ROOT/CartaLib
QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/../CartaLib\''
