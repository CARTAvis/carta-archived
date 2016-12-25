#!/bin/bash

yell() { echo "$0: $*" >&2; }
die() { yell "$*"; exit 111; }
try() { "$@" || die "cannot $*"; }

HOME=/home/developer
export PATH=$PATH:$HOME/Qt/5.3/gcc_64/bin

ln -s $HOME/src/CARTAvis-externals/ThirdParty/ast-8.0.2 $HOME/src/CARTAvis-externals/ThirdParty/ast
ln -s $HOME/src/CARTAvis-externals/ThirdParty/casacore-2.10.2016 $HOME/src/CARTAvis-externals/ThirdParty/casacore
ln -s $HOME/src/CARTAvis-externals/ThirdParty/cfitsio-shared $HOME/src/CARTAvis-externals/ThirdParty/cfitsio
ln -s $HOME/src/CARTAvis-externals/ThirdParty/imageanalysis-2.10.2016 $HOME/src/CARTAvis-externals/ThirdParty/imageanalysis

cp $HOME/src/CARTAvis/carta/scripts/runScriptedClientTests.sh $HOME
cp $HOME/src/CARTAvis/carta/scripts/startCARTAServer.sh $HOME

cd $HOME/src/CARTAvis
sudo chmod -R a+w carta

cd $HOME/src/CARTAvis/carta/html5/common/skel
./generate.py source > /dev/null
./generate.py  > /dev/null

mkdir $HOME/src/CARTAvis/buildindocker
cd $HOME/src/CARTAvis/buildindocker
qmake CARTA_BUILD_TYPE=dev $HOME/src/CARTAvis/carta -r

if [ $# -eq 0 ] ; then
    echo "No make arguments supplied"
    try make
else
    try make -j $1
fi

find . -name "*.o" -exec rm -f {} \;
find . -name "Makefile" -exec rm -f {} \;
find . -name "*.h" -exec rm -f {} \;
find . -name "*.cpp" -exec rm -f {} \;
