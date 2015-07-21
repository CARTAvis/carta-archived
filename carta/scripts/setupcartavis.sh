#!/bin/bash
####################################################################################################
# Title       :	setupcartavis.sh
# Author      :	Alex Strilets, strilets@ualberta.ca
# Date        :	May 19, 2015
# Descritpion :	this is shell script to setup catsvis envirotnemt before running cartavis.sh 
####################################################################################################
PN=`basename "$0"`

usage () {
    echo >&2 -e "$PN - shell script setup environment to run CARTAvis viewer\n"
    exit 1
}

if [ $# -gt 0 ]; then
    usage
fi

dirname=`dirname $0`
tmp="${dirname#?}"

if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
fi

#  check for existance of $HOME/.cartavis directory
if [ ! -d $HOME/.cartavis ]; then
	echo "creating $HOME/.cartavis directory..."
	mkdir $HOME/.cartavis
fi

#  check for existance of $HOME/.cartavis/log directory
if [ ! -d $HOME/.cartavis/log ]; then
	echo "creating $HOME/.cartavis/log directory..."
	mkdir $HOME/.cartavis/log
fi

# check that config.json exists
if [ ! -f $HOME/.cartavis/config.json ]; then
	echo "copying config.json file to  $HOME/.cartavis directory..."
	cp $dirname/config/config.json $HOME/.cartavis
fi

# check that $HOME/CARTA directory exists
if [ ! -d $HOME/CARTA ]; then
	echo "creating $HOME/CARTA directory..."
	mkdir $HOME/CARTA
fi

# check that $HOME/CARTA directory exists
if [ ! -d $HOME/CARTA ]; then
	echo "creating $HOME/CARTA directory..."
	mkdir $HOME/CARTA
fi

# check that $HOME/CARTA/Images directory exists
if [ ! -d $HOME/CARTA/Images ]; then
	echo "creating $HOME/CARTA/Images directory..."
	mkdir $HOME/CARTA/Images
fi

# check that $HOME/CARTA/snapshots directory exists
if [ ! -d $HOME/CARTA/snapshots ]; then
	echo "creating $HOME/CARTA/snapshots directory..."
	mkdir $HOME/CARTA/snapshots
fi

# check that sample files exists
if [ ! -f $HOME/CARTA/Images/555wmos.fits ]; then
	echo "copying sample images to $HOME/CARTA/Images directory ..."
	cp $dirname/images/*.fits $HOME/CARTA/Images
fi
