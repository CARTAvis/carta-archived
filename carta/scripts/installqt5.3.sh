#!/bin/bash

QTINSTALLER=qt-unified-linux-x64-2.0.5-online.run
QTURL=http://ftp.jaist.ac.jp/pub/qtproject/archive/online_installers/2.0/$QTINSTALLER

isCentOS=true
centos_release=/etc/os-release
if [ -f "$centos_release" ] && grep -q CentOS $centos_release  ; then
    echo "isCentOS"
else
    echo "should be Ubuntu or Mac"
	isCentOS=false
fi
# QTINSTALLER=qt-opensource-linux-x64-5.8.0.run
# QTURL=http://download.qt.io/official_releases/qt/5.8/5.8.0/$QTINSTALLER
#       http://download.qt.io/archive/qt/5.3/5.3.2/qt-opensource-linux-x64-5.3.2.run

if [ "$isCentOS" = true ] ; then
    # this is needed by installing 5.3.2 from qt-opensource-linux-x64-5.3.2.run,
    # not sure if it is needed by online_installer
    sudo yum -y install mesa-libGL-devel ## may need for launching carta
else
    ## needed to launch qt creator installer
    sudo apt-get -y insatll libx11-xcb-dev libdbus-1-3
    ## needed to complete installing qt
    sudo apt-get -y install libgl1-mesa-glx libglib2.0-0
fi

wget $QTURL
chmod 755 $QTINSTALLER
./$QTINSTALLER ## will prompt UI to install, use default location to install
