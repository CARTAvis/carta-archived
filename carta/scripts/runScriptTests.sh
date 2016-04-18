#!/bin/bash

yell() { echo "$0: $*" >&2; }
die() { yell "$*"; exit 111; }
try() { "$@" || die "cannot $*"; }

try py.test -v test_cartavis.py::test_addLink
try py.test -v test_cartavis.py::test_centerImage
try py.test -v test_cartavis.py::test_closeImage
try py.test -v test_cartavis.py::test_getAnimatorViews
try py.test -v test_cartavis.py::test_getCenterPixel
try py.test -v test_cartavis.py::test_getColormapViews
try py.test -v test_cartavis.py::test_getColormaps
try py.test -v test_cartavis.py::test_getCoordinates
try py.test -v test_cartavis.py::test_getEmptyWindowCount
try py.test -v test_cartavis.py::test_getHistogramViews
try py.test -v test_cartavis.py::test_getImageDimensions
try py.test -v test_cartavis.py::test_getImageNames
try py.test -v test_cartavis.py::test_getImageViews
try py.test -v test_cartavis.py::test_getIntensity
#try py.test -v test_cartavis.py::test_getLinkedAnimators
try py.test -v test_cartavis.py::test_getMaxImageCount
try py.test -v test_cartavis.py::test_getPixelUnits
try py.test -v test_cartavis.py::test_getPixelValue
try py.test -v test_cartavis.py::test_getPluginList
try py.test -v test_cartavis.py::test_isEmpty
try py.test -v test_cartavis.py::test_loadFile
try py.test -v test_cartavis.py::test_removeLink
#try py.test -v test_cartavis.py::test_saveFullImage
try py.test -v test_cartavis.py::test_setAnalysisLayout
try py.test -v test_cartavis.py::test_setCPPColormap
try py.test -v test_cartavis.py::test_setColorMix
try py.test -v test_cartavis.py::test_setCustomLayout
try py.test -v test_cartavis.py::test_setDataTransform
#try py.test -v test_cartavis.py::test_setDefaultColormap
try py.test -v test_cartavis.py::test_setEmptyWindowPlugin
try py.test -v test_cartavis.py::test_setGamma
#try py.test -v test_cartavis.py::test_setImage
try py.test -v test_cartavis.py::test_setImageLayout
try py.test -v test_cartavis.py::test_zoomLevel
try py.test -v test_cartavis.py::test_getChannelCount
try py.test -v test_cartavis.py::test_setChannel
