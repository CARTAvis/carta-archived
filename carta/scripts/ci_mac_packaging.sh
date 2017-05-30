#!/bin/bash

DIR=""
DIR=`dirname "$0"`; DIR=`eval "cd \"$DIR\" && pwd"`
export cartawork=$DIR/../../..
echo "CARTA working folder is $cartawork"
source $cartawork/CARTAvis/carta/scripts/ci_mac_common.sh

installforBuildCARTAandPackaging

### build UI of CARTA
echo "step8: Build UI of CARTA"
pause
su $SUDO_USER <<EOF
echo $cartawork
cd $cartawork/CARTAvis/carta/html5/common/skel
./generate.py
###
EOF
printDuration

### packagize CARTA
echo "step10: packagize CARTA"
pause
cd $CARTABUILDHOME
su $SUDO_USER <<EOF
curl -O https://raw.githubusercontent.com/CARTAvis/deploytask/Qt5.8.0/final_mac_packaging_steps.sh
chmod 755 final_mac_packaging_steps.sh
if [ "$TRAVIS_BUILD_NUMBER" != "" ] ; then
  export cartaversion=$TRAVIS_BUILD_NUMBER
fi
./final_mac_packaging_steps.sh
EOF
printDuration

echo "step11: reset folder permission to normal owner, not root"
chown -R $SUDO_USER:staff $cartawork

echo "list final result in build folder"
ls -al $CARTABUILDHOME
echo "end"

echo "end time:"
date
