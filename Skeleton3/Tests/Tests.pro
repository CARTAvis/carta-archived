! include(../common.pri) {
  error( "Could not find the common.pri file!" )
}

QT      +=  core
HEADERS += catch.h

unix: LIBS += -L$$OUT_PWD/../common/ -lcommon
unix: PRE_TARGETDEPS += $$OUT_PWD/../common/libcommon.a

SOURCES += \
    TopoSortTest.cpp \
    mainTester.cpp

#CONFIG += precompile_header
#PRECOMPILED_HEADER = catch.h
#QMAKE_CXXFLAGS += -H
