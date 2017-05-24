#!/bin/bash

function printDuration(){
  duration=$SECONDS
  echo "$(($duration / 60)) minutes and $(($duration % 60)) seconds elapsed."
}

function pause(){
  # afplay /System/Library/Sounds/Funk.aiff
  # read -p "Press [Enter] key to continue"
}

### TODO
## 1. use the following two to improve sudo part
## https://askubuntu.com/questions/585043/run-only-parts-of-a-script-as-sudo-entering-password-only-once
## or https://unix.stackexchange.com/questions/70859/why-doesnt-sudo-su-in-a-shell-script-run-the-rest-of-the-script-as-root
## 2. cache for gsl??? no, at least need build again !!! sakura and homebrew global part, hombrew can not be used with sudo
## x3. webkit preview 5 of qt 5.8 !!!!!
echo "start time:"
date
SECONDS=0

echo "step1: define your variables first"

# if [ -z ${cartawork+x} ]; then
#   export cartawork=~/cartahome
# fi

if [ $# -eq 0 ] ; then
  export cartawork=~/build/CARTAvis # for travis-ci
else
  export cartawork=$1
fi

echo "CARTA working folder is $cartawork"

qt57brew=/usr/local/opt/qt\@5.7
export qt57brewrealpath=/usr/local/Cellar/qt@5.7/5.7.1
export QT5PATH=$qt57brew
export CARTABUILDHOME=$cartawork/CARTAvis/build
branch=upgradeToNewNamespace #optional
qtwebkit=qtwebkit-tp4-qt57-darwin
qtwebkitlink=https://github.com/annulen/webkit/releases/download/qtwebkit-tp4/qtwebkit-tp4-qt57-darwin.tar.xz
##

echo "step1-2: create ThirdParty folder"
su $SUDO_USER <<EOF
mkdir -p $cartawork
mkdir -p $cartawork/CARTAvis-externals/ThirdParty
EOF

# su $SUDO_USER <<EOF
# /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
# brew install wget
# EOF

### prerequisite
# 1. xcode
# 2. isntall homebrw
echo "step2: prerequisite: install or update  homebrew"
if [ ! -f "`which brew`" ] ; then
    # Install Homebrew
    echo "brew is not installd, install it "
    sudo -u $SUDO_USER ruby \
      -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)" \
      </dev/null
else
    echo "brew is installed, update it"
    # sudo -u $SUDO_USER brew update
fi
printDuration

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

echo "step4-0:install homebrew's gcc to get gfortran for ast, also cmake for libsakura, casa libs"
if [ "$TRAVIS" = true ] ; then
  echo "backup travis-c++"
  mv /usr/local/include/c++ /usr/local/include/c++_backup
fi
su $SUDO_USER <<EOF
### Basic tools
if brew ls --versions cmake ; then
  echo "cmake is already installed"
else
  brew install cmake
  brew link --overwrite cmake
fi
## now it is 7.1 we only use its gfortran which is also used by code
brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/0ab90d39e3ca786c9f0b2fb44c2fd29880336cd2/Formula/gcc.rb
EOF
if [ "$TRAVIS" = true ] ; then
  echo "resume travis-c++"
  mv /usr/local/include/c++_backup /usr/local/include/c++
fi
printDuration

### Install 3 party for CARTA
echo "step4: Install & build libraries for CARTA"

echo "step4-1: Install Qt for CARTA if you use default homebrew-qt"
if [ "$QT5PATH" == "$qt57brew" ]; then
  echo "start to install homebrew-qt"
  pause
  sudo su $SUDO_USER -c "brew tap CARTAvis/tap"
  sudo su $SUDO_USER -c "brew install CARTAvis/tap/qt@5.7"
  printDuration
else
  echo "you use your own qt version"
fi

echo "step4-2: Use homebrew to Install some libs needed by flex and bison for CARTA"
pause
su $SUDO_USER <<EOF
brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/49887a8f215bd8b365c28c6ae5ea62bb1350c893/Formula/bison.rb
if brew ls --versions autoconf ; then
  echo "autoconf is already installed"
else
  brew install autoconf
fi
if brew ls --versions automake ; then
  echo "automake is already installed"
else
  brew install automake
fi
brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/056def4318cd874871b266c9f1df88e450411966/Formula/texinfo.rb
brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/512e7f6e9673a6f359c4c36d908126cb9c284f9f/Formula/help2man.rb
brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/d407fb8563f480391d918e1f1160cb7e244a1a12/Formula/gettext.rb
brew cask install basictex
EOF
printDuration

#TODO: cache
echo "step4-3: build flex"
cd $cartawork/CARTAvis-externals/ThirdParty
curl -O -L https://github.com/westes/flex/archive/flex-2.5.37.tar.gz
tar zxvf flex-2.5.37.tar.gz > /dev/null
mv flex-flex-2.5.37 flex-2.5.37
cd flex-2.5.37
export PATH=/usr/local/opt/gettext/bin:$PATH
export PATH=/usr/local/opt/texinfo/bin:$PATH
export PATH=/usr/local/Cellar/help2man/1.47.4/bin:$PATH
export PATH="$PATH:/Library/TeX/texbin"
./autogen.sh
./configure --disable-dependency-tracking  --prefix=`pwd`/../flex
make install
###
printDuration
echo "finish building flex"

e_ast=$cartawork/CARTAvis-externals/ThirdParty/ast/bin/ast_link
e_cfitsio=$cartawork/CARTAvis-externals/ThirdParty/cfitsio/lib/libcfitsio.a
e_qwt=$cartawork/CARTAvis-externals/ThirdParty/qwt-6.1.2/include/qwt.h
e_wcslib=$cartawork/CARTAvis-externals/ThirdParty/wcslib/lib/libwcs.5.15.dylib
e_qooxdoo=$cartawork/CARTAvis-externals/ThirdParty/qooxdoo-3.5.1-sdk
e_rapidjson=$cartawork/CARTAvis-externals/ThirdParty/rapidjson

#TODO: cache
echo "step4-4: Build most of the Third party libs for CARTA"
if [ -f "$e_ast" ] && [ -f "$e_cfitsio" ] && [ -f "$e_qwt" ] && [ -f "$e_wcslib" ] && [ -f "$e_qooxdoo" ] && [ -f "$e_rapidjson" ]
then
	echo "built cache of qwt etc libs exit !!!!"
else
  echo "built cache of qwt etc libs not exit, start to build!!!!"
  pause
  cd $cartawork
  ## if use sudo ./xx.sh will let it can not inherit QT5PATH
  ./CARTAvis/carta/scripts/install3party.sh
  ###
  printDuration
fi

## gsl
echo "step4-5: build gsl"
cd $cartawork/CARTAvis-externals/ThirdParty
# yum:1.15. so carta keeps building it from source code.
# casa's cmake needs yum version to pass the check but it will use /usr/local in high priority when building
curl -O -L http://ftp.gnu.org/gnu/gsl/gsl-2.1.tar.gz
tar xvfz gsl-2.1.tar.gz > /dev/null
mv gsl-2.1 gsl-2.1-src
cd gsl-2.1-src
./configure
make
sudo make install
#cd ..
cd $cartawork/CARTAvis-externals/ThirdParty

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
git clone https://github.com/grimmer0125/libsakura
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

echo "check everything before building casa"
cd $cartawork/CARTAvis-externals/ThirdParty
sudo su $SUDO_USER -c "brew list"
echo "list ThirdParty"
echo "list ThirdParty end"

ls $e_ast
ls $e_cfitsio
ls $e_qwt
ls $e_wcslib
ls $e_qooxdoo
ls $e_rapidjson
# ls ./ast/bin/ast_link
# ls ./cfitsio/lib/libcfitsio.a
# ls ./qwt-6.1.2/include/qwt.h
# ls ./wcslib/lib/libwcs.5.15.dylib
# ls ./qooxdoo-3.5.1-sdk
# ls ./rapidjson
ls /usr/local/lib/libgsl.a # can not cache
ls /usr/local/lib/libsakura.4.0.dylib # can not cache
ls /usr/local/bin/gfortran
which cython
ls ~/Library/Python/2.7/lib/python/site-packages/matplotlib
ls -l # print again

### build casa
#TODO: cache
echo "step6: Build casa"
checkcasacore=$cartawork/CARTAvis-externals/ThirdParty/casacore/include/casacore/casa/aips.h
checkcasacode=$cartawork/CARTAvis-externals/ThirdParty/imageanalysis/lib/libimageanalysis.dylib
if [ -f "$checkcasacore" ] && [ -f "$checkcasacode" ]
then
	echo "casacore and code built cache exist!!"
else
  echo "casacore and code built cache not exist, start to built it"
  pause
  cd $cartawork
  sudo su $SUDO_USER -c "./CARTAvis/carta/scripts/buildcasa.sh"
  printDuration
  echo "check everything after building casa"
  ls $checkcasacore
  ls $checkcasacode
fi
###

### setup QtWebkit
# echo "step7: setup QtWebkit"
# pause
# su $SUDO_USER <<EOF
# cd $cartawork/CARTAvis-externals/ThirdParty
# curl -O -L $qtwebkitlink
# tar -xvzf $qtwebkit.tar.xz > /dev/null
# # copy include
# cp -r $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/include/QtWebKit $QT5PATH/include/
# cp -r $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/include/QtWebKitWidgets $QT5PATH/include/
# # copy lib
# cp $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/lib/libqt* $QT5PATH/lib/
# cp -r $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/lib/QtWebKit.framework $QT5PATH/lib/
# cp -r $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/lib/QtWebKitWidgets.framework $QT5PATH/lib/
# cp -r $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/lib/cmake/* $QT5PATH/lib/cmake/
# cp $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/lib/pkgconfig/* $QT5PATH/lib/pkgconfig/
# # copy mkspecs
# cp $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/mkspecs/modules/* $QT5PATH/mkspecs/modules/
# echo "QT.webkit.module = QtWebKit" >> $QT5PATH/mkspecs/modules/qt_lib_webkit.pri
# perl -pi.bak -e 's/QT.webkit.module_config =/QT.webkit.module_config = v2 lib_bundle/g' $QT5PATH/mkspecs/modules/qt_lib_webkit.pri
# echo "QT.webkitwidgets.module = QtWebKitWidgets" >> $QT5PATH/mkspecs/modules/qt_lib_webkitwidgets.pri
# perl -pi.bak -e 's/QT.webkitwidgets.module_config =/QT.webkitwidgets.module_config = v2 lib_bundle /g' $QT5PATH/mkspecs/modules/qt_lib_webkitwidgets.pri
# EOF
# ###
# printDuration
#
# ### build UI of CARTA
# echo "step8: Build UI of CARTA"
# pause
# su $SUDO_USER <<EOF
# echo $cartawork
# cd $cartawork/CARTAvis/carta/html5/common/skel
# ./generate.py
# ###
# EOF
# printDuration
#
# ### build CARTA
# echo "step9: Build CARTA"
# pause
# su $SUDO_USER <<EOF
# echo $QT5PATH
# export PATH=$QT5PATH/bin:$PATH
# mkdir -p $CARTABUILDHOME
# cd $CARTABUILDHOME
# qmake -config release NOSERVER=1 CARTA_BUILD_TYPE=release $cartawork/CARTAvis/carta -r
# make -j2
# ###
# EOF
# printDuration
#
# echo "end time:"
# date
#
# ### packagize CARTA
# echo "step10: packagize CARTA"
# pause
# su $SUDO_USER <<EOF
# curl -O https://raw.githubusercontent.com/CARTAvis/deploytask/Qt5.8.0/final_mac_packaging_steps.sh
# chmod 755 final_mac_packaging_steps.sh
# ./final_mac_packaging_steps.sh
# EOF
#
# echo "step11: reset folder permission to normal owner, not root"
# chown -R $SUDO_USER:staff $cartawork
#
# echo "list final result in build folder"
# ls -al $CARTABUILDHOME
# echo "end"
