#!/bin/bash

ln -s ./src/build/ ./carta_latest

# copy server to local directory
scp ubuntu@199.116.235.163:/cartasrc/cartaserver.tgz .
tar xzf cartaserver.tgz
rm cartaserver.tgz

cd ~/carta_latest
mkdir -p VFS/PureWebDevel
cd VFS/PureWebDevel
ln -s /home/developer/src/CARTAvis/carta/html5/common ./common
ln -s /home/developer/src/CARTAvis/carta/html5/server ./server
ln -s /home/developer/src/CARTAvis/carta/html5/common/skel/source/resource/skel ./skel
scp ubuntu@199.116.235.163:/cartasrc/serverIndex.html .
sudo cp serverIndex.html ./server

cd /scratch/Images
scp ubuntu@199.116.235.163:/cartasrc/testimages/*.fits .

cd ~/cartaserver
./runserver.sh
