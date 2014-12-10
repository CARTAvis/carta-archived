! include(../common.pri) {
  error( "Could not find the common.pri file!" )
}

QT       += network xml

TARGET = CartaLib
TEMPLATE = lib

DEFINES += CARTALIB_LIBRARY

SOURCES += \
    CartaLib.cpp \
    HtmlString.cpp \
    IColormapScalar.cpp \
    Hooks/ColormapsScalar.cpp \
    IImage.cpp \
    PixelType.cpp \
    Slice.cpp \
    AxisInfo.cpp \
    ICoordinateFormatter.cpp \
    ICoordinateGridPlotter.cpp \
    IPlotLabelGenerator.cpp \
    Hooks/LoadAstroImage.cpp

HEADERS += \
    CartaLib.h\
    cartalib_global.h \
    HtmlString.h \
    IColormapScalar.h \
    Hooks/ColormapsScalar.h \
    Hooks/HookIDs.h \
    IPlugin.h \
    IImage.h \
    PixelType.h \
    Nullable.h \
    Slice.h \
    AxisInfo.h \
    ICoordinateFormatter.h \
    ICoordinateGridPlotter.h \
    IPlotLabelGenerator.h \
    Hooks/LoadAstroImage.h \
    PixelPipeline/Id2d.h \
    TPixelPipeline/IScalar2Scalar.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

OTHER_FILES += \
    readme.txt
