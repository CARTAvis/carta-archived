
#!/usr/bin/env bash

DIR=""
DISPLAYARG=""

if [ "$(uname)" == "Darwin" ]; then
    # Do something under Mac OS X platform
    ip=$(ifconfig en0 | grep inet | awk '$1=="inet" {print $2}')
    xhost + $ip
    # DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
    DISPLAYARG=${ip}":0"
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    # Do something under GNU/Linux platform
    xhost +
    # DIR=`dirname "$0"`; DIR=`eval "cd \"$DIR\" && pwd"`
    # $DISPLAY = :0
    DISPLAYARG=$DISPLAY
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]; then
    # Do something under Windows NT platform
    echo "This is windows platform"
fi

DIR=`dirname "$0"`; DIR=`eval "cd \"$DIR\" && pwd"`

echo $DIR
echo $DISPLAYARG

if [ $# -eq 0 ] ; then
    docker run --rm -u 1000 -p 8080:8080 -p 9999:9999 -ti --name carta \
    -w="/home/developer" -e DISPLAY=$DISPLAYARG \
    -e SHELL="/bin/bash" \
    -v /tmp/.X11-unix:/tmp/.X11-unix -v $DIR/../../:/home/developer/src/CARTAvis \
    grimmer0125/cartabuild:20161224 bash -c "/home/developer/src/CARTAvis/carta/scripts/dockerlaunchDesktop.sh"
else
    docker run --rm -u 1000 -p 8080:8080 -p 9999:9999 -ti --name carta \
    -w="/home/developer" -e DISPLAY=$DISPLAYARG \
    -e SHELL="/bin/bash" \
    -v /tmp/.X11-unix:/tmp/.X11-unix -v $DIR/../../:/home/developer/src/CARTAvis \
    -v $1:/home/developer/CARTA/Images \
    grimmer0125/cartabuild:20161224 bash -c "/home/developer/src/CARTAvis/carta/scripts/dockerlaunchDesktop.sh"
fi
