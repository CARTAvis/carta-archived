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

imagefile=$HOME/CARTA/Images/aH.fits

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

appname=CARTA
dirname=`dirname $0`

tmp="${dirname#?}"
if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
fi

logfilename=$HOME/.cartavis/log/$(date +"%Y%m%d_%H%M%S_%Z").log

if [ ! -d $HOME/CARTA/Images/CubesTest -o\
     ! -d $HOME/.cartavis/log  -o\
     ! -f $HOME/.cartavis/config.json -o\
     ! -d $HOME/CARTA/snapshots/data ]; then
	$dirname/setupcartavis.sh 2>&1  > /dev/null
fi

ulimit -n 2048

cd $dirname/cpp/desktop && ./$appname --html $dirname/VFS/DesktopDevel/desktop/desktopIndex.html --scriptPort 9999 $imagefile >> $logfilename 2>&1 &
