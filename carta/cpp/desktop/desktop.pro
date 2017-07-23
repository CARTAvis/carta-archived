! include(../common.pri) {
  error( "Could not find the common.pri file!" )
}

QT      +=  webkitwidgets network widgets xml

HEADERS += \
    MainWindow.h \
    CustomWebPage.h \
    DesktopPlatform.h \
    DesktopConnector.h \
    NetworkReplyFileq.h \
    NetworkAccessManager.h

SOURCES += \
    MainWindow.cpp \
    CustomWebPage.cpp \
    DesktopPlatform.cpp \
    desktopMain.cpp \
    DesktopConnector.cpp \
    NetworkAccessManager.cpp \
    NetworkReplyFileq.cpp

RESOURCES = resources.qrc

INCLUDEPATH += ../../../ThirdParty/rapidjson/include

unix: LIBS += -L$$OUT_PWD/../core/ -lcore
DEPENDPATH += $$PROJECT_ROOT/core

QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/../core\''

QWT_ROOT = $$absolute_path("../../../ThirdParty/qwt")
unix:macx {
    QMAKE_LFLAGS += '-F$$QWT_ROOT/lib'
    LIBS +=-framework qwt
    PRE_TARGETDEPS += $$OUT_PWD/../core/libcore.dylib
}
else{
    QMAKE_LFLAGS += '-Wl,-rpath,\'$$QWT_ROOT/lib\''
    LIBS +=-L$$QWT_ROOT/lib -lqwt
    PRE_TARGETDEPS += $$OUT_PWD/../core/libcore.so
}

# set the name of Desktop Application
TARGET = CARTA

# for release builds
carta_qrc {

PREPROCESS_FILES = .
preprocess.name = autogenerate qrc file for release mode
preprocess.input = PREPROCESS_FILES
preprocess.output = files.qrc
preprocess.commands = touch files.qrc
preprocess.variable_out = RESOURCES
QMAKE_EXTRA_COMPILERS += preprocess

}
