! include(../common.pri) {
  error( "Could not find the common.pri file!" )
}

QT      +=  webkitwidgets network widgets

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

unix: LIBS += -L$$OUT_PWD/../common/ -lcommon
unix: PRE_TARGETDEPS += $$OUT_PWD/../common/libcommon.a

DEPENDPATH += $$PROJECT_ROOT/common
