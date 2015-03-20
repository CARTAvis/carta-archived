! include(../common.pri) {
  error( "Could not find the common.pri file!" )
}

QT      +=  webkitwidgets network widgets xml

HEADERS += \
    MainWindow.h \
    CustomWebPage.h \
    DesktopPlatform.h \
    DesktopConnector.h

SOURCES += \
    MainWindow.cpp \
    CustomWebPage.cpp \
    DesktopPlatform.cpp \
    desktopMain.cpp \
    DesktopConnector.cpp

RESOURCES = resources.qrc

INCLUDEPATH += ../../../ThirdParty/rapidjson/include

unix: LIBS += -L$$OUT_PWD/../common/ -lcommon
unix: PRE_TARGETDEPS += $$OUT_PWD/../common/libcommon.so
DEPENDPATH += $$PROJECT_ROOT/common

unix: LIBS += -L$$OUT_PWD/../CartaLib/ -lCartaLib
unix: PRE_TARGETDEPS += $$OUT_PWD/../CartaLib/libCartaLib.so
DEPENDPATH += $$PROJECT_ROOT/CartaLib
QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/../CartaLib:\$$ORIGIN/../common\''

QWT_ROOT = $$absolute_path("../../../ThirdParty/qwt")
QMAKE_LFLAGS += '-Wl,-rpath,\'$$QWT_ROOT/lib\''
