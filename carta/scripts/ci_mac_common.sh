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
export CARTABUILDHOME=$cartawork/build
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
# brew install https://github.com/CARTAvis/homebrew-tap/releases/download/0.1.3/gsl-2.3.el_capitan.bottle.tar.gz
# brew link --overwrite gsl
curl -O -L http://ftp.gnu.org/gnu/gsl/gsl-2.3.tar.gz
tar xvfz gsl-2.3.tar.gz > /dev/null
mv gsl-2.3 gsl-2.3-src
cd gsl-2.3-src
./configure
make
sudo make install
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

  ls $cartawork/CARTAvis-externals/ThirdParty/casacore/include/casacore/casa/aips.h
  ls $cartawork/CARTAvis-externals/ThirdParty/imageanalysis/lib/libimageanalysis.dylib
  ls $cartawork/CARTAvis-externals/ThirdParty/casa/trunk/darwin/lib/libcasa_scimath*

  echo "print fftw in /usr/local/opt"
  ls /usr/local/opt/fftw/lib/*
}

function installforBuildCARTAandPackaging() {

sudo easy_install cython
sudo easy_install pip
sudo su $SUDO_USER -c "pip install matplotlib --user -U"

  #statements
su $SUDO_USER <<EOF
brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/d407fb8563f480391d918e1f1160cb7e244a1a12/Formula/gettext.rb
EOF

echo "installforBuildCARTAandPackaging"

#qt
echo "step4-1: Install Qt for CARTA if you use default homebrew-qt"
installqt

# gfortran
echo "step4-0:install homebrew's gcc to get gfortran for ast, casa libs"
installgfortran

#gsl
echo "step4-5: install gsl from homebrew"
installgsl

#bison, fftw, boost-python(<-not sure if we need )
su $SUDO_USER <<EOF
brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/49887a8f215bd8b365c28c6ae5ea62bb1350c893/Formula/bison.rb
brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/cb79fc2ec2c5fcebf35f7bf26bb7459c9f87ae0b/Formula/fftw.rb
brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/198f8903db0bb77f1b84e19e020c6f825210433d/Formula/boost-python.rb
EOF

### setup QtWebkit
echo "step7: setup QtWebkit"
pause
su $SUDO_USER <<EOF
cd $cartawork/CARTAvis-externals/ThirdParty
curl -O -L $qtwebkitlink
tar -xvzf $qtwebkit.tar.xz > /dev/null
# copy include
cp -r $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/include/QtWebKit $QT5PATH/include/
cp -r $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/include/QtWebKitWidgets $QT5PATH/include/
# copy lib
cp $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/lib/libqt* $QT5PATH/lib/
cp -r $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/lib/QtWebKit.framework $QT5PATH/lib/
cp -r $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/lib/QtWebKitWidgets.framework $QT5PATH/lib/
cp -r $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/lib/cmake/* $QT5PATH/lib/cmake/
cp $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/lib/pkgconfig/* $QT5PATH/lib/pkgconfig/
# copy mkspecs
cp $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/mkspecs/modules/* $QT5PATH/mkspecs/modules/
echo "QT.webkit.module = QtWebKit" >> $QT5PATH/mkspecs/modules/qt_lib_webkit.pri
perl -pi.bak -e 's/QT.webkit.module_config =/QT.webkit.module_config = v2 lib_bundle/g' $QT5PATH/mkspecs/modules/qt_lib_webkit.pri
echo "QT.webkitwidgets.module = QtWebKitWidgets" >> $QT5PATH/mkspecs/modules/qt_lib_webkitwidgets.pri
perl -pi.bak -e 's/QT.webkitwidgets.module_config =/QT.webkitwidgets.module_config = v2 lib_bundle /g' $QT5PATH/mkspecs/modules/qt_lib_webkitwidgets.pri
EOF
###
printDuration

}
