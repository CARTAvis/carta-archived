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

INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..
unix: LIBS += -L$$OUT_PWD/../common/ -lcommon
unix: PRE_TARGETDEPS += $$OUT_PWD/../common/libcommon.a

#CASACOREDIR=$$(HOME)/Software/casacore-1.5.0-shared
#WCSLIBDIR=$$(HOME)/Software/wcslib-4.23-shared
#CFITSIODIR=$$(HOME)/Software/cfitsio3360shared

#casacoreLIBS += -L$${CASACOREDIR}/lib
#casacoreLIBS += -lcasa_images -lcasa_components -lcasa_coordinates -lcasa_fits -lcasa_measures
#casacoreLIBS += -lcasa_lattices -lcasa_tables -lcasa_scimath -lcasa_scimath_f -lcasa_mirlib
#casacoreLIBS += -lcasa_casa -llapack -lblas -lgfortran -ldl
#casacoreLIBS += -L$${WCSLIBDIR}/lib -lwcs
#casacoreLIBS += -L$${CFITSIODIR}/lib -lcfitsio

#INCLUDEPATH += $${CASACOREDIR}/include/casacore
#INCLUDEPATH += $${WCSLIBDIR}/include
#INCLUDEPATH += $${CFITSIODIR}/include

#LIBS += $${casacoreLIBS}
