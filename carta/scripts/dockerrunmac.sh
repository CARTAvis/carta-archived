ip=$(ifconfig en0 | grep inet | awk '$1=="inet" {print $2}')
xhost + $ip

# http://stackoverflow.com/questions/59895/getting-the-current-present-working-directory-of-a-bash-script-from-within-the-s
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

docker run -p 9999:9999 -u 1000 -ti --name cartaDesktop -w="/home/developer" -e DISPLAY=$ip:0 -v /tmp/.X11-unix:/tmp/.X11-unix -v $DIR/../../:/home/developer/src/CARTAvis grimmer0125/cartabuild /bin/bash
