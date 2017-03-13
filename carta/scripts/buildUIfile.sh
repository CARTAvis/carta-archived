#!/bin/bash

##  modify common_config.pri
#gcc481 {
        #    COMPILER = g++-4.8

## build UI files
CARTAWORKHOME=`pwd`

cd $CARTAWORKHOME/CARTAvis/carta/html5/common/skel
./generate.py

## to build carta, Qt (webkit) needs gstreamer libs
## python-devel will install Python.h needed by building carta.
# sudo yum -y install gstreamer-devel
# sudo yum -y install gstreamer-plugins-base
# sudo yum -y install python-devel

## setupt geodetic, ephemerides

##  copy fits file
# mkdir -p ~/CARTA/Images,
# mkdir -p /scratch/Images
# cp aJ.fits /scratch/Images/
# cp aJ.fits ~/CARTA/Images/
##

# setup ~/.cartavis/config.json/config.json
# {
# "_comment" : "List of plugin directories",
# "pluginDirs": [
# "$(APPDIR)/../plugins",
# "$(APPDIR)/../../../../plugins",
# "$(HOME)/.cartavis/plugins"
# ],
# "disabledPlugins" : ["casaCore-2.0.1"],
# "plugins": {
#     "PCacheSqlite3" : {
#         "dbPath": "/tmp/pcache.sqlite"
#     }
# }

# }

## before running
#export LD_LIBRARY_PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/casa/trunk/linux/lib:${LD_LIBRARY_PATH}

## when running carta, loading ImageStatistics fail
# "Loading plugin RegionCASA[c++]"
# Trying to load native plugin "RegionCASA"
#   - heuristics to load libraries: 0
# Raw plugin loaded.
# Carta plugin loaded.
# Calling plugin's initialize
# Calling plugin's getInitialHookList
# Plugin initialized
# "Loading plugin ImageStatistics[c++]"
# Trying to load native plugin "ImageStatistics"
#   - heuristics to load libraries: 0
# QPluginLoader error =  "Cannot load library /root/src/CARTAvis/build/cpp/plugins/ImageStatistics/libplugin.so: (libplugin.so: cannot open shared object file: No such file or directory)"
# QLibrary error: "Cannot load library /root/src/CARTAvis/build/cpp/plugins/ImageStatistics/libplugin.so: (libplugin.so: cannot open shared object file: No such file or directory)"
# "Failed to load plugin ImageStatistics[c++]"
# ...reasons:  "QPluginLoader error: Cannot load library /root/src/CARTAvis/build/cpp/plugins/ImageStatistics/libplugin.so: (libplugin.so: cannot open shared object file: No such file or directory)
# QLibrary error:Cannot load library /root/src/CARTAvis/build/cpp/plugins/ImageStatistics/libplugin.so: (libplugin.so: cannot open shared object file: No such file or directory)"
# "Loading plugin DevIntegration[c++]"
# Trying to load native plugin "DevIntegration"
#   - heuristics to load libraries: 0
# Raw plugin loaded.
# Carta plugin loaded.
