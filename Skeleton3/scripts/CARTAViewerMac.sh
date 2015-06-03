#!/bin/bash
####################################################################################################
# Title      :	cartavisOSX.sh 
# Author     :	Alex Strilets, strilets@ualberta.ca
# Date       :	May 21, 2015
# Description:  this is shell script to start carta visualization viewer on Mac OS X
####################################################################################################

PN=`basename "$0"`			



echo $PN >> $HOME/log.txt

appname=desktop
dirname=`dirname $0`

tmp="${dirname#?}"

if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
fi


logfilename=$HOME/.cartavis/log/$(date +"%Y_%m_%d").log
imagefile=$HOME/CARTA/Images/green.fits

if [ ! -d $HOME/CARTA/Images -o ! -f $imagefile -o ! -d $HOME/.cartavis -o ! -d $HOME/.cartavis/log  -o ! -f $HOME/.cartavis/config.json ]; then
	$dirname/setupcartavis.sh 2>&1  > /dev/null
fi

echo $dirname
cd $dirname/cpp/desktop/desktop.app/Contents/MacOS


./$appname --html $dirname/VFS/DesktopDevel/desktop/desktopIndex.html --scriptPort 9999 $imagefile >> $logfilename 2>&1 &
