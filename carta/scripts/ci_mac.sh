#!/bin/bash

function printDuration(){
  duration=$SECONDS
  echo "$(($duration / 60)) minutes and $(($duration % 60)) seconds elapsed."
}

function pause(){
  read -p "Press [Enter] key to continue"
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
  export cartawork=~/cartahome
else
  export cartawork=$1
fi

echo "CARTA working folder is $cartawork"

export QT5PATH=/usr/local/Cellar/qt/5.8.0_2
export CARTABUILDHOME=$cartawork/CARTAvis/build
export qtwebkit=qtwebkit-tp5-qt58-darwin-x64
branch=upgradeToNewNamespace #optional
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
echo "step2: prerequisite: install homebrew"
# /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
sudo -u $SUDO_USER ruby \
  -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)" \
  </dev/null
sudo su $SUDO_USER -c "brew install wget"
printDuration

if [ -f $cartawork/CARTAvis-externals/ThirdParty/macports/bin/flex ]; then
  echo "Macports-flex exist, so ignore macports part"
else
  echo "Macports-flex does not exist, so start to install macports and its flex, bison"

  echo "step2-2: prerequisite: install macports"
  pause

  # 3. install macports
  macporthome=$cartawork/CARTAvis-externals/ThirdParty/macports
  # mkdir -p $cartawork/CARTAvis-externals/ThirdParty
  cd $cartawork/CARTAvis-externals/ThirdParty
  curl -o MacPorts-2.4.1.tar.gz https://distfiles.macports.org/MacPorts/MacPorts-2.4.1.tar.gz
  tar zxf MacPorts-2.4.1.tar.gz
  cd MacPorts-2.4.1
  # ./configure --prefix=/opt/casa/02 --with-macports-user=root --with-applications-dir=/opt/casa/02/Applications
  ./configure --prefix=$macporthome
  make
  sudo make install
  #sudo /opt/casa/02/bin/port -v selfupdate
  sudo $macporthome/bin/port -v selfupdate
  printDuration

  echo "step2-3: install flex from macports"
  pause

  # these can be installed by ordinary Macports, too.
  sudo $macporthome/bin/port -N install flex  # 2.5.37, long time to install,
  # try this later https://github.com/apiaryio/homebrew/blob/master/Library/Formula/flex.rb
  # https://searchcode.com/codesearch/view/92040310/
  # homebrew official default :2.6.3
  printDuration

  echo "step2-4: install bison from macports"
  pause

  sudo $macporthome/bin/port -N install bison # 3.0.4
  ###
  printDuration

fi

### download CARTA soure code
echo "step3: download CARTA soure code"
pause

su $SUDO_USER <<EOF
cd $cartawork
git clone https://github.com/CARTAvis/carta.git CARTAvis
cd CARTAvis
git checkout $branch
EOF
###
printDuration

### Install 3 party for CARTA
echo "step4: Install Qt for CARTA"
pause

sudo su $SUDO_USER -c "brew install qt"
printDuration

echo "step4-2: Install Third party for CARTA"
pause
cd $cartawork
sudo ./CARTAvis/carta/scripts/install3party.sh
###
printDuration

### Install the libraries for casa
echo "step5: Install some libraries for casa from homebrew"
pause
# part1 homebrew part
sudo su $SUDO_USER -c "./CARTAvis/carta/scripts/installLibsForCASAonMac.sh"
printDuration

echo "step5-2: Install some libraries for casa, build from source-libsakura"
pause
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
sudo make install #/usr/local
cd ../../
echo "step5-3: Install some libraries for casa, build from source-rpfits"
pause
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
###
printDuration

### build casa
echo "step6: Build casa"
pause

cd $cartawork
sudo su $SUDO_USER -c "./CARTAvis/carta/scripts/buildcasa.sh"
###
printDuration

### setup QtWebkit
echo "step7: setup QtWebkit"
pause

su $SUDO_USER <<EOF
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
EOF
###
printDuration

### build UI of CARTA
su $SUDO_USER <<EOF
echo "step8: Build UI of CARTA"
pause
cd $cartawork/CARTAvis/carta/html5/common/skel
./generate.py
###
printDuration

### build CARTA
echo "step9: Build CARTA"
pause
export PATH=$QT5PATH/bin:$PATH
mkdir -p $CARTABUILDHOME
cd $CARTABUILDHOME
qmake -config release NOSERVER=1 CARTA_BUILD_TYPE=release $cartawork/CARTAvis/carta -r
make -j2
###
printDuration

echo "end time:"
date

### packagize CARTA
echo "step10: packagize CARTA"
pause
curl -O https://raw.githubusercontent.com/CARTAvis/deploytask/Qt5.8.0/final_mac_packaging_steps.sh
chmod 755 final_mac_packaging_steps.sh
./final_mac_packaging_steps.sh
EOF
