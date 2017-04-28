#!/bin/bash
####################################################################################################
# Title      :	cartavisOSX.sh
# Author     :	Alex Strilets, strilets@ualberta.ca
# Date       :	May 21, 2015
# Description:  this is shell script to start carta visualization viewer on Mac OS X
####################################################################################################

PN=`basename "$0"`

unset DYLD_LIBRARY_PATH
unset DYLD_FALLBACK_LIBRARY_PATH

appname=CARTA
dirname=`dirname $0`

tmp="${dirname#?}"

if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
fi

logfilename=$HOME/.cartavis/log/$(date +"%Y_%m_%d").log
imagefile=$HOME/CARTA/Images/aH.fits

if [ ! -d $HOME/CARTA/Images/CubesTest -o\
     ! -d $HOME/.cartavis/log  -o\
     ! -f $HOME/.cartavis/config.json -o\
     ! -d $HOME/CARTA/snapshots/data ]; then
	$dirname/setupcartavis.sh 2>&1  > /dev/null
fi

cd $dirname/cpp/desktop/desktop.app/Contents/MacOS
./$appname --html $dirname/VFS/DesktopDevel/desktop/desktopIndex.html --scriptPort 9999 $imagefile >> $logfilename 2>&1 &
