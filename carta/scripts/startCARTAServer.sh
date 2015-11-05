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
wget https://googledrive.com/host/0B6trezaEcQQ9NXN0aGJPRk1XTGM/SeleniumTestImages.tgz
tar xzf SeleniumTestImages.tgz

cd /scratch
sudo mkdir -p snapshots
sudo chmod a+w snapshots
sudo mkdir -p ./snapshots/data
sudo chmod a+w ./snapshots/data
sudo mkdir -p ./snapshots/layout
sudo chmod a+w ./snapshots/layout
sudo mkdir -p ./snapshots/preferences
sudo chmod a+w ./snapshots/preferences

cd ~/cartaserver
./runserver.sh
