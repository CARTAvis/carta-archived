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
    LinearMap.cpp \
    Hooks/ColormapsScalar.cpp \
    Hooks/Histogram.cpp \
    Hooks/HistogramResult.cpp \
    IImage.cpp \
    PixelType.cpp \
    Slice.cpp \
    AxisInfo.cpp \
    AxisLabelInfo.cpp \
    AxisDisplayInfo.cpp \
    ICoordinateFormatter.cpp \
    IPlotLabelGenerator.cpp \
    Hooks/LoadAstroImage.cpp \
    PixelPipeline/CustomizablePixelPipeline.cpp \
    PWLinear.cpp \
    VectorGraphics/VGList.cpp \
    VectorGraphics/BetterQPainter.cpp \
    Algorithms/ContourConrec.cpp \
    IWcsGridRenderService.cpp \
    ContourSet.cpp \
    Algorithms/LineCombiner.cpp \
    IImageRenderService.cpp \
    IRemoteVGView.cpp

HEADERS += \
    CartaLib.h\
    cartalib_global.h \
    HtmlString.h \
    LinearMap.h \
    Hooks/ColormapsScalar.h \
    Hooks/Histogram.h \
    Hooks/HistogramResult.h \
    Hooks/HookIDs.h \
    IPlugin.h \
    IImage.h \
    PixelType.h \
    Nullable.h \
    Slice.h \
    AxisInfo.h \
    AxisLabelInfo.h \
    AxisDisplayInfo.h \
    ICoordinateFormatter.h \
    IPlotLabelGenerator.h \
    Hooks/LoadAstroImage.h \
    TPixelPipeline/IScalar2Scalar.h \
    PixelPipeline/IPixelPipeline.h \
    PixelPipeline/CustomizablePixelPipeline.h \
    PWLinear.h \
    VectorGraphics/VGList.h \
    Hooks/GetWcsGridRenderer.h \
    Hooks/LoadPlugin.h \
    VectorGraphics/BetterQPainter.h \
    Algorithms/ContourConrec.h \
    IWcsGridRenderService.h \
    IContourGeneratorService.h \
    ContourSet.h \
    Algorithms/LineCombiner.h \
    Hooks/GetInitialFileList.h \
    Hooks/Initialize.h \
    IImageRenderService.h \
    Hooks/GetImageRenderService.h \
    IRemoteVGView.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

OTHER_FILES += \
    readme.txt

DISTFILES += \
    VectorGraphics/vg.txt
