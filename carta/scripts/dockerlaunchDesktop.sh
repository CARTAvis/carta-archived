export HOME=/home/developer

ulimit -n 2048

# export LD_LIBRARY_PATH=/opt/casa/lib/libcasa:/opt/casa/lib/libcode/:/opt/casa/otherlib/:/opt/wcslib-5.13/lib/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/src/CARTAvis-externals/ThirdParty/casa/trunk/linux/lib

cd $HOME/src/CARTAvis/buildindocker/cpp/desktop
./CARTA --html $HOME/src/CARTAvis/carta/VFS/DesktopDevel/desktop/desktopIndex.html --scriptPort 9999
