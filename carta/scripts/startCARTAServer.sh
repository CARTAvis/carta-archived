#!/bin/bash

ln -s ./src/build/ ./carta_latest

# copy server to local directory
scp ubuntu@carta-develop.ddns.net:/cartasrc/cartaserver.tgz .
tar xzf cartaserver.tgz
rm cartaserver.tgz

cd ~/carta_latest
mkdir -p VFS/PureWebDevel
cd VFS/PureWebDevel
ln -s /home/developer/src/CARTAvis/carta/html5/common ./common
ln -s /home/developer/src/CARTAvis/carta/html5/server ./server
ln -s /home/developer/src/CARTAvis/carta/html5/common/skel/source/resource/skel ./skel
scp ubuntu@carta-develop.ddns.net:/cartasrc/serverIndex.html .
sudo cp serverIndex.html ./server

cd /scratch/Images
tar xzf images.tgz

mikdir -p /scratch/snapshots
mikdir -p /scratch/snapshots/data
mikdir -p /scratch/snapshots/layout
mikdir -p /scratch/snapshots/preferences

cd ~/cartaserver
./runserver.sh
