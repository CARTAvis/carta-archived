#!/bin/bash

yell() { echo "$0: $*" >&2; }
die() { yell "$*"; exit 111; }
try() { "$@" || die "cannot $*"; }

try py.test -v test_cartavis.py::test_getPixelValue
try py.test -v test_cartavis.py::test_getChannelCount
try py.test -v test_cartavis.py::test_getPixelUnits
try py.test -v test_cartavis.py::test_getImageDimensions
try py.test -v test_cartavis.py::test_zoomLevel
try py.test -v test_cartavis.py::test_getCoordinates
try py.test -v test_cartavis.py::test_saveFullImage
#try py.test -v test_cartavis.py::test_setColormap
try py.test -v test_cartavis.py::test_setImage
#try py.test -v test_cartavis.py::test_invertColormap
#try py.test -v test_cartavis.py::test_reverseColormap
try py.test -v test_cartavis.py::test_setColorMix
try py.test -v test_cartavis.py::test_setDataTransform
try py.test -v test_cartavis.py::test_setGamma
try py.test -v test_cartavis.py::test_closeImage
try py.test -v test_cartavis.py::test_getImageNames
