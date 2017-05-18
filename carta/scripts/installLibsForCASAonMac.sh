#!/bin/bash

### Basic tools
brew install cmake
brew link --overwrite cmake
brew install wget

### For building libsakura
brew install doxygen
brew install eigen@3.2 ## eigen is 3.3.3 but its 3.3.3 is not compatible with libsakura 4.0.2065
brew install log4cxx

### For CASA - submodule - casacore
brew install gcc ## only use its gfortran
brew install fftw
brew install boost-python ## also used by code submodules

### For CASA - submodule - code - imageanalysis, stdcasa, components, the following all are needed by **submodule-code**
brew install readline

# PGPLOT: 5.2.2_2.
# We modify code's cmake to build without pgplot but the cmake's check is still there,
# so keep installing pgplot to pass the check. imageanalysis's testing code needs this (`<casa/System/PGPlotter.h>`)
# but we remove the testing part of it. casacore's lattices' source codes have "pgplot" insdie but seems not need pgplot when compiling casacore
#if you already have xquartz installed, it will not install again. this is needed by pgplot
# brew cask install xquartz 
brew install --ignore-dependencies pgplot

brew install dbus # To pass cmake's check

brew install libxml2
brew install xerces-c

## Qt 4.8. Not sure if the target three submodules need Qt but cmake's check will check if Qt exists
# brew tap cartr/qt4
# brew tap-pin cartr/qt4
# brew install qt # /usr/local/Cellar/qt/4.8.7_3
## brew install cartr/qt4/qwt-qt4, somehow fail, so just build from source code.

###########################
