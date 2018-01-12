TEMPLATE = subdirs
#CONFIG += ordered

SUBDIRS += casaCore
SUBDIRS += CasaImageLoader
SUBDIRS += Colormaps1
SUBDIRS += Fitter1D
SUBDIRS += Histogram
SUBDIRS += WcsPlotter
SUBDIRS += ConversionSpectral
SUBDIRS += ConversionSpectral/Test.pro
SUBDIRS += ConversionIntensity
SUBDIRS += ConversionIntensity/Test.pro
SUBDIRS += ImageAnalysis
SUBDIRS += ImageStatistics
SUBDIRS += RegionCASA
SUBDIRS += RegionDs9
SUBDIRS += ProfileCASA
SUBDIRS += qimage
SUBDIRS += python273
SUBDIRS += ColormapsPy
SUBDIRS += CyberSKA
SUBDIRS += DevIntegration
SUBDIRS += PCacheSqlite3
SUBDIRS += PercentileHistogram
SUBDIRS += PercentileManku99
SUBDIRS += PercentileManku99/Test.pro

unix:macx {
# Disable PCacheLevelDB plugin on Mac, since build carta error with LevelDB enabled.
}
else{
  SUBDIRS += PCacheLevelDB
}
