Introduction to build and use Desktop ver. of Carta Viewer on CentOS 6, 7 and Ubuntu 14.04~16.04

=======

Tested: CentOS 7, Ubuntu 16.04.

# Steps before building CARTA

## Install basic build tools

Paste the the following script part into your terminal to install. Ignore this step if you already have compatible version of them but you need modify some path by yourself.

Our gcc/g++ minimal requirement is >=4.8.1

CentOS 7:

```
sudo yum -y update;\
sudo yum -y install gcc gcc-c++ make cmake git subversion-libs unzip wget
```

Ubuntu 16.04:
```
sudo apt-get -y update;\
sudo apt-get -y install gcc g++;\
sudo apt-get -y install make cmake git subversion unzip wget curl
```

## Choose your working space (folder) of CARTA
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

## Download and install + Qt 5.3.2 library + latest Qt Creator (4.2.x)

CARTA uses Qt 5.3.2 and it uses QtWebKit which exists in Qt 5.3, 5.4. Prebuilt Qt 5.5 installer does not QtWebkit but can be built from 5.5 source code. After 5.6, QtWebKit is replaced by QtWebEngine which is based on Chronium.

It is encouraged to use newest IDE to get more good development features. You can use only command line without Qt Creator, or use the other IDE, such as **Eclipse** or **IntelliJ IDEA**.

cd `your-carta-work`, then   

### Either execute prepared script to install
`sudo ./CARTAvis/carta/scripts/installqt5.3.sh` which does the following things

1. download Qt online installer `qt-unified-linux-x64-2.0.5-online.run` under your ~/download folder
2. install needed package before launching Qt 5.3.2 installer
3. Please choose `/opt/Qt` as the installed position. You can install in other position but you need to change Qt path in the following scripts. (This path may not be good enough, will change to use default path later)
4. During installing, uncheck 5.8 and check Qt 5.3 and keep Qt Creator 4.2.x checked.

Qt 5.3 is for following things:

1. To build carta by command line.
2. To build Qwt 6.1.2 needed by Carta in some scripts.  

### Or you can download the the other Qt offline installers to install Qt Creator and Qt 5.3.2

1. Latest Qt creator (4.2.x): https://download.qt.io/official_releases/qtcreator/4.2/4.2.0/
2. Qt 5.3.2, bundle with old Qt creator (3.2.x): http://download.qt.io/archive/qt/5.3/5.3.2/

Also, if you choose another path to install (Qt lib part), you need to setup QT5PATH variable manually, such as `QT5PATH=/opt/Qt/5.3/gcc_64/bin/`

## Install most Third Party libraries, some are built from source code
cd `your-carta-work`, execute `sudo ./CARTAvis/carta/scripts/install3party.sh`

## Build CASA libraries

**CentOS 7:**

The `buildcasa.sh` will use `yum` to install specific version of gcc, g++ compilers (4.9.2) and gfortran from casa yum repo. But of course you can use any installed build tools. Just specify the path of them into cmake flag of building casa part in `buildcasa.sh`.

cd `your-carta-work`, execute
`./CARTAvis/carta/scripts/buildcasa.sh`, which does the following things

1. Download Qt 4.8.5 source code (269MB), build and install it into `/usr/local/Trolltech/Qt-4.8.5/`, take hours. Keep in mind that you **can not** ignore executing the previous steps of `buildcasa.sh` which install some `dbus` related libraries which are needed by qt-dbus module, which is neeed for **code submodule**.
2. Use Qt 4.8.5 to build needed Qwt 6.1.0.
3. svn checkout casa source code into `$CARTAWORKHOME/CARTAvis-externals/ThirdParty/casa/trunk/`, then use Qt 4.8.5 to build CASA libraries.

The default build flag for CASA I try is `make -j 2`, and this is a compromise way. Only `make` is very slow but setting more than 2 let the possibility of building fail become higher, since to no official support of building casa. You can try other flags to build (e.g. `make` to guarantee success or more than `2`). Also `make -j` may not be the fatest (and it may also hang on your computers). The faster `n` in `make -j(n)` is according your environment and may be different.

To reduce time spending on building Qt 4.8.5, it is possible to install pre-built Qt 4.8.5. `yum install qt-devel.x86_64 qt.x86_64` (Not try yet).

**Ubuntu 16.04:**

Same as CentOS, except will not install another specific g++ compilers.

It is possible to install built 4.8.7 by `apt-get install libqt4-dev libqt4-dev-bin` (Not test yet). More modules: `apt-get install libqt4-debug libqt4-gui libqt4-sql libqt4-dev-tools qt4-doc qt4-designer qt4-qtconfig`.

### Some notes about Casa: ###

#### 1.Carta use two submodule of Casa ####

The main code repo of Casa is https://svn.cv.nrao.edu/svn/casa/trunk/

There are some main submodule
1. casacore: svn external of the above url. It it in GitHub and also can be cloned by Git. (GitHub supplies svn)
2. code (reply on casacore)
3. gcwrap
4. others...

We only `casacore` and `code`. Regarding `code`, we mainly use `code/imageanalysis`.

#### 2. Dependency between Carta and Casa ####

Some of third-party libraries they use are the same,  but may use different version. Here is the list.

1. cfitsio
2. wcslib
3. flex
4. bison
5. gsl (carta requires 2.1 version of source code to build,  and casa/code seems not require version and we usually install apt/ym version, 1.5 for casa/code)
6. Cython (?,required by carta, but not sure is required by casa-submodues)
7. gfortan (carta uses ast library which uses gforan, and casacore uses this, too)

#### 3.How to change the revision of casa we use to build ####

Go to `buildcasa.sh`, change the svn revision of casa we use to checkout. Now we use a fixed revision around September, 2016.

#### 4.Start from August, 2016 to November, 2016, namaspace of casa libs was changing ####

It transited from **CASA::** to **CASACORE::**. So do not use the combination of the submodules (casacore and code) during this time.

# Build Carta program

## Choose the location of build folder

Suggested path:

`mkdir -p $CARTAWORKHOME/CARTAvis/build`

## Use command line to build

1. Install required libraries when building CARTA

    CentOS 7:
    ```
    ## gstreamer libs are needed by Qt (webkit).
    ## python-devel will install Python.h (should be 2.7)
    sudo yum -y install gstreamer-devel; \
    sudo yum -y install gstreamer-plugins-base; \
    sudo yum -y install python-devel;
    ```
    Ubuntu 16.04:
    ```
    sudo apt-get -y install libgstreamer0.10-dev gstreamer0.10-plugins-base

    ## seems no need to manually install the below
    ## sudo apt-get -y install libpython2.7-dev; #(or  python-dev?)
    ```

2. Setup Qt5 path,
 `export PATH=/opt/Qt/5.3/gcc_64/bin/:$PATH` or `your Qt Path`

3. `cd $CARTAWORKHOME/CARTAvis/build`
4. `qmake NOSERVER=1 CARTA_BUILD_TYPE=dev $CARTAWORKHOME/CARTAvis/carta -r`
5. `make -j 2`

## Use Qt creator to build and debug (will complement this part later)

Open carta.pro, then setup some build and run setting, then build.

## Use Other IDE to build and debug

# Build needed JavaScript UI files of CARTA

cd `your-carta-work`, execute `./CARTAvis/carta/scripts/generateUIfile.sh`

# Run Carta

Need to prepare some things needed for running CARTA and also appended parameters.

### requirement 1: create needed folders

cd `your-carta-work`,, execute `./CARTAvis/carta/scripts/setupcartavis.sh`.

It is optional. You do not need to setup this and can use CARTA smoothly. But not sure if `snaptshot` function of CARTA will work OK without setup this.

### requirement 2: setup necessary config.json

Paste the following data to be the content of `~/.cartavis/config.json`

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
}
```

### requirement 3: install data of geodetic, ephemerides for some kinds of fits file.

Paste the following content to your terminal to install.

```
mkdir data ; \
mkdir data/ephemerides ;\
mkdir data/geodetic ; \
svn co https://svn.cv.nrao.edu/svn/casa-data/distro/ephemerides/ data/ephemerides ;\
svn co https://svn.cv.nrao.edu/svn/casa-data/distro/geodetic/ data/geodetic ; \
mv data ~/
```

The default location is under home directory `~/`, and will be improved to better place.

### requirement 4: prepare fits or casa image format files.

The default loading path is `~/CARTA/Images` and you can put there or other places (you need to switch the folder in the file browser of CARTA).

You can also chooose fits file in this git project folder, `your-carta-work/CARTAvis/carta/scriptedClient/tests/data` when using file browser of CARTA. The other ways to get testing fits files,

1. https://drive.google.com/open?id=0B22Opq0T64ObTGJhNTBGeU04elU (zip file)
2. https://svn.cv.nrao.edu/svn/casa-data/trunk/demo/ (some files here)
3. Contact ASIAA members to get some.

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

## Run and Debug by Qt Creator

# Prepare distributable and packaged installer/zip

#Build on CI/CD

To do list

1. try to install Qt 5.3.2 without GUI
2. use *rpath* to solve dynamic search path issue on Carta.
3. try to use apt/yum way to install pre-built some third party libs for carta. (cfitsio, wcslib, even gsl but need to change default installation path of gsl)
