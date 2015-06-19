#!/bin/bash
export CARTAROOT=$HOME/src/CARTA
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
export QWTLOCATION=/usr/local/qwt-6.1.3-svn
rmsymlink qwt/include
rmsymlink qwt/lib
ln -s $QWTLOCATION/include qwt/include
ln -s $QWTLOCATION/lib qwt/lib

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
mkdir -p cfitsio-shared
rmsymlink cfitsio-3360-shared
ln -s ./cfitsio-shared ./cfitsio-3360-shared
export LIBFITSIO=$GNULIB
rmsymlink cfitsio-shared/lib
rmsymlink cfitsio-shared/include
ln -s $LIBFITSIO cfitsio-shared/lib 
ln -s /usr/include  cfitsio-shared/include

#
# wcslib  installation location
# do not change is unless you ibstalled it in your own directory
#
mkdir -p wcslib-shared
rmsymlink wcslib-4.23-shared
ln -s wcslib-shared ./wcslib-4.23-shared
export LIBWCS=/usr/include/wcslib-4.20
rmsymlink wcslib-shared/lib
rmsymlink wcslib-shared/include
ln -s $LIBWCS wcslib-shared/include
ln -s $GNULIB  wcslib-shared/lib

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
mkdir -p casacore-shared
rmsymlink casacore-2.0.1-shared
ln -s casacore-shared ./casacore-2.0.1-shared
export CASACORE=$HOME/casacore-2.0.1
rmsymlink  casacore-shared/include
rmsymlink  casacore-shared/lib
ln -s $CASACORE/include ./casacore-shared/include 
ln -s $CASACORE/lib ./casacore-shared/lib

# CASACORE installation directories
mkdir -p ast
rmsymlink ast-8.0.2
ln -s ast ./ast-8.0.2
export AST=$HOME/ast-8.0.4
rmsymlink  ast/include
rmsymlink  ast/lib
ln -s $AST/include ./ast/include 
ln -s $AST/lib ./ast/lib
