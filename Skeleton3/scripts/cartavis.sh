#!/bin/bash
####################################################################################################
# Title      :	cartavis.sh 
# Author     :	Alex Strilets, strilets@ualberta.ca
# Date       :	May 19, 2015
# Usage      :	cartavis.sh [fits/carta file to view] 
#               Examples:
#                   cartavis.sh - called with not paramters  will display sample 555wmos.fits file
#                   cartavis.sh /mypath/myfiletoview.fits - will display myfiletoview.fits
####################################################################################################

PN=`basename "$0"`			

usage () {
    echo >&2 "$PN - shell script to lunch CARTAvis viewer
usage: $PN [file to View ]

If no file name passed, default sample file 555wmos.fits will be displayed.

               Examples:
                   cartavis.sh - called with not paramters  will display sample 555wmos.fits file
                   cartavis.sh /mypath/myfiletoview.fits - will display myfiletoview.fits "
    exit 1
}

imagefile=$HOME/CARTA/Images/555wmos.fits
if [ ! -d $HOME/data/ephemerides -o ! -d $HOME/data/geodetic -o ! -d $HOME/CARTA/Images -o ! -f $imagefile -o ! -d $HOME/.cartavis  -o ! -f $HOME/.cartavis/config.json ]; then
	echo >&2 -e "Please run setupcatavis.sh script first to setup your carta environment\n"
	exit 1
fi


while [ $# -gt 0 ]
do
    case "$1" in
	--)	shift; break;;
	-h)	usage;;
	-*)	usage;;
	*) imagefile=$1;;			# image file name specified on the command line
    esac
    shift
done

appname=desktop
dirname=`dirname $0`

tmp="${dirname#?}"

if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
fi
LD_LIBRARY_PATH=$dirname/cpp/common
export LD_LIBRARY_PATH
cd $dirname/cpp/desktop

logfilename=$dirname/log/cartavis$(date +"%Y_%m_%d").log

./$appname --html $dirname/VFS/DesktopDevel/desktop/desktopIndex.html --scriptPort 9999 $imagefile >> $logfilename 2>&1 &
