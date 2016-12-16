ip=$(ifconfig en0 | grep inet | awk '$1=="inet" {print $2}')
xhost + $ip
docker run -ti --name cartaDesktop -w="/home/developer" -e DISPLAY=$ip:0 -v /tmp/.X11-unix:/tmp/.X11-unix -v $(pwd)/:/home/developer/src/CARTAvis vsuorant/cartabuild /bin/bash
