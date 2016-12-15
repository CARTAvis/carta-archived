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

mkdir $HOME/src/build
cd $HOME/src/build
qmake  CARTA_BUILD_TYPE=dev $HOME/src/CARTAvis/carta -r
try make
find . -name "*.o" -exec rm -f {} \;
find . -name "Makefile" -exec rm -f {} \;
find . -name "*.h" -exec rm -f {} \;
find . -name "*.cpp" -exec rm -f {} \;

# tar all buid into one file
cd $HOME/src
tar czf carta.latest.build.tgz ./build

cd $HOME/src/CARTAvis/carta
tar czf html5.tgz ./html5

if [ ! $CIRUN ]
then
	echo "This is NOT CI run will transffer files to CYBERA servere"
	# trasffer it to cybera server
	cd $HOME/src
	scp carta.latest.build.tgz ubuntu@carta-develop.ddns.net:/cartasrc/dockerbuild

	cd $HOME/src/CARTAvis/carta
	scp html5.tgz ubuntu@carta-develop.ddns.net:/cartasrc/carta_latest

	ssh ubuntu@carta-develop.ddns.net "cd /cartasrc/dockerbuild; rm -rf ./build; tar xzf carta.latest.build.tgz; rm carta.latest.build.tgz"
	ssh ubuntu@carta-develop.ddns.net "cd /cartasrc/carta_latest; rm -rf html5; tar xzf html5.tgz; rm html5.tgz"
	ssh ubuntu@carta-develop.ddns.net "/cartasrc/copy2carta.sh"
	ssh ubuntu@carta-develop.ddns.net "cd /cartasrc; rm -f carta_latest_ubuntu.tgz; tar czf carta_latest_ubuntu.tgz ./carta_latest;"
else
	echo "This is CI run will not transffer files to CYBERA servere"
fi
