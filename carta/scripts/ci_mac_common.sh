#!/bin/bash

### prerequisite
# 1. xcode
# 2. isntall homebrw

### download CARTA soure code
# echo "step3: download CARTA soure code" # no need to manually download for real ci.
# pause
# su $SUDO_USER <<EOF
# cd $cartawork
# git clone https://github.com/CARTAvis/carta.git CARTAvis
# cd CARTAvis
# git checkout $branch
# EOF
# ###
# printDuration

SECONDS=0
function printDuration(){
  duration=$SECONDS
  echo "$(($duration / 60)) minutes and $(($duration % 60)) seconds elapsed."
}

function pause() {
  echo "dummy pause"
}

echo "start time:"; date

echo "step1: define your variables first"

# DIR=""
# DIR=`dirname "$0"`; DIR=`eval "cd \"$DIR\" && pwd"`
# export cartawork=$DIR/../../..
# echo "CARTA working folder is $cartawork"

qt57brew=/usr/local/opt/qt\@5.7
export qt57brewrealpath=/usr/local/Cellar/qt@5.7/5.7.1
export QT5PATH=$qt57brew
export CARTABUILDHOME=$cartawork/CARTAvis/build
branch=upgradeToNewNamespace #optional
qtwebkit=qtwebkit-tp4-qt57-darwin
qtwebkitlink=https://github.com/annulen/webkit/releases/download/qtwebkit-tp4/qtwebkit-tp4-qt57-darwin.tar.xz
##

# ast, casa libs
function installgfortran() {

echo "install gfortran, start to backup travis-c++"
mv /usr/local/include/c++ /usr/local/include/c++_backup # this folder is from homebrew's gcc49
su $SUDO_USER <<EOF
## now it is 7.1 we only use its gfortran which is also used by code
if sw_vers -productVersion | grep 10.12 ; then
  echo "it is 10.12"
  brew install https://github.com/CARTAvis/homebrew-tap/releases/download/0.1.2/gcc-7.1.0.sierra.bottle.tar.gz
else
  echo "it is 10.11"
  brew install https://github.com/CARTAvis/homebrew-tap/releases/download/0.1.2/gcc-7.1.0.el_capitan.bottle.tar.gz
fi
EOF
echo "resume travis-c++"
mv /usr/local/include/c++_backup /usr/local/include/c++
printDuration

}

# qwt, carta
function installqt() {
  if [ "$QT5PATH" == "$qt57brew" ]; then
    echo "start to install homebrew-qt"
    pause
    if sw_vers -productVersion | grep 10.12 ; then
      echo "it is 10.12"
      sudo su $SUDO_USER -c "brew install https://github.com/CARTAvis/homebrew-tap/releases/download/0.1/qt.5.7-5.7.1.sierra.bottle.tar.gz"
    else
      echo "it is 10.11"
      sudo su $SUDO_USER -c "brew install https://github.com/CARTAvis/homebrew-tap/releases/download/0.1/qt.5.7-5.7.1.el_capitan.bottle.tar.gz"
    fi
    printDuration
  else
    echo "you use your own qt version"
  fi
}

function installgsl() {
su $SUDO_USER <<EOF
brew install gsl
brew link --overwrite gsl
EOF
}

e_ast=$cartawork/CARTAvis-externals/ThirdParty/ast/bin/ast_link
e_cfitsio=$cartawork/CARTAvis-externals/ThirdParty/cfitsio/lib/libcfitsio.a
e_qwt=$cartawork/CARTAvis-externals/ThirdParty/qwt-6.1.2/include/qwt.h
e_wcslib=$cartawork/CARTAvis-externals/ThirdParty/wcslib/lib/libwcs.5.15.dylib
e_qooxdoo=$cartawork/CARTAvis-externals/ThirdParty/qooxdoo-3.5.1-sdk.zip
e_rapidjson=$cartawork/CARTAvis-externals/ThirdParty/rapidjson
e_flex=$cartawork/CARTAvis-externals/ThirdParty/flex/bin/flex
# cd $cartawork/CARTAvis-externals/ThirdParty
# # yum:1.15. so carta keeps building it from source code.
# # casa's cmake needs yum version to pass the check but it will use /usr/local in high priority when building
# curl -O -L http://ftp.gnu.org/gnu/gsl/gsl-2.1.tar.gz
# tar xvfz gsl-2.1.tar.gz > /dev/null
# mv gsl-2.1 gsl-2.1-src
# cd gsl-2.1-src
# ./configure
# make
# sudo make install
# #cd ..
# cd $cartawork/CARTAvis-externals/ThirdParty

function checkAllforCASA() {
  echo "checkAllforCASA"
  #statements
  ls $e_cfitsio
  ls $e_wcslib
  ls /usr/local/lib/libgsl.a # can not cache
  ls /usr/local/lib/libsakura.4.0.dylib # can not cache
  ls /usr/local/bin/gfortran
}

function checkAllforCARTA() {
  echo "checkAllforCARTA"
  ls $e_ast
  ls $e_cfitsio
  ls $e_qwt
  ls $e_wcslib
  ls $e_qooxdoo
  ls $e_rapidjson
  ls $e_flex
  ls /usr/local/lib/libgsl.a # can not cache
  ls /usr/local/bin/gfortran
  which cython
  ls ~/Library/Python/2.7/lib/python/site-packages/matplotlib
}
