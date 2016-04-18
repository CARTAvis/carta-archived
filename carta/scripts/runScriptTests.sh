#!/bin/bash

yell() { echo "$0: $*" >&2; }
die() { yell "$*"; exit 111; }
try() { "$@" || die "cannot $*"; }

py.test -v test_cartavis.py::test_addLink
py.test -v test_cartavis.py::test_centerImage
py.test -v test_cartavis.py::test_closeImage
py.test -v test_cartavis.py::test_getAnimatorViews
py.test -v test_cartavis.py::test_getCenterPixel
py.test -v test_cartavis.py::test_getColormapViews
py.test -v test_cartavis.py::test_getColormaps
py.test -v test_cartavis.py::test_getCoordinates
py.test -v test_cartavis.py::test_getEmptyWindowCount
py.test -v test_cartavis.py::test_getHistogramViews
py.test -v test_cartavis.py::test_getImageDimensions
py.test -v test_cartavis.py::test_getImageNames
py.test -v test_cartavis.py::test_getImageViews
py.test -v test_cartavis.py::test_getIntensity
py.test -v test_cartavis.py::test_getLinkedAnimators
py.test -v test_cartavis.py::test_getMaxImageCount
py.test -v test_cartavis.py::test_getPixelUnits
py.test -v test_cartavis.py::test_getPixelValue
py.test -v test_cartavis.py::test_getPluginList
py.test -v test_cartavis.py::test_isEmpty
py.test -v test_cartavis.py::test_loadFile
py.test -v test_cartavis.py::test_removeLink
#try py.test -v test_cartavis.py::test_saveFullImage
py.test -v test_cartavis.py::test_setAnalysisLayout
py.test -v test_cartavis.py::test_setCPPColormap
py.test -v test_cartavis.py::test_setColorMix
py.test -v test_cartavis.py::test_setCustomLayout
py.test -v test_cartavis.py::test_setDataTransform
#try py.test -v test_cartavis.py::test_setDefaultColormap
py.test -v test_cartavis.py::test_setEmptyWindowPlugin
py.test -v test_cartavis.py::test_setGamma
#try py.test -v test_cartavis.py::test_setImage
py.test -v test_cartavis.py::test_setImageLayout
py.test -v test_cartavis.py::test_zoomLevel
py.test -v test_cartavis.py::test_getChannelCount
py.test -v test_cartavis.py::test_setChannel
