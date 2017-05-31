#!/bin/bash

### For CASA - submodule - casacore
brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/cb79fc2ec2c5fcebf35f7bf26bb7459c9f87ae0b/Formula/fftw.rb
brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/198f8903db0bb77f1b84e19e020c6f825210433d/Formula/boost-python.rb ## also used by code submodules

### For CASA - submodule - code - imageanalysis, stdcasa, components, the following all are needed by **submodule-code**

### For building libsakura
brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/0ee09d116505e7a5cbc4e2b94335fca82a69e146/Formula/doxygen.rb
brew install eigen@3.2 ## the latest of eigen is 3.3.3 but its 3.3.3 is not compatible with libsakura 4.0.2065
brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/712fb0b23bad079214c28e325cd82548b216dcab/Formula/log4cxx.rb
###

brew install readline

# PGPLOT: 5.2.2_2.
# We modify code's cmake to build without pgplot but the cmake's check is still there,
# so keep installing pgplot to pass the check. imageanalysis's testing code needs this (`<casa/System/PGPlotter.h>`)
# but we remove the testing part of it. casacore's lattices' source codes have "pgplot" insdie but seems not need pgplot when compiling casacore
#if you already have xquartz installed, it will not install again. this is needed by pgplot
# brew cask install xquartz
brew install --ignore-dependencies https://raw.githubusercontent.com/Homebrew/homebrew-core/1fa20e6da369410aa02a7d467ac0d24f36579d13/Formula/pgplot.rb

brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/70cdd3ed9ab1e4b7721f9884d363745e46a68539/Formula/dbus.rb # To pass cmake's check

brew install libxml2
brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/8bac3e33d64f57a18b328b8284d37cb40979c481/Formula/xerces-c.rb

## Qt 4.8. Not sure if the target three submodules need Qt but cmake's check will check if Qt exists
# brew tap cartr/qt4
# brew tap-pin cartr/qt4
# brew install qt # /usr/local/Cellar/qt/4.8.7_3
## brew install cartr/qt4/qwt-qt4, somehow fail, so just build from source code.

###########################
