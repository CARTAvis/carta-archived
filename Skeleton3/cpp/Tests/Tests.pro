! include(../common.pri) {
  error( "Could not find the common.pri file!" )
}

QT      +=  core
HEADERS += catch.h

SOURCES += \
    TopoSortTest.cpp \
    mainTester.cpp \
    SliceTester.cpp \
    StateTester.cpp \
    pixelPipelineTest.cpp

#CONFIG += precompile_header
#PRECOMPILED_HEADER = catch.h
#QMAKE_CXXFLAGS += -H

unix: LIBS += -L$$OUT_PWD/../common/ -lcommon
DEPENDPATH += $$PROJECT_ROOT/common

unix: LIBS += -L$$OUT_PWD/../CartaLib/ -lCartaLib
DEPENDPATH += $$PROJECT_ROOT/CartaLib

QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/../CartaLib\''
unix:macx {
    PRE_TARGETDEPS += $$OUT_PWD/../common/libcommon.dylib
    PRE_TARGETDEPS += $$OUT_PWD/../CartaLib/libCartaLib.dylib
    LIBS+=-L/usr/local/lib
}
else{
    PRE_TARGETDEPS += $$OUT_PWD/../common/libcommon.so
    PRE_TARGETDEPS += $$OUT_PWD/../CartaLib/libCartaLib.so
}

