#!/bin/bash

DIR=""
DIR=`dirname "$0"`; DIR=`eval "cd \"$DIR\" && pwd"`
export cartawork=$DIR/../../..
echo "CARTA working folder is $cartawork"
source $cartawork/CARTAvis/carta/scripts/ci_mac_common.sh

su $SUDO_USER <<EOF
if brew ls --versions cmake ; then
  echo "cmake is already installed"
else
  brew install cmake
  brew link --overwrite cmake
fi
EOF

function prepare3partyForcasa() {
  #statements
  echo "step4-0:install homebrew's gcc to get gfortran for ast, casa"
  installgfortran

  echo "step4-5: install gsl from homebrew"
  installgsl

  ### Install the libraries for casa
  echo "step5: use Homebrew to Install some libraries for casa"
  pause
  # part1 homebrew part
  cd $cartawork
  sudo su $SUDO_USER -c "./CARTAvis/carta/scripts/installLibsForCASAonMac.sh"
  printDuration

  echo "step5-2: Build libsakura for casa"
  pause
  cd $cartawork/CARTAvis-externals/ThirdParty
  ## build libsakura-4.0.2065.
  git clone https://github.com/CARTAvis/libsakura
  curl -o gtest-1.7.0.zip -L https://github.com/google/googletest/archive/release-1.7.0.zip
  unzip gtest-1.7.0.zip -d libsakura > /dev/null
  cd libsakura
  ln -s googletest-release-1.7.0 gtest
  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_DOC=BOOL:OFF -DSIMD_ARCH=SSE4 \
  -DEIGEN3_INCLUDE_DIR=/usr/local/Cellar/eigen@3.2/3.2.10/include/eigen3 ..
  make
  make apidoc
  sudo make install #/usr/local
  cd ../../
  printDuration
}


echo "check everything before building casa"
cd $cartawork/CARTAvis-externals/ThirdParty
sudo su $SUDO_USER -c "brew list"
checkAllforCASA

# ### build casa
# #TODO: cache
echo "step6: Build casa"
checkcasacore=$cartawork/CARTAvis-externals/ThirdParty/casacore/include/casacore/casa/aips.h
checkcasacode=$cartawork/CARTAvis-externals/ThirdParty/imageanalysis/lib/libimageanalysis.dylib
if [ -e "$checkcasacore" ] && [ -e "$checkcasacode" ]
then
	echo "casacore and code built cache exist!!"
else
  echo "casacore and code built cache not exist, start to built it"
  pause
  rm -rf $cartawork/CARTAvis-externals/ThirdParty/casa
  rm -rf $cartawork/CARTAvis-externals/ThirdParty/casacore
  rm -rf $cartawork/CARTAvis-externals/ThirdParty/imageanalysis
  prepare3partyForcasa
  cd $cartawork
  sudo su $SUDO_USER -c "./CARTAvis/carta/scripts/buildcasa.sh"
  printDuration
  echo "check everything after building casa"
  ls $checkcasacore
  ls $checkcasacode
fi
###
