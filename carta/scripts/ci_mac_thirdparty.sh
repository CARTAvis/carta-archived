#!/bin/bash

DIR=""
DIR=`dirname "$0"`; DIR=`eval "cd \"$DIR\" && pwd"`
export cartawork=$DIR/../../..
echo "CARTA working folder is $cartawork"
source $cartawork/CARTAvis/carta/scripts/ci_mac_common.sh

echo "step1-2: create ThirdParty folder"
su $SUDO_USER <<EOF
if [ -e "$cartawork/CARTAvis-externals/ThirdParty" ]; then
  echo "thirdparty exist";
else
  echo "third party does not exist";
  mkdir -p $cartawork/CARTAvis-externals/ThirdParty
fi
EOF

echo "step2: prerequisite: install or update  homebrew"
if [ ! -f "`which brew`" ] ; then
    # Install Homebrew
    echo "brew is not installd, install it "
    sudo -u $SUDO_USER ruby \
      -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)" \
      </dev/null
else
    echo "brew is installed"
fi
printDuration

function installflex() {
echo "step4-2: Use homebrew to Install some libs needed by flex for CARTA" #bison
pause
su $SUDO_USER <<EOF
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
echo "flex is in:"
echo `pwd`
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

}

echo "list Third party before building build something inside"
ls -l $cartawork/CARTAvis-externals/ThirdParty
echo "list ThirdParty end"

echo "step4: Install & build libraries for CARTA"

#TODO: cache
echo "step4-4: Build most of the Third party libs for CARTA"
if [ -e "$e_ast" ] && [ -e "$e_cfitsio" ] && [ -e "$e_qwt" ] && [ -e "$e_wcslib" ] && [ -e "$e_qooxdoo" ] && [ -e "$e_rapidjson" ] && [ -e "$e_flex" ]
then
	echo "built cache of qwt etc libs exit !!!!"
else
  echo "built cache of qwt etc libs not exit, start to build!!!!"
  pause
  echo "clean ThirdParty"
  rm -rf $cartawork/CARTAvis-externals/ThirdParty/*
  echo "step4-0:install homebrew's gcc to get gfortran for ast"
  installgfortran
  echo "step4-1: Install Qt for CARTA if you use default homebrew-qt"
  installqt
  cd $cartawork
  ## if use sudo ./xx.sh will let it can not inherit QT5PATH
  ./CARTAvis/carta/scripts/install3party.sh
  ###
  printDuration

  installflex
fi

echo "list Third party before after build something inside"
cd  $cartawork/CARTAvis-externals/ThirdParty
ls -al
echo "current third party is"
echo `pwd`
echo "size is"
du -sh .
echo "list ThirdParty end"
