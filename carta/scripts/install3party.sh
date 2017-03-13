#!/bin/bash

## working folder, $CARTAWORKHOME
CARTAWORKHOME=`pwd`

##################################
# Install some required packages
##################################

## Requirement of carta:
## g++ >= 4.8.1
## cfitsio >= 3360
## wcslib >= 4.23
## gsl >= 2.1
## ast >= 8.0.2

#####
# How to solve portable issues for dynamic libs, e.g. move carta binary or even packaing? Use rpath??
####

isCentOS=true
if grep -q CentOS /etc/os-release; then
    echo "isCentOS"
else
    echo "should be Ubuntu"
		isCentOS=false
fi

if [ "$isCentOS" = true ] ; then
	##### CentOS 7

	## these (wcslib, cfitsio) are required by Carta (but also duplicate install from source),
	## also are required by casa-submodules (at least casacore)
	## Todo [Giveup] !!: carta switchs to use yum version which are 3370, 5.15.
	## carta uses ThirdParty to look for cfitsio, wcslio,
	## so to use yum version, switch to /usr/lib, /usr/include etc? <-on Linux, default search path
	sudo yum -y install epel-release ## which has cfitsio, leveldb, old wcslib
	## to get the same version, so that cfitsio, wcslib are built for carta, casa

	## move to buildcasa
	# sudo yum -y install wcslib wcslib-devel

	# sudo yum -y install cfitsio cfitsio-devel
	# -> change to all use build from code, since yum/apt/mac ports have different version

	## Default wcslib on CentOS 7 is 4.x. But it seems that, after using casa.repo.
	## wcslib.x86_64 5.15-2.el7.1 @casa

	## required by carta, casa-submodue:code
	# yum:1.15. so carta keeps building it from source code.
	# Todo [Done]: fix header/lib/dynamic path in Qt, on Mac, CentOS, for Carta.
	# carta uses /usr/local to look for gsl.
	# Maybe just build from source and install there, let carta and casa use
	# sudo yum -y install gsl gsl-devel -> move to buildcasa.sh

	## these are required by carta, also are required by casa-submodules
	sudo yum -y install flex flex-devel bison bison-devel

	## required by carta
	sudo yum -y install Cython

	## carta only:
	## ast (static linking)

	## sqlite
	sudo yum -y install sqlite-devel

	## leveldb
	# leveldb needs epel-release, so install it first
	# (nrao-carta does not mention but nrao-casa mention)
	# second way: curl -O -L  https://dl.fedoraproject.org/pub/epel/RPM-GPG-KEY-EPEL-7
	# mv RPM-GPG-KEY-EPEL-7  /etc/pki/rpm-gpg/RPM-GPG-KEY-EPEL-7
	sudo yum -y install leveldb leveldb-devel
	sudo yum -y install gcc-gfortran # needed by ast, building casa also needes

	## needed by building gsl and casacore,
	## but seems already exist or no need ?
	sudo yum -y install blas blas-devel
else
	##### Ubuntu 16.04
	# sudo apt-get -y install libcfitsio3-dev
	sudo apt-get -y install flex
	sudo apt-get -y install bison libbison-dev
	sudo apt-get -y install sqlite sqlite3 libsqlite3-dev
	sudo apt-get -y install libleveldb-dev
	sudo apt-get -y install cython ## will install python2.7, gcc
	sudo apt-get -y install gfortran

	## also are needed by casacore
  sudo apt-get -y install libblas-dev libblas3

	## needed for compiling qwt
	apt-get install mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev
fi

#####

mkdir -p $CARTAWORKHOME/CARTAvis-externals/ThirdParty
cd $CARTAWORKHOME/CARTAvis-externals/ThirdParty

## for building qwt by qt5.3 for carta
if [ -z ${QT5PATH+x} ]; then
	echo "QT5PATH is unset";
	QT5PATH=/opt/Qt/5.3/gcc_64/bin/
	export PATH=$QT5PATH:$PATH
else
	echo "QT5PATH is already set to '$QT5PATH'";
	export PATH=$QT5PATH:$PATH
fi

curl -O -L http://downloads.sourceforge.net/project/qwt/qwt/6.1.2/qwt-6.1.2.tar.bz2
tar xvfj qwt-6.1.2.tar.bz2 && mv qwt-6.1.2 qwt-6.1.2-src
cd qwt-6.1.2-src # can use qwt 6.1.3 Pavol uses
# for unix part
sed -i "22,22c QWT_INSTALL_PREFIX    = $CARTAWORKHOME/CARTAvis-externals/ThirdParty/qwt-6.1.2" qwtconfig.pri
qmake qwt.pro
make && make install
cd ..
ln -s qwt-6.1.2 qwt

## Install qooxdoo for CARTA
wget 'http://downloads.sourceforge.net/project/qooxdoo/qooxdoo-current/3.5/qooxdoo-3.5-sdk.zip?r=https%3A%2F%2Fsourceforge.net%2Fprojects%2Fqooxdoo%2Ffiles%2Fqooxdoo-current%2F3.5%2F&ts=1479095500&use_mirror=excellmedia' -O qooxdoo-3.5-sdk.zip
unzip qooxdoo-3.5-sdk.zip

## rapidjson
curl -O -L https://github.com/miloyip/rapidjson/archive/v1.0.2.tar.gz
tar xvfz v1.0.2.tar.gz
ln -s rapidjson-1.0.2 rapidjson # can become mv

## wcslib-5.15
wget ftp://ftp.atnf.csiro.au/pub/software/wcslib/wcslib-5.15.tar.bz2
tar xvfj wcslib-5.15.tar.bz2 && mv wcslib-5.15 wcslib-5.15-src
cd wcslib-5.15-src
# ./configure --prefix=${THIRDPARTY_PATH}/wcslib/ --without-pgplot <-pgplot ?? <-add back later
./configure --prefix=`pwd`/../wcslib/  ##--without-pgplot which is needed by casa
make && make install
cd ..

## cfitsio3390
curl -O -L http://heasarc.gsfc.nasa.gov/FTP/software/fitsio/c/cfitsio3390.tar.gz
tar xvfz cfitsio3390.tar.gz && mv cfitsio cfitsio-src
cd cfitsio-src
./configure --prefix=`pwd`/../cfitsio/
make && make install
cd ..

## ast
wget http://www.starlink.ac.uk/download/ast/ast-8.0.2.tar.gz
tar xvfz ast-8.0.2.tar.gz && mv ast-8.0.2 ast-8.0.2-src
cd ast-8.0.2-src
./configure --prefix=`pwd`/../ast/
make && make install
cd ..

## gsl
curl -O -L http://ftp.gnu.org/gnu/gsl/gsl-2.1.tar.gz
tar xvfz gsl-2.1.tar.gz && mv gsl-2.1 gsl-2.1-src
cd gsl-2.1-src
./configure
make && make install
cd ..
