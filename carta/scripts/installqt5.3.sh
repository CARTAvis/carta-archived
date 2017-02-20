#!/bin/bash

## create working folder, $CARTAWORKHOME
# mkdir ~/src
# cd ~/src

CARTAWORKHOME=`pwd`
QTINSTALLER=qt-unified-linux-x64-2.0.5-online.run
QTURL=http://ftp.jaist.ac.jp/pub/qtproject/archive/online_installers/2.0/$QTINSTALLER

isCentOS=true
if grep -q CentOS /etc/os-release; then
    echo "isCentOS"
else
    echo "should be Ubuntu"
	isCentOS=false
    # QTINSTALLER=qt-opensource-linux-x64-5.8.0.run
    # QTURL=http://download.qt.io/official_releases/qt/5.8/5.8.0/$QTINSTALLER
    #       http://download.qt.io/archive/qt/5.3/5.3.2/qt-opensource-linux-x64-5.3.2.run
fi

if [ "$isCentOS" = true ] ; then
    # this is needed by installing 5.3.2 from qt-opensource-linux-x64-5.3.2.run,
    # not sure if it is needed by online_installer
    sudo yum -y install mesa-libGL-devel
else
    ## needed to launch qt creator installer
    sudo apt-get -y insatll libx11-xcb-dev libdbus-1-3
    ## needed to complete installing qt 
    sudo apt-get -y install libgl1-mesa-glx libglib2.0-0
fi

## Use latest qt online installer to install latest creator + qt 5.3.2 choosed
# Question: possible to use sudo yum to install qt 5.3.2 ? ref: qt5-qtbase-5.2.0-4.fc20.x86_64?

# mkdir -p ~/download
# cd ~/download
wget $QTURL
chmod 755 $QTINSTALLER
# http://doc.qt.io/qt-5/linux.html
# mkdir -p $CARTAWORKHOME/CARTAvis-externals/ThirdParty/Qt ## include qt creaetor
./$QTINSTALLER ## will prompt UI to install, but font is missing
# choose to install in  ~/src/CARTAvis-externals/ThirdParty/Qt5.3.2/,
# can change to use default location, ~/Qt/?
