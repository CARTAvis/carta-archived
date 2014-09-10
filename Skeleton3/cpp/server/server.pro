! include(../common.pri) {
  error( "Could not find the common.pri file!" )
}

CONFIG += qt
#CONFIG += warn_off
QT += widgets
QT += xml

HEADERS       = \
    ServerPlatform.h \
    ServerConnector.h \
    StateXmlWriter.h 

SOURCES       = \
    ServerPlatform.cpp \
    ServerConnector.cpp \
    StateXmlWriter.cpp \
    serverMain.cpp 
    


# dependencies


# pureweb related stuff
PUREWEB_LIBS=$$(HOME)/Work/Software/PureWeb/4.1.1/SDK/Redistributable/Libs

win32:INCLUDEPATH += "$${PUREWEB_LIBS}/C++/VS2010/include"
win32:LIBS += $${PUREWEB_LIBS}/C++/VS2010/lib/CSI.PureWeb.ImageProcessing32d.lib
win32:LIBS += $${PUREWEB_LIBS}/C++/VS2010/lib/CSI.PureWeb.StateManager32d.lib
win32:LIBS += $${PUREWEB_LIBS}/C++/VS2010/lib/CSI.Standard32d.lib
win32:LIBS += $${PUREWEB_LIBS}/C++/VS2010/lib/CSI.Typeless32d.lib

unix:INCLUDEPATH += $${PUREWEB_LIBS}/C++/include

unix:QMAKE_CXXFLAGS += -isystem $${PUREWEB_LIBS}/C++/include

unix:LIBS += $${PUREWEB_LIBS}/C++/lib/libCSI.PureWeb.ImageProcessing64.so
unix:LIBS += $${PUREWEB_LIBS}/C++/lib/libCSI.PureWeb.StateManager64.so
unix:LIBS += $${PUREWEB_LIBS}/C++/lib/libCSI.Standard64.so
unix:LIBS += $${PUREWEB_LIBS}/C++/lib/libCSI.Typeless64.so

unix:LIBS += $${PUREWEB_LIBS}/C++/lib/libicudata.so.46
unix:LIBS += $${PUREWEB_LIBS}/C++/lib/libicui18n.so.46
unix:LIBS += $${PUREWEB_LIBS}/C++/lib/libicuio.so.46
unix:LIBS += $${PUREWEB_LIBS}/C++/lib/libicule.so.46
unix:LIBS += $${PUREWEB_LIBS}/C++/lib/libiculx.so.46
unix:LIBS += $${PUREWEB_LIBS}/C++/lib/libicutu.so.46
unix:LIBS += $${PUREWEB_LIBS}/C++/lib/libicuuc.so.46
unix:LIBS += $${PUREWEB_LIBS}/C++/lib/libjpeg.so.62


DEPENDPATH += $$$PROJECT_ROOT/common

unix: LIBS += -L$$OUT_PWD/../common/ -lcommon
unix: PRE_TARGETDEPS += $$OUT_PWD/../common/libcommon.a
