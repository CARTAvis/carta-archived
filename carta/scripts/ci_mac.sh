#!/bin/bash

### TODO
## 1. use the following two to improve sudo part
## https://askubuntu.com/questions/585043/run-only-parts-of-a-script-as-sudo-entering-password-only-once
## or https://unix.stackexchange.com/questions/70859/why-doesnt-sudo-su-in-a-shell-script-run-the-rest-of-the-script-as-root
## 2. cache for gsl??? no, at least need build again !!! sakura and homebrew global part, hombrew can not be used with sudo
## x3. webkit preview 5 of qt 5.8 !!!!!

### define your variables first
export cartawork=~/src2
export QT5PATH=/usr/local/Cellar/qt/5.8.0_2
export CARTABUILDHOME=$cartawork/CARTAvis/build
qtwebkit=qtwebkit-tp5-qt58-darwin-x64
branch=upgradeToNewNamespace #optional
##

### prerequisite
# 1. xcode
# 2. isntall homebrw
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
brew install wget
# 3. install macports
mkdir -p $cartawork/CARTAvis-externals/ThirdParty
cd $cartawork/CARTAvis-externals/ThirdParty
curl -o MacPorts-2.4.1.tar.gz https://distfiles.macports.org/MacPorts/MacPorts-2.4.1.tar.gz
tar zxf MacPorts-2.4.1.tar.gz
cd MacPorts-2.4.1
./configure --prefix=/opt/casa/02 --with-macports-user=root --with-applications-dir=/opt/casa/02/Applications
make
sudo make install
sudo /opt/casa/02/bin/port -v selfupdate
##

### download CARTA soure code
mkdir -p $cartawork
cd $cartawork
git clone https://github.com/CARTAvis/carta.git CARTAvis
cd CARTAvis
git checkout $branch

brew install qt

### Install 3 party for CARTA
cd $cartawork
sudo ./CARTAvis/carta/scripts/install3party.sh
# these can be installed by ordinary Macports, too.
sudo /opt/casa/02/bin/port -N install flex  # 2.5.37
sudo /opt/casa/02/bin/port -N install bison # 3.0.4

### Install the libraries for casa
# part1 homebrew part
sudo su $SUDO_USER -c "./CARTAvis/carta/scripts/installLibsForCASAonMac.sh"
# part2
cd $cartawork/CARTAvis-externals/ThirdParty
## build libsakura-4.0.2065.
git clone https://github.com/grimmer0125/libsakura
wget -O gtest-1.7.0.zip https://github.com/google/googletest/archive/release-1.7.0.zip
unzip gtest-1.7.0.zip -d libsakura
cd libsakura
ln -s googletest-release-1.7.0 gtest
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_DOC=BOOL:OFF -DSIMD_ARCH=SSE4 \
-DEIGEN3_INCLUDE_DIR=/usr/local/Cellar/eigen@3.2/3.2.10/include/eigen3 ..
make
make apidoc
sudo make install
cd ../../
# in casa-code's cmake, its related parameter is -DLIBSAKURA_ROOT_DIR, but not need now since we install into /usr/local
# part3, Build rpfits-2.24, make sure you are still in `your-carta-work`/CARTAvis-externals/ThirdParty/
wget ftp://ftp.atnf.csiro.au/pub/software/rpfits/rpfits-2.24.tar.gz
tar xvfz rpfits-2.24.tar.gz && mv rpfits rpfits-src
mkdir -p rpfits/lib
mkdir -p rpfits/include
mkdir -p rpfits/bin
cd rpfits-src
export RPARCH="darwin"
export PATH=/usr/local/Cellar/gcc/6.3.0_1/bin:$PATH
make FC=gfortran-6 -f GNUmakefile
cp librpfits.a ../rpfits/lib/
cp rpfex rpfhdr ../rpfits/bin/
cp code/RPFITS.h ../rpfits/include/

### build casa
cd $cartawork
./CARTAvis/carta/scripts/buildcasa.sh

### setup QtWebkit
cd $cartawork/CARTAvis-externals/ThirdParty
wget https://github.com/annulen/webkit/releases/download/qtwebkit-tp5/$qtwebkit.tar.xz
tar -xvzf $qtwebkit.tar.xz
# export QT5PATH=/usr/local/Cellar/qt/5.8.0_2
# /Users/grimmer/Qt/5.7/clang_64
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

### build UI of CARTA
cd $cartawork/CARTAvis/carta/html5/common/skel
./generate.py

### build CARTA
export PATH=$QT5PATH/bin:$PATH
mkdir -p $CARTABUILDHOME
cd $CARTABUILDHOME
qmake -config release NOSERVER=1 CARTA_BUILD_TYPE=release $cartawork/CARTAvis/carta -r
make -j2

### packagize CARTA
curl -O https://raw.githubusercontent.com/CARTAvis/deploytask/Qt5.8.0/final_mac_packaging_steps.sh
chmod 755 final_mac_packaging_steps.sh && ./final_mac_packaging_steps.sh
