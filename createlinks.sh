#!/bin/bash
export CARTAROOT=$HOME/dev
export GNULIB=/usr/lib/x86_64-linux-gnu

rmsymlink()
{
   [ -h ./$1 ] && rm ./$1
}

if [ ! -h ThirdParty ]
then
	echo "Can not find link to ThirdParty directory"
	exit 1
fi

cd ./ThirdParty

#
# qwt  installation location
# change this to poit to your installation diretory
#
rmsymlink qwt
mkdir -p qwt
export QWTLOCATION=/usr/local/qwt-6.1.2
rmsymlink qwt/include
rmsymlink qwt/lib
ln -s $QWTLOCATION/include qwt/include
ln -s $QWTLOCATION/lib qwt/lib

#
# qooxdoo installation location
# change this to poit to your installation diretory
#
export QOOXDOO35LOCATION=/home/jeff/dev/qooxdoo-3.5-sdk
rmsymlink qooxdoo-3.5-sdk 
ln -s ${QOOXDOO35LOCATION} qooxdoo-3.5-sdk

rmsymlink qooxdoo-sdk
ln -s ${QOOXDOO35LOCATION} qooxdoo-sdk

#
# libfistio  installation location
# do not change is unless you ibstalled it in your own directory
#
rmsymlink cfitsio-shared
mkdir -p cfitsio-shared
export LIBFITSIO=$GNULIB
rmsymlink cfitsio-shared/lib
rmsymlink cfitsio-shared/include
ln -s $LIBFITSIO cfitsio-shared/lib 
ln -s /usr/include  cfitsio-shared/include

#
# wcslib  installation location
# do not change is unless you ibstalled it in your own directory
#
rmsymlink wcslib-shared
mkdir -p wcslib-shared
export LIBWCS=/usr/include/wcslib-4.20
rmsymlink wcslib-shared/lib
rmsymlink wcslib-shared/include
ln -s $LIBWCS wcslib-shared/include
ln -s $GNULIB  wcslib-shared/lib

#
#rapidjson installation location
#
export RAPIDJSON=/usr/include/rapidjson
rmsymlink rapidjson
ln -s $RAPIDJSON rapidjson

# pureweb installation location
export PUREWEBSDK=$CARTAROOT/PureWeb/4.1.1/SDK
rmsymlink pureweb-4.1.1-sdk
ln -s $PUREWEBSDK pureweb-4.1.1-sdk

# CASACORE installation directories
rmsymlink  casacore-shared
mkdir -p casacore-shared
export CASACORE_1_7_0=/usr/local/include/casacore
rmsymlink  casacore-shared/include
rmsymlink  casacore-shared/lib
ln -s $CASACORE_1_7_0 ./casacore-shared/include 
ln -s /usr/local/lib ./casacore-shared/lib
