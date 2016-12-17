export HOME=/home/developer

ulimit -n 2048

export LD_LIBRARY_PATH=/opt/casa/lib/libcasa:/opt/casa/lib/libcode/:/opt/casa/otherlib/:/opt/wcslib-5.13/lib/
cd $HOME/src/CARTAvis/buildindocker/cpp/desktop
./desktop --html $HOME/src/CARTAvis/carta/VFS/DesktopDevel/desktop/desktopIndex.html --scriptPort 9999
