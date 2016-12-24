#!/bin/bash

export HOME=/home/developer
export DISPLAY=:1
export CARTASRC=$HOME/src/CARTAvis

Xvfb :1 -extension GLX -screen 0 1024x780x24 &
ulimit -n 2048
sleep 10

#pytest.ini use /src/build too
export LD_LIBRARY_PATH=/opt/casa/lib/libcasa:/opt/casa/lib/libcode/:/opt/casa/otherlib/:/opt/wcslib-5.13/lib/
cd $HOME/src/build/cpp/desktop
# ./desktop --html $HOME/src/CARTAvis/carta/VFS/DesktopDevel/desktop/desktopIndex.html --scriptPort 9999 2>&1 &
cp /home/developer/src/CARTAvis/carta/scripts/pytest.ini /home/developer/src/CARTAvis/carta/scriptedClient/tests
cp /home/developer/src/CARTAvis/carta/scripts/runScriptTests.sh /home/developer/src/CARTAvis/carta/scriptedClient/tests
cd /home/developer/src/CARTAvis/carta/scriptedClient/tests
./runScriptTests.sh
