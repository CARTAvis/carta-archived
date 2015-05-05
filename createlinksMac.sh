#!/bin/bash
export CARTAROOT=$HOME/src/CARTA
export GNUROOT=/usr/local

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
export QWT_INCLUIDE=/usr/local/qwt-6.1.3-svn/lib/qwt.framework/Versions/6/Headers/
export QWT_LIB=/usr/local/qwt-6.1.3-svn/lib

rmsymlink qwt/include
rmsymlink qwt/lib
ln -s $QWT_INCLUIDE qwt/include
ln -s $QWT_LIB  qwt/lib

#
# qooxdoo installation location
# change this to poit to your installation diretory
#
export QOOXDOO35LOCATION=$CARTAROOT/qooxdoo-3.5-sdk
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
export CFITSIO_INCLUDE=/usr/local/opt/cfitsio/include
export CFITSIO_LIB=/usr/local/opt/cfitsio/lib

rmsymlink cfitsio-shared/lib
rmsymlink cfitsio-shared/include
ln -s $CFITSIO_LIB  cfitsio-shared/lib 
ln -s $CFITSIO_INCLUDE cfitsio-shared/include

#
# wcslib  installation location
# do not change is unless you ibstalled it in your own directory
#
rmsymlink wcslib-shared
mkdir -p wcslib-shared
export LIBWCS_INCLUDE=/usr/local/opt/wcslib/include
export LIBWCS_LIB=/usr/local/opt/wcslib/lib

rmsymlink wcslib-shared/lib
rmsymlink wcslib-shared/include
ln -s $LIBWCS_LIB wcslib-shared/lib
ln -s $LIBWCS_INCLUDE wcslib-shared/include

#
#rapidjson installation location
#
export RAPIDJSON=$CARTAROOT/rapidjson
rmsymlink rapidjson
ln -s $RAPIDJSON rapidjson

# pureweb installation location
export PUREWEBSDK=$CARTAROOT/PureWeb/4.1.1/SDK
rmsymlink pureweb-4.1.1-sdk
ln -s $PUREWEBSDK pureweb-4.1.1-sdk

# CASACORE installation directories
rmsymlink  casacore-shared
mkdir -p casacore-shared

export CASACORE_INCLUDE=/usr/local/opt/casacore/include
export CASACORE_LIB=/usr/local/opt/casacore/lib
rmsymlink  casacore-shared/include
rmsymlink  casacore-shared/lib
ln -s $CASACORE_INCLUDE ./casacore-shared/include 
ln -s $CASACORE_LIB ./casacore-shared/lib
