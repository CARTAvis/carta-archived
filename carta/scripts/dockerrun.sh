
#!/usr/bin/env bash

if [ "$(uname)" == "Darwin" ]; then
    # Do something under Mac OS X platform
    ip=$(ifconfig en0 | grep inet | awk '$1=="inet" {print $2}')
    xhost + $ip
    DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
    docker run --rm -u 1000 -p 9999:9999 -ti --name cartaDesktop -w="/home/developer" -e DISPLAY=$ip:0 -v /tmp/.X11-unix:/tmp/.X11-unix -v $DIR/../../:/home/developer/src/CARTAvis grimmer0125/cartabuild /bin/bash
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    # Do something under GNU/Linux platform
    xhost +
    DIR=`dirname "$0"`; DIR=`eval "cd \"$DIR\" && pwd"`
    # $DISPLAY = :0
    docker run --rm -u 1000 -p 9999:9999 -ti --name cartaDesktop -w="/home/developer" -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix -v $DIR/../../:/home/developer/src/CARTAvis grimmer0125/cartabuild /bin/bash
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]; then
    # Do something under Windows NT platform
    echo "This is windows platform"
fi
