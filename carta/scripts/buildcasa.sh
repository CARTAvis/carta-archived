#!/bin/bash
export CARTAWORKHOME=`pwd`

isCentOS=true
if grep -q CentOS /etc/os-release; then
    echo "isCentOS"
else
    echo "should be Ubuntu"
	isCentOS=false
fi

TARGETOS=linux

if [ "$(uname)" == "Darwin" ]; then
    ## should do https://github.com/CARTAvis/carta/wiki/Install-Third-Party-For-CARTA-CASA-On-Mac first
    ## its qmake (homebrew) is in /usr/local/Cellar/qt/4.8.7_3/bin

    TARGETOS=darwin

    ## in this path, in include "dbuspp-xml2cpp" which will affect code's cmake:DBUS_FLAVOR_dbuspp-xml2cpp_EXECUTABLE
    export PATH=/opt/casa/02/bin:$PATH

elif [ "$isCentOS" = true ] ; then

cat > "/etc/yum.repos.d/casa.repo" <<EOF
[casa]
name=CASA RPMs for RedHat Enterprise Linux 7 (x86_64)
baseurl=http://svn.cv.nrao.edu/casa/repo/el7/x86_64
gpgkey=http://svn.cv.nrao.edu/casa/RPM-GPG-KEY-casa http://www.jpackage.org/jpackage.asc http://svn.cv.nrao.edu/casa/repo/el7/RPM-GPG-KEY-redhat-release http://svn.cv.nrao.edu/casa/repo/el7/RPM-GPG-KEY-EPEL
EOF


    # for building Qt 4.8.5,
    # On Grimmer's docker -centos 7.3 , only when buidling Carta by Qt-5.3.2, we need gstreamer-devel, gstreamer-plugins-base
    # But on Chia-Jung Hsu's pc - centos 7.3, building Qt 4.8.5 needs pkgconfig(gstreamer-app-0.10) which install gstreamer-plugins-base-devel ,
    #    also gstreamer-devel, gstreamer-plugins-base,  gstreamer
    # Qt's QtWebkit may use gstreamer libs
    sudo yum -y install 'pkgconfig(gstreamer-app-0.10)'

    ##### CentOS 7
    ## https://safe.nrao.edu/wiki/bin/view/Software/CASA/CartaBuildInstructionsForEL7
    ## To do: should spend time to minimalize them,
    ## also tell them which part is for casacore, which is for casa(code)
    ## only support CentOS 7 ??
    # sudo yum -y install devtoolset*

    #   devtoolset-3-gcc.x86_64 0:4.9.2-6.2.el7
    #   devtoolset-3-gcc-c++.x86_64 0:4.9.2-6.2.el7
    #   devtoolset-3-gcc-gfortran.x86_64 0:4.9.2-6.2.el7
    ## not sure if other libs are needed or not. seems not
    # Installed:
    #   devtoolset-3-binutils.x86_64 0:2.24-18.el7                          devtoolset-3-dwz.x86_64 0:0.11-1.1.el7                            devtoolset-3-elfutils.x86_64 0:0.161-1.el7
    #   devtoolset-3-elfutils-libelf.x86_64 0:0.161-1.el7                   devtoolset-3-elfutils-libs.x86_64 0:0.161-1.el7                   devtoolset-3-gcc.x86_64 0:4.9.2-6.2.el7
    #   devtoolset-3-gcc-c++.x86_64 0:4.9.2-6.2.el7                         devtoolset-3-gcc-gfortran.x86_64 0:4.9.2-6.2.el7                  devtoolset-3-gdb.x86_64 0:7.8.2-38.el7
    #   devtoolset-3-libquadmath-devel.x86_64 0:4.9.2-6.2.el7               devtoolset-3-libstdc++-devel.x86_64 0:4.9.2-6.2.el7               devtoolset-3-ltrace.x86_64 0:0.7.91-9.el7
    #   devtoolset-3-memstomp.x86_64 0:0.1.5-3.el7                          devtoolset-3-runtime.x86_64 0:3.1-12.el7                          devtoolset-3-strace.x86_64 0:4.8-8.el7
    #   devtoolset-3-toolchain.x86_64 0:3.1-12.el7
    #
    # Dependency Installed:
    #   audit-libs-python.x86_64 0:2.6.5-3.el7          checkpolicy.x86_64 0:2.5-4.el7                    glibc-devel.x86_64 0:2.17-157.el7_3.1          glibc-headers.x86_64 0:2.17-157.el7_3.1
    #   kernel-headers.x86_64 0:3.10.0-514.6.1.el7      libcgroup.x86_64 0:0.41-11.el7                    libgomp.x86_64 0:4.8.5-11.el7                  libmpc.x86_64 0:1.0.1-3.el7
    #   libselinux-python.x86_64 0:2.5-6.el7            libselinux-utils.x86_64 0:2.5-6.el7               libsemanage-python.x86_64 0:2.5-5.1.el7_3      mpfr.x86_64 0:3.1.1-4.el7
    #   policycoreutils.x86_64 0:2.5-11.el7_3           policycoreutils-python.x86_64 0:2.5-11.el7_3      python-IPy.noarch 0:0.75-6.el7                 scl-utils.x86_64 0:20130529-17.el7_1
    #   setools-libs.x86_64 0:3.3.8-1.1.el7
    #
    # Dependency Updated:
    #   libsemanage.x86_64 0:2.5-5.1.el7_3
    ####
    # Xerces-C++ is a validating XML parser

    ## need to check if we really need this to install casa01-mpi4py, casa01-openmpi
    ## https://safe.nrao.edu/wiki/bin/view/Software/CASA/CartaBuildInstructionsForEL7

    ##  casa01-python-tools seems to have python-numpy, boost-python

    sudo yum -y install \
    python-devel boost-python \
    # casa01-mpi4py.x86_64 casa01-openmpi.x86_64 \
    # casa01-python.x86_64 casa01-python-devel.x86_64, casa01-python-tools.x86_64 \ (2.7.5)
    lapack-devel xerces-c-devel

    # sudo yum -y install wcslib wcslib-devel

    ## casacore needs. not sure if code needs or not
    ## casacore refernece: https://github.com/casacore/casacore
    ## the above link shows. (optional) yum -y install ncurses ncurses-devel, but it is default when building on Ubuntu?
    # sudo yum -y install gcc-gfortran ## 4.8.5, ast also needs this
    sudo yum -y install boost
    sudo yum -y install boost-devel
    sudo yum -y install numpy
    sudo yum -y install fftw fftw-devel gsl gsl-devel

    ### (casa)code needs
    ## when config casa-code: -- casa_packages=/usr/lib64/casapy (second), but not exist now
    sudo yum -y install java
    sudo yum -y install libxml2-devel libxslt-devel
    sudo yum -y install rpfits readline-devel
    # sudo yum -y install casa01-dbus-cpp casa01-dbus-cpp-devel ## for Qt d-bus (QtDbus)
    sudo yum -y install dbus dbus-c++ dbus-c++-devel
    sudo yum -y install libsakura # /opt/casa/01/
    sudo yum -y install pgplot-devel pgplot-demos pgplot-motif # , 5.3.1

    #### install Qt 4.8.5
    sudo yum -y install qt-devel.x86_64
    alias qmake='qmake-qt4'
else
	##### Ubuntu 16.04
    ## can use sudo aptitude search to search packages
    ## another ref: https://github.com/casacore/casacore/blob/master/.travis.yml

    ####### casacore part:
    # sudo apt-get -y install libwcs5 wcslib-dev
    sudo apt-get -y install gfortran python-numpy libfftw3-dev liblapacke-dev
    sudo apt-get -y install libboost-dev
    # sudo apt-get -y install libblas-dev libblas3 \ move to install3party since gsl needs too
    liblapack-dev liblapack3 liblapacke liblapacke-dev

    sudo apt-get -y install libgsl-dev
    sudo apt-get -y install libxerces-c-dev
    sudo apt-get -y install libncurses5-dev ## do not know why CentOS 7 does not need to install this
    sudo apt-get -y install libboost-python-dev

    ## forget the below are for casacore or casa-code (<-seems this)
    sudo apt-get -y install libboost-regex-dev libboost-program-options-dev \
    ibboost-thread-dev libboost-serialization-dev libboost-filesystem-dev libboost-system-dev
    #######

    ####### casa-code part:
    ## needed to be installed before building qt 4.8, otherwise Qt d-bus (QtDbus) moduble will not be built in
    ## not sure if all are needed or not
    sudo apt-get -y install libdbus-1-dev \
    libqt4-dbus libqtdbus4 dbus-cpp-dev libdbus-c++-dev libdbus-cpp-dev

    sudo apt-get -y install default-jre ## openjdk-7-jdk, openjdk-7-jre
    sudo apt-get -y install libxslt1-dev

    sudo apt-get -y install libreadline-dev libxml2-dev

    ## https://launchpad.net/~radio-astro/+archive/ubuntu/main
    # sudo add-apt-repository -s ppa:radio-astro/main # seems this repo having pgplot, too
    sudo add-apt-repository -s ppa:kernsuite/kern-1
    sudo apt-get update
    sudo apt-get -y install rpfits

    sudo apt-get install software-properties-common
    sudo apt-add-repository multiverse
    sudo apt-get update
    sudo apt-get -y install pgplot5 ## needed for casa-code, ubuntu 15.04 does not find it

    ## not sure if this is in multiverse repo
    sudo apt-get -y install libpgsbox-dev

    ####### there is no-prebuilt libsakura for ubuntu, needed for casa-code
    ## http://alma-intweb.mtk.nao.ac.jp/~sakura/api/html/INSTALL.txt
    apt-get install doxygen
    # apt-get install gtest
    apt-get install libeigen3-dev
    apt-get install liblog4cxx10v5

    #wget ftp://alma-dl.mtk.nao.ac.jp/sakura/releases/latest_src/libsakura-4.0.2065.tar.gz
    ## tar -xvzf libsakura-4.0.2065.tar.gz
    ## Use our own modified version,
    ## since the original source code will not be compiled OK by gcc 5.4, gcc 4.8 seem neither
    git clone https://github.com/grimmer0125/libsakura

    wget -O gtest-1.7.0.zip https://github.com/google/googletest/archive/release-1.7.0.zip
    unzip gtest-1.7.0.zip -d libsakura
    cd libsakura
    ln -s googletest-release-1.7.0 gtest
    mkdir build
    cd build
    cmake ..
    make
    make apidoc
    make install ## default: /usr/local
    ####### casa-code part

    ##### Qt 4.8.5 way1:  Build (slow)
    #wget https://download.qt.io/archive/qt/4.8/4.8.5/qt-everywhere-opensource-src-4.8.5.zip
    #unzip -a qt-everywhere-opensource-src-4.8.5.zip # -d $CARTAWORKHOME/CARTAvis-externals/ThirdParty/Qt4.8.5
    ## cd $CARTAWORKHOME/CARTAvis-externals/ThirdParty/Qt4.8.5/qt-everywhere-opensource-src-4.8.5
    #cd qt-everywhere-opensource-src-4.8.5
    #printf 'yes\n' | ./configure -v -opensource -dbus-linked
    #make
    #make install
    #export PATH=/usr/local/Trolltech/Qt-4.8.5/bin:$PATH
    #######

    # ./configure --prefix $CARTAWORKHOME/CARTAvis-externals/ThirdParty/Qt4.8.5 -> fail
    #./configure # some interactive questioin. "o", "yes" !!

    ##### TODO: way2, to install Qt 4.8.7, use qmake or qmake-qt4. Not use this way to test all.
    sudo apt-get install libqt4-dev
fi

# can try to use tools to answer stdin questions automatically
# http://askubuntu.com/questions/338857/automatically-enter-input-in-command-line
# http://stackoverflow.com/questions/14392525/passing-arguments-to-an-interactive-program-non-interactively
# http://stackoverflow.com/questions/4857702/how-to-provide-password-to-a-command-that-prompts-for-one-in-bash
# or use sudo yum to insatll ? qt-4.8.6-30.fc20.x86_64

## for building qwt for casa-submodue-code, by using Qt4.8.5
cd $CARTAWORKHOME/CARTAvis-externals/ThirdParty
curl -O -L http://downloads.sourceforge.net/project/qwt/qwt/6.1.0/qwt-6.1.0.tar.bz2
tar xvfj qwt-6.1.0.tar.bz2 && mv qwt-6.1.0 qwt-6.1.0-src
cd qwt-6.1.0-src # can use qwt 6.1.3 Pavol uses
# for unix part
if [ "$(uname)" == "Darwin" ]; then
    perl -i -pe 's/.*/ QWT_INSTALL_PREFIX    = $ENV{CARTAWORKHOME}\/CARTAvis-externals\/ThirdParty\/qwt-6.1.0\/ / if $.==22' qwtconfig.pri
else
    sed -i "22,22c QWT_INSTALL_PREFIX    = $CARTAWORKHOME/CARTAvis-externals/ThirdParty/qwt-6.1.0" qwtconfig.pri
fi
qmake qwt.pro
make && make install
# export PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/qwt-6.1.0/include:$PATH
# export PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/qwt-6.1.0/lib:$PATH
cd ..

## cascore and code
# in the other instruction to build carta + casa, usually
# casa
# cartabuild(CARTAWORKHOME) / CARTAvis(carta, git root folder)
# but we put casa inside $CARTAWORKHOME/CARTAvis-externals/ThirdParty
mkdir casa
cd casa
# it seems that its svn external link, casa submodule - casacore will checkout its latest one
svn co --ignore-externals -r 38314 https://svn.cv.nrao.edu/svn/casa/trunk
cd trunk

# may switch to use git clone, https://safe.nrao.edu/wiki/bin/view/Software/CASA/CasaBuildInstructions
# git clone https://github.com/casacore/casacore.git
# git checkout 5f4ffede19d6dbeb296e4db2ebe467db842e7b46 (= svn 105506, 20160919, can not find 105507 on Svn client now)
# no need to rename for git way
svn co -r 105507 https://github.com/casacore/casacore/trunk
mv trunk casacore

############################ casacore

cd casacore
mkdir build && cd build

# https://sites.google.com/a/asiaa.sinica.edu.tw/acdc/bui/centos7-2,
# maybe this is due to the above reason, required by latest versoin of casacore
# export PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/cfitsio/include:$PATH \
# export PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/cfitsio/lib:$PATH

###  two more official build refernece
# https://github.com/casacore/casacore
# https://github.com/casacore/casacore/wiki/CmakeInstructions
###

## it is better to rm -rf * in build folder if rebuild manually + dependency changes
## can use your own compiler and gfortan here
## use $CARTAWORKHOME may be better for -DCMAKE_INSTALL_PREFIX=../../linux
## After checking, /usr/lib64/casa/01 does not exist but build OK

if [ "$(uname)" == "Darwin" ]; then
    cmake -DBoost_NO_BOOST_CMAKE=1 -DCASA_BUILD=1 \
    -DCMAKE_Fortran_COMPILER=/usr/local/Cellar/gcc/6.3.0_1/bin/gfortran-6 \
    -DPYTHON_LIBRARY=/System/Library/Frameworks/Python.framework/Versions/2.7/Python \
    -DWCSLIB_ROOT_DIR=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/wcslib \
    -DCFITSIO_ROOT_DIR=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/cfitsio \
    -DBOOST_ROOT=/usr/local/Cellar/boost/1.63.0 \
    -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
    -DCMAKE_C_COMPILER=/usr/bin/clang \
    -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF \
    -DCMAKE_INSTALL_PREFIX=../../$TARGETOS -DBUILD_PYTHON=1 -DCXX11=1 ..
elif [ "$isCentOS" = true ] ; then
    cmake -DBoost_NO_BOOST_CMAKE=1 -DCASA_BUILD=1 -DBUILD_TESTING=OFF \
    -DCMAKE_INSTALL_PREFIX=../../$TARGETOS -DBUILD_PYTHON=1 \
    -DPYTHON_INCLUDE_DIR=/usr/include/python2.7 \
    -DPYTHON_LIBRARY=/usr/lib64/libpython2.7.so \
    -DWCSLIB_ROOT_DIR=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/wcslib \
    -DCFITSIO_ROOT_DIR=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/cfitsio \
    -DCMAKE_BUILD_TYPE=Release \
    -DCXX11=1 ..
    # -DPYTHON_LIBRARY=/opt/casa/01/lib/libpython2.7.so \ ??
    # -DPYTHON_INCLUDE_DIR=/opt/casa/01/include/python2.7/ \
    # -DCMAKE_CXX_COMPILER=/opt/rh/devtoolset-3/root/usr/bin/g++ \
    # -DCMAKE_C_COMPILER=/opt/rh/devtoolset-3/root/usr/bin/gcc \
    # -DCMAKE_Fortran_COMPILER=/opt/rh/devtoolset-3/root/usr/bin/gfortran ..
else
    # https://safe.nrao.edu/wiki/bin/view/Software/CASA/CartaBuildInstructionsForUbuntu
    # -DCMAKE_PREFIX_PATH=/media/workdrive/CARTA/CARTAvis-externals/ThirdParty/wcslib
    cmake -DBoost_NO_BOOST_CMAKE=1 -DCASA_BUILD=1 -DBUILD_TESTING=OFF \
    -DCMAKE_INSTALL_PREFIX=../../$TARGETOS -DBUILD_PYTHON=1 \
    -DCMAKE_BUILD_TYPE=Release -DCXX11=1 ..
fi

### Mac part:
# -DCMAKE_Fortran_COMPILER=/opt/casa/02/bin/gfortran-mp-5 \ ?
# -DPYTHON_LIBRARY=/opt/casa/02/lib/libpython2.7.dylib \ ?
## DUseCrashReporter seems to be not used by casacore
# cmake -DBoost_NO_BOOST_CMAKE=1 -DCASA_BUILD=1 \
# -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
# -DCMAKE_C_COMPILER=/usr/bin/clang \
# -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF \
# -DCMAKE_INSTALL_PREFIX=../../darwin -DBUILD_PYTHON=1 -DCXX11=1 ..
###

# it may build fail due to no official parallel build support of casa
make -j2
make install

############################ casa-submodule: code/imageanalysis
cd ../../code

# Apply https://safe.nrao.edu/wiki/pub/Software/CASA/CartaBuildInstructionsForUbuntu/imageanalysisonubuntu.diff
# to Code to build imageanalysis only. Reduce build time a lot !!!!!
curl -O http://www.asiaa.sinica.edu.tw/~tckang/casa/casacodereduce1.diff
svn patch casacodereduce1.diff

## if no insert NO_LINK, casa will try to use the libraries to run and will not find out their shared libraries,
# since no more yum/apt version which is easily searchable without using rpath or LD_LIBRARY_PATH.
perl -pi -e '$_ .= qq(NO_LINK\n) if /casa_find\( WCSLIB/' CMakeLists.txt
perl -pi -e '$_ .= qq(NO_LINK\n) if /casa_find\( CASACORE/' CMakeLists.txt

perl -pi.bak -e 's/QtGui QtDBus QtXml/QtGui QtXml/g' CMakeLists.txt
sed 's/.*casa_add_module( graphics/#&/' CMakeLists.txt
sed 's/.*casa_add_module( atmosphere/#&/' CMakeLists.txt
sed 's/.*casa_add_module( parallel/#&/' CMakeLists.txt
sed 's/.*casa_add_module( casadbus/#&/' CMakeLists.txt

mkdir build && cd build

# -DEXTRA_C_FLAGS=-DPG_PPU -I/opt/casa/02/include/wcslib

## it is better to rm -rf * in build folder if rebuild manually + dependency changes
## DSKIP_PGPLOT is used by display, qtviewer/guitools?, or some imageanalysis/test
if [ "$(uname)" == "Darwin" ]; then
    cmake -DUseCrashReporter=0 -DBoost_NO_BOOST_CMAKE=1 \
    -Darch=$TARGETOS -DCMAKE_BUILD_TYPE=Release -DCXX11=1 \
    -DWCSLIB_ROOT_DIR=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/wcslib \
    -DCFITSIO_ROOT_DIR=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/cfitsio \
    -DREADLINE_ROOT_DIR=/usr/local/opt/readline \
    -DPGPLOT_ROOT_DIR=/usr/local/opt/pgplot \
    -DPGPLOT_INCLUDE_DIRS=/usr/local/opt/pgplot/include \
    -DPGPLOT_LIBRARIES=/usr/local/opt/pgplot/lib \
    -DSKIP_PGPLOT=1 \
    -DLIBXML2_ROOT_DIR=/usr/local/opt/libxml2 \
    -DQWT_ROOT_DIR=/usr/local/opt/qwt-qt4 \
    -DXERCES_ROOT_DIR=/usr/local/opt/xerces-c \
    -DRPFITS_ROOT_DIR=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/rpfits \
    -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
    -DCMAKE_C_COMPILER=/usr/bin/clang \
    -DLLVMCOMPILER=1 \
    -DINTERACTIVE_ITERATION=1  ..
elif [ "$isCentOS" = true ] ; then
    cmake -DUseCrashReporter=0 -DBoost_NO_BOOST_CMAKE=1 -DEXTRA_C_FLAGS=-DPG_PPU \
    -DWCSLIB_ROOT_DIR=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/wcslib \
    -DCFITSIO_ROOT_DIR=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/cfitsio \
    -DQWT_ROOT_DIR=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/qwt-6.1.0 \
    -DLIBSAKURA_ROOT_DIR=/opt/casa/01/lib/libsakura/default/ \
    -Darch=$TARGETOS -DCMAKE_BUILD_TYPE=Release ..
    ## -DCXX11=1
    # -DCMAKE_CXX_COMPILER=/opt/rh/devtoolset-3/root/usr/bin/g++ \
    # -DCMAKE_C_COMPILER=/opt/rh/devtoolset-3/root/usr/bin/gcc \
    # -DCMAKE_Fortran_COMPILER=/opt/rh/devtoolset-3/root/usr/bin/gfortran ..
else
    # -DWCSLIB_ROOT_DIR=/media/workdrive/CARTA/CARTAvis-externals/ThirdParty/wcslib
    cmake -DUseCrashReporter=0 -DBoost_NO_BOOST_CMAKE=1 '-DEXTRA_C_FLAGS=-DPG_PPU' \
    -DWCSLIB_ROOT_DIR=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/wcslib \
    -DCFITSIO_ROOT_DIR=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/cfitsio \
    -DQWT_ROOT_DIR=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/qwt-6.1.0 \
    -DLIBSAKURA_ROOT_DIR=/opt/casa/01/lib/libsakura/default/ \
    -Darch=$TARGETOS -DCMAKE_BUILD_TYPE=Release ..
fi

# -DCMAKE_Fortran_COMPILER=/opt/casa/02/bin/gfortran-mp-5 \
### Mac part
# cmake -DUseCrashReporter=0 -DBoost_NO_BOOST_CMAKE=1 \
# -Darch=darwin -DCMAKE_BUILD_TYPE=Release -DCXX11=1 \
# -DWCSLIB_ROOT_DIR=/usr/local/Cellar/wcslib/5.16_1 \
# -DREADLINE_ROOT_DIR=/usr/local/opt/readline \
# -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
# -DCMAKE_C_COMPILER=/usr/bin/clang \
# -DLLVMCOMPILER=1 \
# -DINTERACTIVE_ITERATION=1 -Dpgplot_ext=_nox ..
# # -DCMAKE_INSTALL_PREFIX=../../darwin
###

# https://safe.nrao.edu/wiki/bin/view/Software/CASA/CartaBuildInstructionsForUbuntu
# cmake -DUseCrashReporter=0 -DBoost_NO_BOOST_CMAKE=1 '-DEXTRA_C_FLAGS=-DPG_PPU' \
# -Darch=linux -DCMAKE_BUILD_TYPE=Release \
# -DCXX11=1 -DWCSLIB_ROOT_DIR=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/wcslib ..

# it may build fail due to no official parallel build support of casa
make -j2

# Ville said code does not need make install
# make install

cd $CARTAWORKHOME/CARTAvis-externals/ThirdParty
mkdir imageanalysis
cd imageanalysis
ln -s $CARTAWORKHOME/CARTAvis-externals/ThirdParty/casa/trunk/$TARGETOS/include/casacode/ include
ln -s $CARTAWORKHOME/CARTAvis-externals/ThirdParty/casa/trunk/$TARGETOS/lib lib

cd ..
mkdir casacore
cd casacore
ln -s $CARTAWORKHOME/CARTAvis-externals/ThirdParty/casa/trunk/$TARGETOS/include/ include
ln -s $CARTAWORKHOME/CARTAvis-externals/ThirdParty/casa/trunk/$TARGETOS/lib lib

if [ "$isCentOS" = true ] ; then
    unalias qmake
}
