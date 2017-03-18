CARTA Viewer
=======

#### Branching model
`master`:  
mainstream branch, no development.

`develop`:  
development branch, usually will merge feature branch to it and include hot fix for those feature. After each phase release, merge `develop` to `master`.  

`each feature branch`:  
people develop each feature in its branch whose name can be `peter/drawImage` or `issue-131` if we use tickets. When it is finished, use `pull request` to proceed code review and then merge to develop. After merging, evaluate those added features on `develop`.

`Fix Bug`:
Except some special cases, such as modifying documents, changing build scripts, low/no-risk fixes, otherwise you need to commit your bug fixes in Hotfix branch or the original feature branch, then make a pull request to do code review.

Introduction to build, run and deploy Desktop ver. of CARTA Viewer on Mac and Linux
=======

Development platform:
1. CentOS 7 (7.3.1611 tested)
2. Ubuntu 14.04 (not tested yet with this guide)
3. Ubuntu 16.04
4. Mac 10.11, 10.12

Supported deployment platform:
1. CentOS 6, 7
2. Ubuntu (not official)
3. Mac: OS X El Capitan (10.11), macOS Sierra (10.12)

Tested g++ compiler: 4.8.5, 5.4 (used by Ubuntu 16.04).

CARTA can be built by Qt 5.3, 5.4, 5.5, 5.6 (not tested), 5.7.

# Steps before building CARTA

## Step1 - Install basic build tools

Paste the the following script part into your terminal to install. Ignore this step if you already have compatible version of them but you need modify some path by yourself.

CARTA's gcc/g++ minimal requirement is >=4.8.1, to use c++11 full features.

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

Mac 10.11, 10.12:

**Install Xcode and get its g++ compiler**

On Mac, its g++ compiler is called clang and its standard c++ lib is **libc++**. Command **g++** is linking to clang. Do the following to install.

- Install Xcode 8 from **App Store**.
- Launch Xcode 8 or type `xcodebuild -license` to accept the license
- Type `xcode-select --install` to install xcode command line tools.

## Step2 - Choose your working space (folder) of CARTA and clone source code of CARTA
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

**Create this working folder**, alias `your-carta-work`(will use variable CARTAWORKHOME to store this path somewhere),  then

1. cd `your-carta-work`
2. `git clone https://github.com/CARTAvis/carta.git CARTAvis`

p.s. Since `CARTAvis` is the old git repo name and used in some testing and building scripts, use new name `carta` may be OK when developing but may happen issues at other time, so just rename `carta` to `CARTAvis` when git cloning.

## Step3 - Download and install + Qt 5.3.2 library + latest Qt Creator (4.2.x)

CARTA uses Qt 5.3.2 and it uses QtWebKit which exists in Qt 5.3, 5.4, 5.5. Start from Qt 5.6, its installer does not include QtWebkit but people can be build it from Qt source code. QtWebKit is replaced by QtWebEngine framework which is based on Chromium.

It is encouraged to use newest IDE to get more good development features. You can use only command line without Qt Creator, or use the other IDE, such as **Xcode**, **Eclipse** or **IntelliJ IDEA**.

cd `your-carta-work`, then   

### On Linux, execute prepared script to install, or you can use the below Mac's way on Linux
`sudo ./CARTAvis/carta/scripts/installqt5.3.sh` which does the following things

1. download Qt online installer `qt-unified-linux-x64-2.0.5-online.run` under your ~/download folder
2. install needed package before launching Qt 5.3.2 installer
3. Please choose **/opt/Qt** as the installed position. You can install in other position but you need to change Qt path in the following scripts. (This path may not be good enough, will change to use default path later)
4. During installing, uncheck 5.8 and check Qt 5.3 and keep Qt Creator 4.2.x checked.

Qt 5.3 is for following things:

1. To build CARTA.
2. To build Qwt 6.1.2 needed by CARTA in some scripts.  

### On Mac, download Qt installers to install Qt Creator and Qt 5.3.2

Download Latest Qt creator (4.2.x) + (optional) Qt : http://ftp.jaist.ac.jp/pub/qtproject/archive/online_installers/2.0/qt-unified-mac-x64-2.0.5-online.dmg

Please use **~/Qt** as your installation location.

You need to do the following patch for Qt on Mac.

1. One issue about Xcode + Qt 5.3 (5.5, 5.6 do not need). In $HOME/Qt/5.x/clang_64/mkspecs/qdevice.pri, Change `!host_build:QMAKE_MAC_SDK = macosx10.8` to `!host_build:QMAKE_MAC_SDK = macosx`. Ref: http://stackoverflow.com/questions/26320677/error-could-not-resolve-sdk-path-for-macosx10-8

2. one issue about Xcode 8 + Qt. In Qt/5.x/clang_64/mkspecs/macx-clang/qmake.conf, chanage `QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7` to `QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10`. Ref: http://stackoverflow.com/questions/24243176/how-to-specify-target-mac-os-x-version-using-qmake.

3. (Not always happen, fix when it happen). When this error, **Qt Creator - Project ERROR: Xcode not set up properly. You may need to confirm the license agreement by running /usr/bin/xcodebuild** happens, open Qt_install_folder/5.7/clang_64/mkspecs/features/mac/default_pre.prf, replace `isEmpty($$list($$system("/usr/bin/xcrun -find xcrun 2>/dev/null")))` with `isEmpty($$list($$system("/usr/bin/xcrun -find xcodebuild 2>/dev/null")))`. Ref: http://stackoverflow.com/questions/33728905/qt-creator-project-error-xcode-not-set-up-properly-you-may-need-to-confirm-t.

### If you choose your preferred position to install Qt 5.3.2

Setup QT5PATH variable, like `QT5PATH=/opt/Qt/5.3/gcc_64/bin`, this is to build Qwt in the following step.

### [Experiment] Use Qt 5.7 + Community QtWebKit package on Mac

1. Get Webkit from https://github.com/annulen/webkit/releases/, use its version >= QtWebKit Technology Preview 4.
2. [Fix] Workaround for users of Xcode withi this package http://qt5blogger.blogspot.tw/

## Step4 - Install most Third Party libraries, some are built from source code
cd `your-carta-work`, execute `sudo ./CARTAvis/carta/scripts/install3party.sh`

## Step5 - Build CASA libraries

**Mac 10.12:**

1. Use Homebrew and Macports to install needed libraries first, follow this guide,
https://github.com/CARTAvis/carta/wiki/Install-Third-Party-For-CARTA-CASA-On-Mac

2. cd `your-carta-work`, execute `./CARTAvis/carta/scripts/buildcasa.sh`.

3. After building CASA libraries, **brew unlink qt** to remove Qt 4.8 from `PATH`. Otherise it will find out Qt 4.8 headers and libs when building CARTA which only need Qt 5.3.2 or higher Qt.

**CentOS 7:**

The `buildcasa.sh` will use `yum` to install specific version of gcc, g++ compilers (4.9.2) and gfortran from casa yum repo. But of course you can use any installed build tools. Just specify the path of them into cmake flag of building casa part in `buildcasa.sh`.

cd `your-carta-work`, execute
`./CARTAvis/carta/scripts/buildcasa.sh`, which does the following things

1. Use **yum install qt-devel.x86_64 qt.x86_64** to get Qt 4.8.5.
2. Use Qt 4.8.5 to build needed Qwt 6.1.0.
3. svn checkout casa source code into `$CARTAWORKHOME/CARTAvis-externals/ThirdParty/casa/trunk/`, then use Qt 4.8.5 to build CASA libraries.

The default build flag for CASA I try is `make -j2`, and this is a compromise way. Only `make` is very slow but setting more than 2 let the possibility of building fail become higher, since to no official support of building casa. You can try other flags to build (e.g. `make` to guarantee success or more than `2`). Also `make -j` may not be the fatest (and it may also hang on your computers). The faster `n` in `make -j(n)` is according your environment and may be different.

**Ubuntu 16.04:**

Same as CentOS, use `apt-get install libqt4-dev` to install **Qt 4.8.7** (the build script has changed to use this but not test yet). More installable modules: `apt-get install libqt4-dev-tools qt4-doc qt4-designer qt4-qtconfig libqt4-debug? libqt4-gui?`.

Old way (approved), previous version of `buildcasa` script, downloads the source code of Qt 4.8.5 (269MB) to build** and install it into `/usr/local/Trolltech/Qt-4.8.5/`, take hours. Keep in mind that you **can not** ignore executing the previous steps of `buildcasa.sh` which install some `dbus` related libraries which are needed by QtDbus module, which is neeed for **code submodule**.

**TODO**:
Now we switch to use `apt/yum` to install built Qt 4.8.x to build casa submodue, code. Need to investigate do they supply static version Qt lib? It may let final casa and needed Qt libs bigger. Ubuntu: check /usr/lib/x86_64-linux-gnu, seems not having static version, like libQtGui.a.   

### Some notes about Casa:

#### 1.CARTA use two submodule of Casa

The main code repo of Casa is https://svn.cv.nrao.edu/svn/casa/trunk/

There are some main submodule

1. casacore: in trunk, svn external of the above url. It is in GitHub and also can be cloned by Git (GitHub supplies svn).
    https://github.com/casacore/casacore
2. code (reply on casacore)
3. gcwrap
4. asap (in trunk, it uses svn external)

We only use `casacore` and `code`. Regarding `code`, we mainly use **code/imageanalysis** and its dependency is **code/stdcasa and code/components**.

#### 2. Dependency between Carta and Casa

Some of third-party libraries they use are the same,  but may use different version. Here is the list.

1. cfitsio
2. wcslib (CARTA build from source code and its default configure seems to have pgplot. casa: after installing wcslib from apt/yum, casa-code still needs people to install pgplot)
3. flex
4. bison
5. gsl (CARTA uses 2.1 version of gsl source code to build, and casa/code seems not require specific version and we usually install apt/ym version, 1.5 for casa/code)
6. blas lib. (required when building GSL, also when building casacore)
7. gfortan (CARTA uses ast library which uses gfortan, and casacore uses this, too)
8. Qt. (CARTA uses 5.3.2, 5.4/5.5 may be ok. casa & its needed qwt uses 4.8.5)  
9. qwt (CARTA uses qt 5.3.2 to build qwt 6.1.2 and casa submodule, code uses 4.8.5 to build qwt 6.1.0)
9. Python and numpy (should be ok. CARTA:2.7. casa:2.7)

#### 3.How to change the revision of casa we use to build

Go to `buildcasa.sh`, change the svn revision of casa we use to checkout. Now we use a fixed revision around September, 2016.

#### 4.Start from August, 2016 to November, 2016, namaspace of casa libs was changing

It transited from **CASA::** to **CASACORE::**. So do not use the combination of the submodules (casacore and code) during this time.

# Build CARTA program

## Step1 - Choose the location of build folder

Suggested path:

`mkdir -p $CARTAWORKHOME/CARTAvis/build`

## Step2 - Build needed Qooxdoo JavaScript UI files of CARTA

cd `your-carta-work`, execute `./CARTAvis/carta/scripts/buildUIfile.sh`. This step is not necessary to be executed before buliding CARTA (Qt-c++ program). Without this, you will see blank UI when you launching CARTA. Also, if you just modify JavaScript UI file but do not add new JavaScript class, you do not need to build again.   

## Step3 - use command line to build

1. Install required libraries when building CARTA

    CentOS 7:
    ```
    ## gstreamer libs are needed by Qt (webkit).
    ## python-devel will install Python.h (should be 2.7)
    ## sudo yum -y install python-devel; (will be installed when building casa)
    ```
    Ubuntu 16.04:
    ```
    sudo apt-get -y install libgstreamer0.10-dev gstreamer0.10-plugins-base

    ## seems no need to manually install the below
    ## sudo apt-get -y install libpython2.7-dev; #(or  python-dev?)
    ```

2. Setup Qt5 path

    Linux: `export PATH=/opt/Qt/5.3/gcc_64/bin:$PATH` or `your Qt Path`.

    Mac: `export PATH=$HOME/Qt/5.x/clang_64/bin:$PATH`. You can type `qmake --versoion` to check the current Qt version.  

3. `cd $CARTAWORKHOME/CARTAvis/build`
4. `qmake NOSERVER=1 CARTA_BUILD_TYPE=dev $CARTAWORKHOME/CARTAvis/carta -r`
5. `make -j2`

**CARTA_BUILD_TYPE** can be

1. release
2. bughunter (for debugging/debugger, having symbol information, also it is the only one which uses **-O0** (no optimization) instead of using **-O2** optimization.  
3. dev (having more logs than release, but no debug symbol for debugging)

## Use Qt creator to build and debug (will complement this part later)

Open carta.pro, then setup some build and run setting, then build.

## Use Other IDE to build and debug

# Run CARTA

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
        "$(APPDIR)/../../../../plugins"
    ],
    "disabledPlugins" : ["casaCore-2.0.1"],
    "plugins": {
        "PCacheSqlite3" : {
            "dbPath": "/tmp/pcache.sqlite"
        }
    },
    "qtDecorations" : "true"
}
```

`$(APPDIR)/../plugins` is for Linux. `"$(APPDIR)/../../../../plugins"` is for Mac.

You can browse more detailed instruciton about these parameters from here,
http://cartaserver.ddns.net/docs/html/developer/contribute/Writinganimageplugin.html#appendix-e-carta-config-file

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

### requirement 4: prepare fits, casa image format, or miriad files.

The default loading path is `~/CARTA/Images` and you can put there or other places (you need to switch the folder in the file browser of CARTA).

You can also chooose fits file in this git project folder, `your-carta-work/CARTAvis/carta/scriptedClient/tests/data` when using file browser of CARTA. The other ways to get testing fits files,

1. https://drive.google.com/open?id=0B22Opq0T64ObTGJhNTBGeU04elU (zip file)
2. https://svn.cv.nrao.edu/svn/casa-data/trunk/demo/ (some files here)
3. Contact ASIAA members to get some.

## Run by command line

1. Current CARTA needs to execute the following command every time to find correct **dynamic/shared Library** before running CARTA. Will improve later by using `rpath`. It seems that we don't setup for libCARTA.so and libcore.so.

    1. setup LD_LIBRARY_PATH on Mac/Linux
    ## ok way, use this

    ```
    export LD_LIBRARY_PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/casa/trunk/linux/lib:${LD_LIBRARY_PATH}
    ```
    ## or this path, should work but not test. It is the symbolic link of the above path,
    ## export LD_LIBRARY_PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/casacore/lib:${LD_LIBRARY_PATH}

    ```
    ## no need to setup the path wcslib, we already use QMAKE_RPATHDIR 
    ## export LD_LIBRARY_PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/wcslib/lib:${LD_LIBRARY_PATH}
    ```

    2. On Mac, you can use Qt Creator build **without setting 1st thing (LD_LIBRARY_PATH)**. In Qt Creator, there is a default enabled setting which will automatically add build library search path to DYLD_LIBRARY_PATH and DYLD_FRAMEWORK_PATH (Mac, work), add build library search path to LD_LIBRARY_PATH (Linux, not work, don't know why).

    3. On Mac, you need to setup below thing (will improve later), you can copy them as `Custom Process Setup` in Qt Creator's or a shell script
    ```
    cd $CARTAWORKHOME/CARTAvis
    export CARTABUILDHOME=`pwd`

    mkdir -p $CARTABUILDHOME/build/cpp/desktop/desktop.app/Contents/Frameworks/
    cp $CARTABUILDHOME/build/cpp/core/libcore.1.dylib $CARTABUILDHOME/build/cpp/desktop/desktop.app/Contents/Frameworks/

    # need to rm for qt creator 4.2, otherwise when build+run together will result in core/libcore.1.dylib not able find out qwt
    rm $CARTABUILDHOME/build/cpp/core/libcore.1.dylib
    cp $CARTABUILDHOME/build/cpp/CartaLib/libCartaLib.1.dylib $CARTABUILDHOME/build/cpp/desktop/desktop.app/Contents/Frameworks/

    install_name_tool -change qwt.framework/Versions/6/qwt $CARTABUILDHOME/ThirdParty/qwt-6.1.2/lib/qwt.framework/Versions/6/qwt $CARTABUILDHOME/build/cpp/desktop/desktop.app/Contents/MacOS/desktop
    install_name_tool -change qwt.framework/Versions/6/qwt $CARTABUILDHOME/ThirdParty/qwt-6.1.2/lib/qwt.framework/Versions/6/qwt $CARTABUILDHOME/build/cpp/desktop/desktop.app/Contents/Frameworks/libcore.1.dylib

    # not sure the effect of the below line, try comment
    # install_name_tool -change libplugin.dylib $CARTABUILDHOME/build/cpp/plugins/CasaImageLoader/libplugin.dylib $CARTABUILDHOME/build/cpp/plugins/ImageStatistics/libplugin.dylib
    install_name_tool -change libcore.1.dylib  $CARTABUILDHOME/build/cpp/desktop/desktop.app/Contents/Frameworks/libcore.1.dylib $CARTABUILDHOME/build/cpp/plugins/ImageStatistics/libplugin.dylib

    install_name_tool -change libCartaLib.1.dylib  $CARTABUILDHOME/build/cpp/desktop/desktop.app/Contents/Frameworks/libCartaLib.1.dylib $CARTABUILDHOME/build/cpp/plugins/ImageStatistics/libplugin.dylib
    install_name_tool -change libcore.1.dylib  $CARTABUILDHOME/build/cpp/desktop/desktop.app/Contents/Frameworks/libcore.1.dylib $CARTABUILDHOME/build/cpp/desktop/desktop.app/Contents/MacOS/desktop
    install_name_tool -change libCartaLib.1.dylib  $CARTABUILDHOME/build/cpp/desktop/desktop.app/Contents/Frameworks/libCartaLib.1.dylib $CARTABUILDHOME/build/cpp/desktop/desktop.app/Contents/MacOS/desktop
    install_name_tool -change libCartaLib.1.dylib  $CARTABUILDHOME/build/cpp/desktop/desktop.app/Contents/Frameworks/libCartaLib.1.dylib $CARTABUILDHOME/build/cpp/desktop/desktop.app/Contents/Frameworks/libcore.1.dylib

    for f in `find . -name libplugin.dylib`; do install_name_tool -change libcore.1.dylib  $CARTABUILDHOME/build/cpp/desktop/desktop.app/Contents/Frameworks/libcore.1.dylib $f; done
    for f in `find . -name libplugin.dylib`; do install_name_tool -change libCartaLib.1.dylib  $CARTABUILDHOME/build/cpp/desktop/desktop.app/Contents/Frameworks/libCartaLib.1.dylib $f; done
    for f in `find . -name "*.dylib"`; do install_name_tool -change libwcs.5.15.dylib  $CARTABUILDHOME/ThirdParty/wcslib/lib/libwcs.5.15.dylib $f; echo $f; done
    ```

2. execute `ulimit -n 2000` before running Carta
3. To run `CARTA` binary with parameters, at least should append `html file path`, example:

```
$CARTAWORKHOME/CARTAvis/build/cpp/desktop/desktop --html $CARTAWORKHOME/CARTAvis/carta/html5/desktop/desktopIndex.html
```

Some of optional parameters:

1. `--scriptPort 9999` for python interface
2. put `/scratch/some-fits-file.fits` in the end

## Run and Debug by Qt Creator

# Deployment: Prepare distributable and packaged installer/zip

Start from Qt 5.5, the **rpath** of dependent Qt library will use **@rpath** instead of using absolute path, on Mac.

The flow is not finalized. Related some library search issues.

To make its size smaller:

1. remove *.o, Makefile, .h and .cpp* in build folder.
2. use `strip` skill to remove unused part of binary.

Observation about build Size (on Mac), before packaging:

```
553M    Carta.app   (without default image)
            qooxdoo (in Resources) ~ 133 MB
            QT (in Frameworks) with a size of ~ 300M
            casa libraries (in Frameworks) with ~ 60 M
            other libraries ~ 20M.  
744M    build folder (Carta.app +others) + CARTA source code +  third-party libs(+ possible source code)
1.5G    prebuilt casacore/casacode + possible source code
```

# Dynamic/Shared Library search issue

1. CARTA is built to **desktop program + dynamic libs (libCARTA, libcore, many libPlugin built from Qt)** and use "a few static Third-party library + many dynamic Third-party library".

2. It seems that Qt-built dynamic libs do not have **Search issue**, at least before moving desktop program (for packaging).

3. We use **install_name_tool** (Mac) and **chrpath** or **PatchELF** (Linux) to specify dynamic linking path of each lib. On mac, **desktop** is located in **desktop.app/Contents/MacOS/desktop**  after building. Linux does not have these folder, so we need to have different handles.

# Build on CI/CD

# To do list

-[] [Done] to check wcslib and cfitsio

    ```    
    On 12 Mar 2016,  https://github.com/Astroua/CARTAvis/releases/tag/0.6.0
    When compiling, one has to be make sure that wcslib and cfitsio libraries are in synch (same version)
    between CARTA and CASA builds.
    ```

- [ ] use **-Wl, rpath** to solve dynamic search path issue on CARTA instead of setting LD_LIBRARY_PATH/DYLD_LIBRARY_PATH(on Mac)/QMAKE_RPATHDIR **OR** change **dynamic link** to **static link**.

- [ ] try to use apt/yum way to install pre-built some third party libs for CARTA. (cfitsio, wcslib, even gsl but need to change default installation path of gsl)

- [x] try to install pre-built Qt 4.8.x to build qwt and casa. But should check if it includes QtDbus or not first (Homebrew's bottle does not include and need rebuilt. CentOS's yum version has).

- [ ] try to install Qt 5.3.2 without GUI, especially for CI/CD. e.g. `apt-get install qtbase5-dev` (not try)

- [ ] May bundle multiple dynamic libs to one libs can lower the complexity, and it is possible done by cmake in the future.

### AppendixA:

CARTA third party libs list:
https://docs.google.com/spreadsheets/d/1SpwEZM2n6xDGBEd6Nisn0s8KbBQG-DtoXUcFONzXXtY/edit#gid=0

## Library and Plugin Issue
1. ImageStatistics plugin sometimes does not work on developer's Mac/Linux.
2. Fitter1d sometimes does not work (Ubuntu).
3. RegionDS9 does not work on every developer's Mac/Ubuntu and in some CentOS. Already know why.
4. ProfileCASA does not work on every developer's Mac. [Having clues]
