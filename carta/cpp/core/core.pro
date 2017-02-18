! include(../common.pri) {
  error( "Could not find the common.pri file!" )
}

TEMPLATE = lib

###CONFIG += staticlib
QT += widgets network
QT += xml

SOURCES = $$files(*.cpp, true)
HEADERS = $$files(*.h  , true)

#message( "common            PWD=$$PWD")
#message( "common         IN_PWD=$$IN_PWD")
#message( "common _PRO_FILE_PWD_=$$_PRO_FILE_PWD_")
#message( "common        OUT_PWD=$$OUT_PWD")

#CONFIG += precompile_header
#PRECOMPILED_HEADER = stable.h
#QMAKE_CXXFLAGS += -H

INCLUDEPATH += $$absolute_path(../../../ThirdParty/rapidjson/include)

#INCLUDEPATH += ../../../ThirdParty/qwt/include
#LIBS += -L../../../ThirdParty/qwt/lib -lqwt

QWT_ROOT = $$absolute_path("../../../ThirdParty/qwt")
INCLUDEPATH += $$QWT_ROOT/include
unix:macx {
	QMAKE_LFLAGS += '-F$$QWT_ROOT/lib'
	LIBS +=-L../CartaLib -lCartaLib -framework qwt
}
else {
	QMAKE_LFLAGS += '-Wl,-rpath,\'$$QWT_ROOT/lib\''
	LIBS +=-L../CartaLib -lCartaLib -L$$QWT_ROOT/lib -lqwt
}

DEPENDPATH += $$PROJECT_ROOT/CartaLib
