Carta Viewer
=======

# Steps before building CARTA on CentOS 7

## Install basic build tools

Paste the script part of the following content in your terminal to install. Ignore this step if you already have compatible version of them.

```
## Our gcc/g++ minial requirement is >=4.8.1
## sudo yum -y devtoolset* will install gcc, g++, gfortran etc tools under /opt/rh/devtoolset-3 
## cmake is needed by building casa

sudo yum -y install wget unzip; \
sudo yum -y devtoolset*;\
sudo yum -y install make git svn ; \
sudo yum -y install cmake; \
```

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

Create this working folder, alias `your-carta-work`,  then cd `your-carta-work`, then
`git clone -b toImproveBuild https://github.com/CARTAvis/carta.git CARTAvis`

p.s. Since `CARTAvis` is the old git repo name and used in some testing and building scripts, use new name `carta` may be OK when developing but may happen issues at other time, so just rename `carta` to `CARTAvis` when git cloning.

## Download and install Qt Creator + Qt 5.3.2 library

cd `your-carta-work`, then   

### Either execute prepared script to install
`sudo ./CARTAvis/carta/scripts/installqt5.3.sh` which does the following things

1. download Qt online installer `qt-unified-linux-x64-2.0.5-online.run` under your ~/download folder
2. install needed package before launching Qt 5.3.2 installer
3. Please choose ../your-carta-work/CARTAvis-externals/ThirdParty/Qt/ as the installed position. You can install in other position but you need to change Qt path in the following scripts. (will add more instruction later)
4. During installing, check 5.8 and check Qt 5.3 and keep Qt Creator 4.2.x checked. 

Qt 5.3 is for following things:

1. To build carta by command line.
2. To build Qwt 6.1.2 needed by Carta in some scripts.  

### Or you can download the the other Qt installers to install Qt Creator and Qt 5.3.2

Such as this offline installer, http://download.qt.io/archive/qt/5.8/5.8.0/qt-opensource-linux-x64-5.8.0.run. Also, if you choose another path to install, you need to setup QT5PATH variable manually, such as `QT5PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/Qt/5.3/gcc_64/bin/`

## Install most Third Party libraries, some are built from source code
cd `your-carta-work`, execute `sudo ./CARTAvis/carta/scripts/install3party.sh`

## Build CASA libraries

If you have your own gcc/g++/gfortran, please specify the path of them into cmake flag in buildcasa.sh

cd `your-carta-work`, execute
`./CARTAvis/carta/scripts/buildcasa.sh`, which does the following things

1. download Qt 4.8.5 source code (269MB), build and install it into `/usr/local/Trolltech/Qt-4.8.5/`, take 1 hour.
2. Use Qt 4.8.5 to build needed Qwt 6.1.0.
3. Use Qt 4.8.5 to build CASA libraries.

The default build flag for CASA is `make -j 2`, and this is a compromise way. Only `make` is very slow but setting more than 2 let the possibility of building fail become higher, since to no official support of building casa. You can try other flags to build. Also `make -j` may not be the fatest and it may also hang on your computers. 

# Build Carta program on CentOS 7

## Choose the location of build folder

Suggested path:

`mkdir -p $CARTAWORKHOME/CARTAvis/build`

## Use command line to build

1. Install required libraries when building CARTA

    ```
    ## gstreamer libs are needed by Qt (webkit).
    ## python-devel will install Python.h
    sudo yum -y install gstreamer-devel \
    sudo yum -y install gstreamer-plugins-base \
    sudo yum -y install python-devel
    ```

2. Setup Qt5 path,
 `export PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/Qt/5.3/gcc_64/bin/:$PATH` or `your Qt Path`

3. `cd $CARTAWORKHOME/CARTAvis/build`
4. `qmake NOSERVER=1 CARTA_BUILD_TYPE=dev $CARTAWORKHOME/CARTAvis/carta -r`
5. `make -j`

## Use Qt creator to build and debug (will complement debug part later)

Open carta.pro, then setup some build and run setting, then build.

# Build needed JavaScript UI files of CARTA

cd `your-carta-work`, execute `./CARTAvis/carta/scripts/generateUIfile.sh`

# Run Carta

Need to prepare some things needed for running CARTA and also appended parameters.

### requirement 1: create needed folders

Execute `./CARTAvis/carta/scripts/setupcartavis.sh`.

It is optional. You do not need to setup this and can use CARTA smoothly. But not sure if `snaptshot` function of CARTA will work OK without setup this.

### requirement 2: setup necessary config.json

Paste the following data to be the content of `~/.cartavis/config.json/config.json`

```
{
"_comment" : "List of plugin directories",
"pluginDirs": [
"$(APPDIR)/../plugins",
"$(APPDIR)/../../../../plugins",
"$(HOME)/.cartavis/plugins"
],
"disabledPlugins" : ["casaCore-2.0.1"],
"plugins": {
    "PCacheSqlite3" : {
        "dbPath": "/tmp/pcache.sqlite"
    }
}
```

### requirement 3: install data of geodetic, ephemerides for some kinds of fits file.

Paste the following content to your terminal to install. 

```
mkdir data ; \
mkdir data/ephemerides ;\
mkdir data/geodetic ; \
svn co https://svn.cv.nrao.edu/svn/casa-data/distro/ephemerides/ data/ephemerides ;\
svn co https://svn.cv.nrao.edu/svn/casa-data/distro/ephemerides/ data/geodetic ; \
mv data ~/
```

The default location is under home directory `~/`, and will be improved to better place. 

### requirement 4: prepare fits or casa image format files.

The default loading path is `~/CARTA/Images` and you can put there or other places (you need to switch the folder in the file browser of CARTA). 

You can also chooose fits file in this git project folder, `carta/carta/scriptedClient/tests/data` when using file browser of CARTA. or download some fits from https://drive.google.com/open?id=0B22Opq0T64ObTGJhNTBGeU04elU or https://svn.cv.nrao.edu/svn/casa-data/trunk/demo/ first. 

## Run by command line

1. Current Carta needs to execute the following command every time before running Carta. Will improve later by using `rpath`. It seems that we don't setup for libCARTA.so and libcore.so.

    ```
    export LD_LIBRARY_PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/casa/trunk/linux/lib:${LD_LIBRARY_PATH}

    ```

2. execute `ulimit -n 2000` before running Carta
3. To run `Carta` binary with parameters, at least should append `html file path`, example:

```
$CARTAWORKHOME/CARTAvis/build/cpp/desktop/desktop --html $CARTAWORKHOME/CARTAvis/carta/html5/desktop/desktopIndex.html
```

You can browse more detailed instruciton about these parameters from here, http://cartaserver.ddns.net/docs/html/developer/contribute/Writinganimageplugin.html#appendix-e-carta-config-file

## Run by Qt Creator

# Prepare distributable and packaged installer/zip

#Build on CI/CD

To do list

1. find a way to install Qt without GUI
