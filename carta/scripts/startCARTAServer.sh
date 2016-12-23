#!/bin/bash

# $HOME/src/CARTAvis/buildindocker
ln -s ./src/build/ ./carta_latest

# copy server to local directory, should include pureweb and runserver.sh
scp ubuntu@carta-develop.ddns.net:/cartasrc/cartaserver.tgz .
tar xzf cartaserver.tgz
rm cartaserver.tgz

# pureweb look for "carta" which ls to ../carta_latest, which should have carta binary and html code
cd ~/carta_latest
mkdir -p VFS/PureWebDevel
cd VFS/PureWebDevel
ln -s /home/developer/src/CARTAvis/carta/html5/common ./common
ln -s /home/developer/src/CARTAvis/carta/html5/server ./server
ln -s /home/developer/src/CARTAvis/carta/html5/common/skel/source/resource/skel ./skel

# carta/VFS/PureWebDevel/server/serverIndex.html, should copy/ls from code,
# since even if ubuntu@carta-develop.ddns.net:/cartasrc/serverIndex.html exist, it uses fixed name inside
scp ubuntu@carta-develop.ddns.net:/cartasrc/serverIndex.html .
sudo cp serverIndex.html ./server

# need this
cd /scratch/Images
wget https://googledrive.com/host/0B6trezaEcQQ9NXN0aGJPRk1XTGM/SeleniumTestImages.tgz
tar xzf SeleniumTestImages.tgz

# seems not need for carta server version
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
