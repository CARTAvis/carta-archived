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
2. Ubuntu 14.04
3. Ubuntu 16.04
4. Mac 10.11, 10.12

Supported deployment platform:
1. CentOS 6, 7
2. Ubuntu
3. Mac: OS X El Capitan (10.11), macOS Sierra (10.12)

Tested c++ compiler: gcc 4.8.5, 5.4 (used by Ubuntu 16.04) & clang on macOS.

CARTA can be built by Qt 5.3, 5.4, 5.5, 5.6~5.8. Start from 5.6,  we need to install QtWebKit & QtWebKit additionally.

# Build CARTA

Follow https://github.com/CARTAvis/carta/wiki/build.

# Run CARTA

Need to prepare some things needed for running CARTA and also appended parameters.

### requirement 1: create needed folders

cd `your-carta-work`,, execute `./CARTAvis/carta/scripts/setupcartavis.sh`.

It is optional. You do not need to setup this and can use CARTA smoothly. But not sure if `snaptshot` function of CARTA will work OK without setup this.

### requirement 2: install data of geodetic, ephemerides for some kinds of fits file.

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

### requirement 3: prepare fits, casa image format, or miriad files.

The default loading path is `~/CARTA/Images` and you can put there or other places (you need to switch the folder in the file browser of CARTA).

You can also chooose fits file in this git project folder, `your-carta-work/CARTAvis/carta/scriptedClient/tests/data` when using file browser of CARTA. The other ways to get testing fits files,

1. https://drive.google.com/open?id=0B22Opq0T64ObTGJhNTBGeU04elU (zip file)
2. https://svn.cv.nrao.edu/svn/casa-data/trunk/demo/ (some files here)
3. Contact ASIAA members to get some.

### (optional) setup config.json to overwrite the default embedded setting

Paste the following data to be the content of `~/.cartavis/config.json`

```
{
    "_comment" : "List of plugin directories",
    "pluginDirs": [
        "$(APPDIR)/../plugins",
        "$(APPDIR)/../../../../plugins"
    ],
    "disabledPlugins" : ["tester1", "clock1", "blurpy"],
    "plugins": {
        "PCacheSqlite3" : {
            "dbPath": "/tmp/pcache.sqlite"
        }
    },
    "qtDecorations" : "true"
}
```

`$(APPDIR)/../plugins` is for Linux. `"$(APPDIR)/../../../../plugins"` is for Mac.

You can browse more detailed instruction about these parameters from here,
http://cartaserver.ddns.net/docs/html/developer/contribute/Writinganimageplugin.html#appendix-e-carta-config-file


## Run by command line

1. Current CARTA needs to execute the following command every time to find correct **dynamic/shared Library** before running CARTA. Will improve later by using `rpath`. It seems that we don't setup for libCARTA.so and libcore.so.

    1. setup LD_LIBRARY_PATH on Mac/Linux

    ```
    ## 1-1 for casa libs
    ## on Linux, use this
    export LD_LIBRARY_PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/casa/trunk/linux/lib:${LD_LIBRARY_PATH}
    ## on Mac, use this
    export LD_LIBRARY_PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/casa/trunk/darwin/lib:${LD_LIBRARY_PATH}
    ## or this path, should work but not test. It is the symbolic link of the above path,
    ## export LD_LIBRARY_PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/casacore/lib:${LD_LIBRARY_PATH}

    ## 1-2 for wcslib, only Mac. no need to setup the path wcslib on Linux, we already use QMAKE_RPATHDIR which seems not work on Mac.
    export LD_LIBRARY_PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/wcslib/lib:${LD_LIBRARY_PATH}

    ## 1-3 for /usr/local/lib/libgsl.dylib which is needed by Fitter1D plugin
    export LD_LIBRARY_PATH=/usr/local/lib:${LD_LIBRARY_PATH}
    ```

    2. On Mac, you can use Qt Creator build **without setting 1-1, 1-3 thing by (LD_LIBRARY_PATH)**. In Qt Creator, there is a default enabled setting which will automatically add build library search path to DYLD_LIBRARY_PATH and DYLD_FRAMEWORK_PATH (Mac, work), add build library search path to LD_LIBRARY_PATH (Linux, not work, don't know why).

    3. On Mac, you need to setup below thing (will improve later), you can copy them as `Custom Process Setup` in Qt Creator's or a shell script
    ```
    cd $CARTAWORKHOME/CARTAvis
    export CARTABUILDHOME=`pwd`

    mkdir -p $CARTABUILDHOME/build/cpp/desktop/CARTA.app/Contents/Frameworks/
    cp $CARTABUILDHOME/build/cpp/core/libcore.1.dylib $CARTABUILDHOME/build/cpp/desktop/CARTA.app/Contents/Frameworks/

    # need to rm for qt creator 4.2, otherwise when build+run together will result in core/libcore.1.dylib not able find out qwt
    rm $CARTABUILDHOME/build/cpp/core/libcore.1.dylib
    cp $CARTABUILDHOME/build/cpp/CartaLib/libCartaLib.1.dylib $CARTABUILDHOME/build/cpp/desktop/CARTA.app/Contents/Frameworks/

    install_name_tool -change qwt.framework/Versions/6/qwt $CARTABUILDHOME/ThirdParty/qwt-6.1.2/lib/qwt.framework/Versions/6/qwt $CARTABUILDHOME/build/cpp/desktop/CARTA.app/Contents/MacOS/CARTA
    install_name_tool -change qwt.framework/Versions/6/qwt $CARTABUILDHOME/ThirdParty/qwt-6.1.2/lib/qwt.framework/Versions/6/qwt $CARTABUILDHOME/build/cpp/desktop/CARTA.app/Contents/Frameworks/libcore.1.dylib

    # not sure the effect of the below line, try comment
    # install_name_tool -change libplugin.dylib $CARTABUILDHOME/build/cpp/plugins/CasaImageLoader/libplugin.dylib $CARTABUILDHOME/build/cpp/plugins/ImageStatistics/libplugin.dylib
    install_name_tool -change libcore.1.dylib  $CARTABUILDHOME/build/cpp/desktop/CARTA.app/Contents/Frameworks/libcore.1.dylib $CARTABUILDHOME/build/cpp/plugins/ImageStatistics/libplugin.dylib

    install_name_tool -change libCartaLib.1.dylib  $CARTABUILDHOME/build/cpp/desktop/CARTA.app/Contents/Frameworks/libCartaLib.1.dylib $CARTABUILDHOME/build/cpp/plugins/ImageStatistics/libplugin.dylib
    install_name_tool -change libcore.1.dylib  $CARTABUILDHOME/build/cpp/desktop/CARTA.app/Contents/Frameworks/libcore.1.dylib $CARTABUILDHOME/build/cpp/desktop/CARTA.app/Contents/MacOS/CARTA
    install_name_tool -change libCartaLib.1.dylib  $CARTABUILDHOME/build/cpp/desktop/CARTA.app/Contents/Frameworks/libCartaLib.1.dylib $CARTABUILDHOME/build/cpp/desktop/CARTA.app/Contents/MacOS/CARTA
    install_name_tool -change libCartaLib.1.dylib  $CARTABUILDHOME/build/cpp/desktop/CARTA.app/Contents/Frameworks/libCartaLib.1.dylib $CARTABUILDHOME/build/cpp/desktop/CARTA.app/Contents/Frameworks/libcore.1.dylib

    for f in `find . -name libplugin.dylib`; do install_name_tool -change libcore.1.dylib  $CARTABUILDHOME/build/cpp/desktop/CARTA.app/Contents/Frameworks/libcore.1.dylib $f; done
    for f in `find . -name libplugin.dylib`; do install_name_tool -change libCartaLib.1.dylib  $CARTABUILDHOME/build/cpp/desktop/CARTA.app/Contents/Frameworks/libCartaLib.1.dylib $f; done
    for f in `find . -name "*.dylib"`; do install_name_tool -change libwcs.5.15.dylib  $CARTABUILDHOME/ThirdParty/wcslib/lib/libwcs.5.15.dylib $f; echo $f; done
    ```

2. execute `ulimit -n 2000` before running Carta
3. To run `CARTA` binary with parameters, at least should append `html file path`, example:

```
$CARTAWORKHOME/CARTAvis/build/cpp/desktop/CARTA --html $CARTAWORKHOME/CARTAvis/carta/html5/desktop/desktopIndex.html
```

Some of optional parameters:

1. `--scriptPort 9999` for python interface
2. put `/scratch/some-fits-file.fits` in the end

## Run and Debug by Qt Creator

On Mac: Work In Progress. (The needed setting is done in https://github.com/cartavis/carta#use-qt-creator-to-build-and-debug-will-complement-this-part-later.

On Linux: Work In Progress.

# Deployment: Prepare distributable and packaged installer/zip

Use this repo, https://github.com/cartavis/deploytask.

### Some notes:

1. Start from Qt 5.5, the **rpath** of dependent Qt library will use **@rpath** instead of using absolute path, on Mac.

2. Observation about build Size (on Mac), before packaging:

```
553M    Carta.app   (without default image)
            qooxdoo (in Resources) ~ 133 MB
            QT (in Frameworks) with a size of ~ 300M
            casa libraries (in Frameworks) with ~ 60 M
            other libraries ~ 20M.  
744M    build folder (Carta.app +others) + CARTA source code +  third-party libs(+ possible source code)
1.5G    prebuilt casacore/casacode + possible source code
```

# Dynamic/Shared Library search notes for runtime and packaging

1. CARTA dekstop version is built to **CARTA program + dynamic libs (libCARTA, libcore, many libPlugin built from Qt)** and use "a few static Third-party library + many dynamic Third-party library".

2. It seems that Qt-built dynamic libs do not have **Search Path issue**, at least before moving CARTA program (for packaging).

3. We use **install_name_tool** (Mac) and **chrpath** or **PatchELF** (Linux) to specify dynamic linking path of each lib. On mac, **CARTA** is located in **CARTA.app/Contents/MacOS/desktop**  after building. Linux does not have these folder, so we need to have different handles.

# CI/CD

CirclCI (docker): https://circleci.com/gh/CARTAvis/carta

Travis CI (Mac): https://travis-ci.org/CARTAvis/carta/.
Mac auto build repo: https://goo.gl/3pRsjs.

# TODO list

https://github.com/CARTAvis/carta/wiki/todo

### AppendixA:

CARTA third party libs list:
https://docs.google.com/spreadsheets/d/1SpwEZM2n6xDGBEd6Nisn0s8KbBQG-DtoXUcFONzXXtY/edit#gid=0
