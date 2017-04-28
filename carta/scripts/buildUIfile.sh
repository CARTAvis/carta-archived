#!/bin/bash


## [depreciated], build UI files,
# please use github.com/cartavis/carta to get more build options for building UI

CARTAWORKHOME=`pwd`

cd $CARTAWORKHOME/CARTAvis/carta/html5/common/skel
./generate.py source-all
