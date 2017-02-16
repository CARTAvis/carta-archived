#!/bin/bash

## working folder, $CARTAWORKHOME
CARTAWORKHOME=`pwd`

##################################
# Install some required RPM packages
##################################

## devtoolset will get gcc 4.8.5 etc
yum -y devtoolset*

## install 'make' tool
yum -y groupinstall "Development Tools"

yum -y install epel-release

## these are required by Carta, may also be required by casa too
yum -y install cfitsio-devel wcslib Cython flex-devel bison-devel

cd $CARTAWORKHOME/CARTAvis-externals/ThirdParty

## for building qwt for carta
export PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/Qt5.3.2/5.3/gcc_64/bin/:$PATH
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
# ./configure --prefix=${THIRDPARTY_PATH}/wcslib/ --without-pgplot <-pgplot ??
./configure --prefix=`pwd`/../wcslib/
make && make install
cd ..

curl -O -L http://heasarc.gsfc.nasa.gov/FTP/software/fitsio/c/cfitsio3380.tar.gz
tar xvfz cfitsio3380.tar.gz && mv cfitsio cfitsio-src
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
./configure --prefix=`pwd`/../gsl/
make && make install
cd ..

## sqlite
yum -y install sqlite-devel

## leveldb
## seems to need yum -y install epel-release first
yum -y install leveldb leveldb-devel
