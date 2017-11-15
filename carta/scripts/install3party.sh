#!/bin/bash

if [ -z ${cartawork+x} ]; then
	export cartawork=`pwd`
fi

isCentOS=true
centos_release=/etc/os-release
if [ -f "$centos_release" ] && grep -q CentOS $centos_release  ; then
    echo "isCentOS"
else
    echo "should be Ubuntu or Mac"
		isCentOS=false
fi

if [ "$(uname)" == "Darwin" ]; then
	if [ "$TRAVIS" != true ] ; then
		echo "3party, not in travis"
		sudo easy_install cython
		sudo easy_install pip
		pip install matplotlib --user -U
	else
		echo "3party, in travis"
	fi
elif [ "$isCentOS" = true ] ; then
	##### CentOS 7

	sudo yum -y install epel-release ## which has cfitsio, leveldb, old wcslib

	## Default wcslib on CentOS 7 is 4.x. But it seems toe become 5.x, after using casa.repo.
	## wcslib.x86_64 5.15-2.el7.1 @casa
	# sudo yum -y install wcslib wcslib-devel
	# sudo yum -y install cfitsio cfitsio-devel
	# -> change to all use build from code, since yum/apt/mac ports have different version

	# sudo yum -y install gsl gsl-devel -> move to buildcasa.sh

	## these are required by carta, also are required by casa-submodules
	sudo yum -y install flex flex-devel bison bison-devel

	## required by carta
	sudo yum -y install Cython
	sudo yum install python-matplotlib

	## sqlite
	sudo yum -y install sqlite-devel

	## leveldb
	# leveldb needs epel-release, so install it first
	# (nrao-carta page does not mention but nrao-casa page mention)
	# second way: curl -O -L  https://dl.fedoraproject.org/pub/epel/RPM-GPG-KEY-EPEL-7
	# mv RPM-GPG-KEY-EPEL-7  /etc/pki/rpm-gpg/RPM-GPG-KEY-EPEL-7
	sudo yum -y install leveldb leveldb-devel
	sudo yum -y install gcc-gfortran # needed by ast, building casa also needes

	## needed by building gsl and casacore,
	## but seems already exist or no need ?
	sudo yum -y install blas blas-devel

	# for building Qt 4.8.5,
	# On Grimmer's docker -centos 7.3 , only when buidling Carta by Qt-5.3.2, we need gstreamer-devel, gstreamer-plugins-base
	# But on Chia-Jung Hsu's pc - centos 7.3, building Qt 4.8.5 needs pkgconfig(gstreamer-app-0.10) which install gstreamer-plugins-base-devel ,
	#    also gstreamer-devel, gstreamer-plugins-base,  gstreamer
	# Qt's QtWebkit may use gstreamer libs
	# We do not build Qt 4.8.5 on CentOS anymore, but building CARTA may need this, so move here
	sudo yum -y install 'pkgconfig(gstreamer-app-0.10)'
else
	##### Ubuntu 16.04
	# sudo apt-get -y install libcfitsio3-dev
	sudo apt-get -y install flex
	sudo apt-get -y install bison libbison-dev
	sudo apt-get -y install sqlite sqlite3 libsqlite3-dev
	sudo apt-get -y install libleveldb-dev
	sudo apt-get -y install cython ## will install python2.7, gcc

	sudo apt-get install python-matplotlib
	sudo apt-get -y install gfortran

	## also are needed by casacore
  sudo apt-get -y install libblas-dev libblas3

	## needed for compiling qwt
	apt-get install mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev

	## needed for building CARTA
	sudo apt-get -y install libgstreamer0.10-dev gstreamer0.10-plugins-base
fi

#####
mkdir -p $cartawork/CARTAvis-externals/ThirdParty

## for building qwt by qt5.3 for carta
if [ -z ${QT5PATH+x} ]; then
	echo "QT5PATH is unset";
	QT5PATH=/
	if [ "$(uname)" == "Darwin" ]; then
		QT5PATH=$HOME/Qt/5.3/clang_64
	else
		QT5PATH=/opt/Qt/5.3/gcc_64
	fi
	export PATH=$QT5PATH/bin:$PATH
else
	echo "QT5PATH is already set to '$QT5PATH'";
	export PATH=$QT5PATH/bin:$PATH # Homebrew case
	# export PATH=$QT5PATH/clang_64/bin:$PATH # Qt official installer case
	# export PATH=$QT5PATH/gcc_64/bin:$PATH # Qt official installer case
fi

cd $cartawork/CARTAvis-externals/ThirdParty
curl -O -L http://downloads.sourceforge.net/project/qwt/qwt/6.1.2/qwt-6.1.2.tar.bz2
tar xvfj qwt-6.1.2.tar.bz2 > /dev/null
mv qwt-6.1.2 qwt-6.1.2-src
cd qwt-6.1.2-src # can use qwt 6.1.2 Pavol uses
if [ "$(uname)" == "Darwin" ]; then
  perl -i -pe 's/.*/ QWT_INSTALL_PREFIX    = $ENV{cartawork}\/CARTAvis-externals\/ThirdParty\/qwt-6.1.2 / if $.==22' qwtconfig.pri
else
	# for unix part, not work on Mac
	sed -i "22,22c QWT_INSTALL_PREFIX    = $cartawork/CARTAvis-externals/ThirdParty/qwt-6.1.2" qwtconfig.pri
fi
qmake qwt.pro
make && make install
cd ..
ln -s qwt-6.1.2 qwt
if [ "$(uname)" == "Darwin" ]; then
	cd qwt
	# sometimes Versions/6 or Versions/1, have not investigated why
	ln -s ../qwt-6.1.2/lib/qwt.framework/Versions/Current/Headers include
	cd ..
fi

echo "install qooxdoo"
cd $cartawork/CARTAvis-externals/ThirdParty
## Install qooxdoo for CARTA
if [ -f qooxdoo-3.5.1-sdk.zip ]
then
	echo "alreayd downloaded qooxdoo-3.5.1-sdk.zip"
else
	curl -o qooxdoo-3.5.1-sdk.zip -L https://github.com/qooxdoo/qooxdoo/releases/download/release_3_5_1/qooxdoo-3.5.1-sdk.zip
fi
unzip qooxdoo-3.5.1-sdk.zip > /dev/null

## rapidjson
echo "build rapidjson"
cd $cartawork/CARTAvis-externals/ThirdParty
curl -O -L https://github.com/miloyip/rapidjson/archive/v1.0.2.tar.gz
mv v1.0.2.tar.gz rapidjson_v1.0.2.tar.gz
tar xvfz rapidjson_v1.0.2.tar.gz > /dev/null
ln -s rapidjson-1.0.2 rapidjson

## to get the same version with casa, so cfitsio, wcslib are built for carta, casa
## wcslib-5.15
echo "build wcslib"
cd $cartawork/CARTAvis-externals/ThirdParty
curl -O -L ftp://ftp.atnf.csiro.au/pub/software/wcslib/wcslib-5.15.tar.bz2
tar xvfj wcslib-5.15.tar.bz2 > /dev/null
mv wcslib-5.15 wcslib-5.15-src
cd wcslib-5.15-src
./configure --prefix=`pwd`/../wcslib/  ##--without-pgplot, pgplot is needed by casa
make && make install
cd ..

## cfitsio3390
echo "build cfitsio"
cd $cartawork/CARTAvis-externals/ThirdParty
curl -O -L http://heasarc.gsfc.nasa.gov/FTP/software/fitsio/c/cfitsio3390.tar.gz
tar xvfz cfitsio3390.tar.gz > /dev/null
mv cfitsio cfitsio-src
cd cfitsio-src
./configure --prefix=`pwd`/../cfitsio/
make && make install
cd ..

echo "build ast"
cd $cartawork/CARTAvis-externals/ThirdParty
## ast: carta only, static linking with CARTA
curl -O -L https://github.com/Starlink/ast/releases/download/v8.4.0/ast-8.4.0.tar.gz
tar xvfz ast-8.4.0.tar.gz > /dev/null
mv ast-8.4.0 ast-8.4.0-src
cd ast-8.4.0-src
./configure --prefix=`pwd`/../ast/
make && make install
cd ..

if [ "$(uname)" == "Darwin" ] && [ "$TRAVIS" == true ]; then
	echo "in travis-ci, not build gsl here, use homebrew to install it somewhere"
else
	echo "build gsl"
	cd $cartawork/CARTAvis-externals/ThirdParty
	# yum:1.15. so carta keeps building it from source code.
	# casa's cmake needs yum version to pass the check but it will use /usr/local in high priority when building
	curl -O -L http://ftp.gnu.org/gnu/gsl/gsl-2.3.tar.gz
	tar xvfz gsl-2.3.tar.gz > /dev/null
	mv gsl-2.3 gsl-2.3-src
	cd gsl-2.3-src
	./configure
	make
	sudo make install
	#cd ..
	cd $cartawork/CARTAvis-externals/ThirdParty
fi
