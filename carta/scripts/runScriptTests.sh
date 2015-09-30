#!/bin/bash

yell() { echo "$0: $*" >&2; }
die() { yell "$*"; exit 111; }
try() { "$@" || die "cannot $*"; }

py.test -v test_cartavis.py::test_getPixelValue
py.test -v test_cartavis.py::test_getChannelCount
py.test -v test_cartavis.py::test_getPixelUnits
py.test -v test_cartavis.py::test_getImageDimensions
py.test -v test_cartavis.py::test_zoomLevel
py.test -v test_cartavis.py::test_getCoordinates
py.test -v test_cartavis.py::test_saveFullImage
py.test -v test_cartavis.py::test_setColormap
py.test -v test_cartavis.py::test_setImage
py.test -v test_cartavis.py::test_invertColormap
py.test -v test_cartavis.py::test_reverseColormap
py.test -v test_cartavis.py::test_setColorMix
py.test -v test_cartavis.py::test_setDataTransform
py.test -v test_cartavis.py::test_setGamma
py.test -v test_cartavis.py::test_closeImage
py.test -v test_cartavis.py::test_getImageNames
