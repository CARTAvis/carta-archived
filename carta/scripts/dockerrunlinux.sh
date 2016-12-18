# http://stackoverflow.com/questions/59895/getting-the-current-present-working-directory-of-a-bash-script-from-within-the-s
xhost +
DIR=`dirname "$0"`; DIR=`eval "cd \"$DIR\" && pwd"`

# $DISPLAY = :0
docker run --rm -u 1000 -p 9999:9999 -ti --name cartaDesktop -w="/home/developer" -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix -v $DIR/../../:/home/developer/src/CARTAvis grimmer0125/cartabuild:v2 /bin/bash

# not sure which factors let linux ok: xhost, QT_GRAPHICSSYSTEM="native", -u 1000, new docker image 
