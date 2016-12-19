#!/bin/sh

# figure out the base directory (where this script is)
# PRG=$(readlink -f "$0")
# BASEDIR=`dirname "$PRG"`

BASEDIR=/home/developer/src/PureWeb/4.1.1

echo "I think the script is in " ${BASEDIR}

export PUREWEB_LIBS=${BASEDIR}/SDK/Redistributable/Libs
export PUREWEB_HOME=${BASEDIR}/Server
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PUREWEB_LIBS}/C++/lib
# export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/calsci/Qt53/5.3/gcc_64/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/developer/Qt/5.3/gcc_64/lib

# export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/calsci/Software/qwt-6.1.2-qt-5.3.2/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/developer/src/CARTAvis-externals/ThirdParty/qwt/lib

# copy from the script to launch desktop version,
ulimit -n 2048
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/casa/lib/libcasa:/opt/casa/lib/libcode/:/opt/casa/otherlib/:/opt/wcslib-5.13/lib/

# alex's , but ville's does not have
# export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$cartabuildfolder/cpp/core \
# :$cartabuildfolder/cpp/CartaLib

export JAVA_HOME=/usr/lib/jvm/java-7-openjdk-amd64

Xvfb :0.0 -screen 0 1600x1200x24+32 &
sleep 5

cp /home/developer/src/CARTAvis/PureWeb*.lic /home/developer/src/PureWeb/4.1.1/Server/conf/

cd ${PUREWEB_HOME}/tomcat/bin
./catalina.sh run
