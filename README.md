Carta Viewer
=======

# Steps before building CARTA

## Choose where you want to build CARTA
Current `CARTAvis` uses the following folder structure

```
../CARTAvis (this git project folder)
../CARTAvis-externals (the Qt project setting of CARTAvis will look for this folder which collects Third-Party libs)
```
Which means there must be something outside the source code directory, and this way may not good enough, we will improve later. So the current suggested way is to choose a `root` folder to be your `CARTA working folder`, like this
```
~/cartawork/CARTAvis
~/cartawork/CARTAvis-externals
```

Create this working folder, then cd into it, then
`git clone -b toImproveBuild https://github.com/CARTAvis/carta.git`

Since `CARTAvis` is the old git repo name and used in some testing and building scripts, use new name `carta` may be OK when developing but may happen issues at other time, so just rename `carta` to `CARTAvis` after downloading.

## Download and install Qt Creator + Qt 5.3.2 library

In `your-carta-work folder`, then   

### Either execute prepared script to install
`./CARTAvis/carta/scripts/installqt5.3.sh` which does the following things

1. download `qt-opensource-linux-x64-5.3.2.run` under your ~/download folder
2. install needed package before launching Qt 5.3.2 installer
3. launch it to show installer GUI to install Qt 5.3.2 and its Qt Creator.
4. Please choose ../your-carta-work/CARTAvis-externals/ThirdParty/Qt5.3.2/ as the installed position. You can install in other position but you need to change Qt path in the following scripts. (will add more instruction later)
5. another script, `install3party.sh` uses `QT5PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/Qt5.3.2/5.3/gcc_64/bin/`

To use Qt5 binary path is for following things:

1. To build carta by command line.
2. To build Qwt needed by Carta in some scripts.  

### Or you can download the latest Qt online installer to install the latest Qt Creator and Qt 5.3.2
Go to Qt official site to download or use this link http://ftp.jaist.ac.jp/pub/qtproject/archive/online_installers/2.0/qt-unified-linux-x64-2.0.5-online.run

This way need to change `QT5PATH` variable manually in the following scripts.

## Install most Third Party libraries, some are built from source code
In `your-carta-work folder`, execute
`./CARTAvis/carta/scripts/install3party.sh`

## Build Casa libraries on CentOS 7
In `your-carta-work folder`, execute
`./CARTAvis/carta/scripts/buildcasa.sh`, which does the following things

1. build and install Qt 4.8.5 into `/usr/local/Trolltech/Qt-4.8.5/`. May find some pre-built Qt 4.8.5 package to improve the speed.
2. Use Qt 4.8.5 to build needed Qwt.
3. Use Qt 4.8.5 to build CASA libraries.

The default build flag for CASA is `make`, you can change to use `make -j` in the script to build parallel but it may build fail due to no official support of building casa.

# Build Carta

## choose the location of build folder

Suggested path:

`mkdir -p $CARTAWORKHOME/CARTAvis/build`

## Use command line to build

1. Setup Qt5 path,
 `export PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/Qt5.3.2/5.3/gcc_64/bin/:$PATH or `your Qt Path`
`
2. `cd $CARTAWORKHOME/CARTAvis/build`
3. `qmake NOSERVER=1 CARTA_BUILD_TYPE=dev $CARTAWORKHOME/CARTAvis/carta -r`
4. `make -j`

### Use Qt creator to build and debug

Open carta.pro, then setup some build and run setting, then build.

# Run Carta

Need to prepare some things needed for running CARTA and also appended parameters 

1. setup necessary config.json
2. (optinal?) setup snapshots folder.  
3. setup geodetic, ephemerides folders for some kinds of fits file.
4. prepare fits or casa image format file.

### Run by command line

1. execute `ulimit -n 2000` before running Carta
2. run `Carta` binary with parameters, at least should append `html file path`.

### Run by Qt Creator

## Prepare distributable and packaged installer/zip

#Build on CI/CD

To do list

1. find a way to install Qt without GUI
