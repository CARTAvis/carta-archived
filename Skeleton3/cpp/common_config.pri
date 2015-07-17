# File: common_config.pri
# Description: this file contains pointers to all third party software 
# packages required to build plugins

CASACOREDIR=../../ThirdParty/casacore-2.0.1-shared
ASTLIBDIR = ../../ThirdParty/ast-8.0.2
WCSLIBDIR=../../ThirdParty/wcslib-4.23-shared
CFITSIODIR=../../ThirdParty/cfitsio-3360-shared

CASACOREDIR=$$absolute_path($${CASACOREDIR})
ASTLIBDIR=$$absolute_path($${ASTLIBDIR})
WCSLIBDIR=$$absolute_path($${WCSLIBDIR})
CFITSIODIR=$$absolute_path($${CFITSIODIR})
