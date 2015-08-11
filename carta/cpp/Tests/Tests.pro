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
    pixelPipelineTest.cpp \
    LineCombinerTest.cpp

#CONFIG += precompile_header
#PRECOMPILED_HEADER = catch.h
#QMAKE_CXXFLAGS += -H

unix: LIBS += -L$$OUT_PWD/../core/ -lcore
DEPENDPATH += $$PROJECT_ROOT/core

unix: LIBS += -L$$OUT_PWD/../CartaLib/ -lCartaLib
DEPENDPATH += $$PROJECT_ROOT/CartaLib

QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/../CartaLib\''
QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/../core\''
unix:macx {
    PRE_TARGETDEPS += $$OUT_PWD/../core/libcore.dylib
    PRE_TARGETDEPS += $$OUT_PWD/../CartaLib/libCartaLib.dylib
    LIBS+=-L/usr/local/lib
}
else{
    PRE_TARGETDEPS += $$OUT_PWD/../core/libcore.so
    PRE_TARGETDEPS += $$OUT_PWD/../CartaLib/libCartaLib.so
}

