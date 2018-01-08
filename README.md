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
2. Ubuntu 14.04, Ubuntu 16.04 and Ubuntu 17.04
3. Mac 10.11, 10.12, 10.13

Supported deployment platform:
1. CentOS 6, 7
2. Ubuntu
3. Mac: OS X El Capitan (10.11), macOS Sierra (10.12)

Tested c++ compiler: gcc 4.8.5, 5.4 (used by Ubuntu 16.04) & clang on macOS.

CARTA can be built by Qt 5.3, 5.4, 5.5, 5.6~5.8. Start from 5.6,  we need to install QtWebKit & QtWebKit additionally.

# Build CARTA

Follow https://github.com/CARTAvis/carta/wiki#build-carta.

# Run CARTA

Need to prepare some things needed for running CARTA and also appended parameters.

### requirement 1: create needed folders

cd `your-carta-work`,, execute `./CARTAvis/carta/scripts/setupcartavis.sh`.

It is optional. You do not need to setup this and can use CARTA smoothly. But not sure if `snaptshot` function of CARTA will work OK without setup this.

There are two things you need to know first.
1. The folder structure and why you need to choose a `your-carta-work`, take a look here, https://github.com/CARTAvis/carta/wiki/build#step2---choose-your-working-space-folder-of-carta-and-clone-source-code-of-carta.

2. Due to the history issue, so you should rename this git project folder from `carta` to `CARTAvis`. Therefore you can executee `git clone https://github.com/CARTAvis/carta.git CARTAvis`.

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

### requirement 4: check if the foldler using by dbPath exist.

The default location is `$(HOME)/CARTA/cache/pcache.sqlite` so create `$(HOME)/CARTA/cache` if it does not exist.

### (optional) modify config.json to overwrite the default embedded setting

Create/Modify the following data to be the content of `~/.cartavis/config.json`

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
            "dbPath": "$(HOME)/CARTA/cache/pcache.sqlite"
        }
    }
}
```

`$(APPDIR)/../plugins` is for Linux. `"$(APPDIR)/../../../../plugins"` is for Mac.

You can browse more detailed instruction about these parameters from here,
http://cartaserver.ddns.net/docs/html/developer/contribute/Writinganimageplugin.html#appendix-e-carta-config-file


## Run by command line

1. Current CARTA needs to execute the following command every time to find correct **dynamic/shared Library** before running CARTA. Will improve later by using `rpath`.

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

    3. On Mac, you need to setup below thing, you can copy them as a shell script. [Caution: the below script is setup when build folder is choosed in CARTAvis]
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

2. execute `ulimit -n 2000` before running CARTA
3. To run `CARTA` binary with parameters, at least should append `html file path`, example:

```
$CARTAWORKHOME/CARTAvis/build/cpp/desktop/CARTA --html $CARTAWORKHOME/CARTAvis/carta/html5/desktop/desktopIndex.html
```

Some of optional parameters:

1. `--scriptPort 9999` for python interface
2. put `/scratch/some-fits-file.fits` in the end

## Run and Debug by Qt Creator

Ref: https://github.com/CARTAvis/carta/wiki/build#use-qt-creator-to-build-and-debug-will-complement-this-part-later

# Deployment: Prepare distributable and packaged installer/zip

Use this repo, https://github.com/cartavis/deploytask.

# Third Party Libraries

| Third-party Libraries | Version | license |
| :--- | :--- | :--- |
| casacore | 2.3+ | GPLv2 |
| casa | 5.0.0+ | GPLv2 |
| ast | 8.4.0 | LGPLv3 |
| gfortran |  4.8+ | GPLv3 |
| WCSLIB | 5.15 | LGPLv3 |
| CFITSIO | 3.39 | [link \(NASA license\)](https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/f_user/node9.html) |
| GSL | 2.3 | GPLv3 |
| flex | 2.5.37 | [link \(flex license\)](https://raw.githubusercontent.com/westes/flex/master/COPYING) |
| RapidJSON | 1.02 | MIT |
| qooxdoo | 3.5.1 | MIT |
| Qt | 5.3+ | LGPLv3 |
| Qwt | 6.1.2 | [link \(Qwt license\)](http://qwt.sourceforge.net/qwtlicense.html) |
| Python \(included for CentOS 6\) | 2.7 | [link \(Python license\)](https://www.python.org/download/releases/2.7/license/) |
| NumPy \(Included for CentOS 6\) |  | BSD |
| Matplotlib \(Included for CentOS 6\) |  | [link \(Matplotlib license\)](https://matplotlib.org/devel/license.html) |
| Cython \(Included for CentOS 6\) | | Apache |
| libstdc++ \(Included for CentOS 6\) | 4.8.1+ | [GCC Runtime Exception](https://www.gnu.org/licenses/gcc-exception-3.1-faq.html) |

# Dynamic/Shared Library search notes for runtime and packaging

1. On Mac, start from Qt 5.5, the **rpath** of dependent Qt library will use **@rpath** instead of using absolute path.

2. CARTA desktop version is built to **CARTA program + dynamic libs (libCARTA, libcore, many libPlugin built from Qt)** and use "a few static Third-party library + many dynamic Third-party library".

3. It seems that Qt-built dynamic libs do not have **Search Path issue**, at least before moving CARTA program (for packaging).

4. We use **install_name_tool** (Mac) and **chrpath** or **PatchELF** (Linux) to specify dynamic linking path of each lib. On mac, **CARTA** is located in **CARTA.app/Contents/MacOS/desktop**  after building. Linux does not have these folder, so we need to have different handles.

# CI/CD

CirclCI (docker): https://circleci.com/gh/CARTAvis/carta

Travis CI (Mac): https://travis-ci.org/CARTAvis/carta/.
Mac auto build repo: https://goo.gl/3pRsjs.

Introduction to build Server ver. of CARTA Viewer on Linux
=======
https://github.com/CARTAvis/carta/wiki/Server-version-of-CARTA
