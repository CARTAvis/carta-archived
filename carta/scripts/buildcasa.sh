#!/bin/bash
CARTAWORKHOME=`pwd`

isCentOS=true
if grep -q CentOS /etc/os-release; then
    echo "isCentOS"
else
    echo "should be Ubuntu"
	isCentOS=false
fi

## will put into the below condition, now somehow it shows some script wrong if in condition
cat > "/etc/yum.repos.d/casa.repo" <<EOF
[casa]
name=CASA RPMs for RedHat Enterprise Linux 7 (x86_64)
baseurl=http://svn.cv.nrao.edu/casa/repo/el7/x86_64
gpgkey=http://svn.cv.nrao.edu/casa/RPM-GPG-KEY-casa http://www.jpackage.org/jpackage.asc http://svn.cv.nrao.edu/casa/repo/el7/RPM-GPG-KEY-redhat-release http://svn.cv.nrao.edu/casa/repo/el7/RPM-GPG-KEY-EPEL
EOF

if [ "$isCentOS" = true ] ; then
    ##### CentOS 7
    ## https://safe.nrao.edu/wiki/bin/view/Software/CASA/CartaBuildInstructionsForEL7
    ## To do: should spend time to minimalize them,
    ## also tell them which part is for casacore, which is for casa(code)
    sudo yum -y install devtoolset*
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

    ## need to check if we really need this to install casa01-openmpi etc
    ## https://safe.nrao.edu/wiki/bin/view/Software/CASA/CartaBuildInstructionsForEL7

    sudo yum -y install casa01-dbus-cpp casa01-dbus-cpp-devel casa01-mpi4py.x86_64 \
    casa01-openmpi.x86_64 casa01-python.x86_64 casa01-python-devel.x86_64 casa01-python-tools.x86_64 \
    libsakura pgplot-devel pgplot-demos pgplot-motif \
    lapack-devel xerces-c-devel

    ## casacore needs. not sure if code needs or not
    ## casacore refernece: https://github.com/casacore/casacore
    sudo yum -y install gcc-gfortran ## 4.8.5, ast also needs this
    sudo yum -y install boost
    sudo yum -y install boost-devel
    sudo yum -y install numpy
    sudo yum -y install fftw fftw-devel gsl gsl-devel

    ### (casa)code needs
    sudo yum -y install java
    sudo yum -y install libxml2-devel libxslt-devel
    sudo yum -y install rpfits readline-devel
else
	##### Ubuntu 16.04
    sudo apt-get -y install gfortran python-numpy libfftw3-dev liblapacke-dev
    sudo apt-get -y install libgsl-dev
    ## libsakura, pgplot ?
    ## blas: mentioned in github casacore
    sudo apt-get -y install libboost-dev
    sudo apt-get -y install libdbus-1-dev
    sudo apt-get -y install libxerces-c-dev
    sudo apt-get -y install libblas-dev libblas3 \
    liblapack-dev liblapack3 liblapacke liblapacke-dev
    sudo apt-get -y install default-jre
    sudo apt-get -y install libxslt1-dev
    sudo apt-get -y install libboost-python-dev
    sudo apt-get -y install libboost-regex-dev libboost-program-options-dev \
    ibboost-thread-dev libboost-serialization-dev libboost-filesystem-dev libboost-system-dev
    ## openjdk-7-jdk, openjdk-7-jre
fi


## Build Qt 4.8.5 (slow)
wget https://download.qt.io/archive/qt/4.8/4.8.5/qt-everywhere-opensource-src-4.8.5.zip
unzip -a qt-everywhere-opensource-src-4.8.5.zip # -d $CARTAWORKHOME/CARTAvis-externals/ThirdParty/Qt4.8.5
# cd $CARTAWORKHOME/CARTAvis-externals/ThirdParty/Qt4.8.5/qt-everywhere-opensource-src-4.8.5
cd qt-everywhere-opensource-src-4.8.5
# ./configure --prefix $CARTAWORKHOME/CARTAvis-externals/ThirdParty/Qt4.8.5 -> fail
#./configure # some interactive questioin. "o", "yes" !!


if [ "$isCentOS" = true ] ; then
    printf 'o\nyes\n' | ./configure
    gmake
    gmake install # /usr/local/Trolltech/Qt-4.8.5/
else
    printf 'yes\n' | ./configure -v -opensource -dbus-linked
    make
    make install
fi

# can try to use tools to answer stdin questions automatically
# http://askubuntu.com/questions/338857/automatically-enter-input-in-command-line
# http://stackoverflow.com/questions/14392525/passing-arguments-to-an-interactive-program-non-interactively
# http://stackoverflow.com/questions/4857702/how-to-provide-password-to-a-command-that-prompts-for-one-in-bash
# or use sudo yum to insatll ? qt-4.8.6-30.fc20.x86_64

export PATH=/usr/local/Trolltech/Qt-4.8.5/bin:$PATH

## for building qwt for casa-submodue-code, by using Qt4.8.5
cd $CARTAWORKHOME/CARTAvis-externals/ThirdParty
curl -O -L http://downloads.sourceforge.net/project/qwt/qwt/6.1.0/qwt-6.1.0.tar.bz2
tar xvfj qwt-6.1.0.tar.bz2 && mv qwt-6.1.0 qwt-6.1.0-src
cd qwt-6.1.0-src # can use qwt 6.1.3 Pavol uses
# for unix part
sed -i "22,22c QWT_INSTALL_PREFIX    = $CARTAWORKHOME/CARTAvis-externals/ThirdParty/qwt-6.1.0" qwtconfig.pri
qmake qwt.pro
make && make install
export PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/qwt-6.1.0/include:$PATH
export PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/qwt-6.1.0/lib:$PATH
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

cd casacore
mkdir build && cd build

# https://sites.google.com/a/asiaa.sinica.edu.tw/acdc/bui/centos7-2,
# maybe this is due to the above reason, required by latest versoin of casacore
export PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/cfitsio/include:$PATH
export PATH=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/cfitsio/lib:$PATH

###  two more official build refernece
# https://github.com/casacore/casacore
# https://github.com/casacore/casacore/wiki/CmakeInstructions
###

## it is better to rm -rf * in build folder if rebuild manually + dependency changes
## can use your own compiler and gfortan here
if [ "$isCentOS" = true ] ; then
    cmake -DUseCrashReporter=0 -DBoost_NO_BOOST_CMAKE=1 -DCASA_BUILD=1 -DBUILD_TESTING=OFF \
    -DCMAKE_INSTALL_PREFIX=../../linux -DBUILD_PYTHON=1 \
    -DPYTHON_INCLUDE_DIR=/opt/casa/01/include/python2.7/ \
    -DPYTHON_LIBRARY=/opt/casa/01/lib/libpython2.7.so \
    -DBOOST_ROOT=/usr/lib64/casa/01 -DCMAKE_BUILD_TYPE=Release \
    -DCXX11=1
    -DCMAKE_CXX_COMPILER=/opt/rh/devtoolset-3/root/usr/bin/g++ \
    -DCMAKE_C_COMPILER=/opt/rh/devtoolset-3/root/usr/bin/gcc \
    -DCMAKE_Fortran_COMPILER=/opt/rh/devtoolset-3/root/usr/bin/gfortran ..
else
    cmake -DUseCrashReporter=0 -DBoost_NO_BOOST_CMAKE=1 -DCASA_BUILD=1 -DBUILD_TESTING=OFF \
    -DCMAKE_INSTALL_PREFIX=../../linux -DBUILD_PYTHON=1 \
    -DCMAKE_BUILD_TYPE=Release -DCXX11=1 ..
fi


# it may build fail due to no official parallel build support of casa
make -j 2
make install

#### casa-submodule: code/imageanalysis
cd ../../code
mkdir build && cd build

## To do: try this later
# Apply https://safe.nrao.edu/wiki/pub/Software/CASA/CartaBuildInstructionsForUbuntu/imageanalysisonubuntu.diff
# to Code to build imageanalysis only

## it is better to rm -rf * in build folder if rebuild manually + dependency changes
if [ "$isCentOS" = true ] ; then
    cmake -DUseCrashReporter=0 -DBoost_NO_BOOST_CMAKE=1 '-DEXTRA_C_FLAGS=-DPG_PPU -I/usr/include/wcslib' \
    -Darch=linux -DCMAKE_BUILD_TYPE=Release -DCXX11=1 \
    -DCMAKE_CXX_COMPILER=/opt/rh/devtoolset-3/root/usr/bin/g++ \
    -DCMAKE_C_COMPILER=/opt/rh/devtoolset-3/root/usr/bin/gcc \
    -DCMAKE_Fortran_COMPILER=/opt/rh/devtoolset-3/root/usr/bin/gfortran ..
else
    ## -DWCSLIB_ROOT_DIR
    cmake -DUseCrashReporter=0 -DBoost_NO_BOOST_CMAKE=1 '-DEXTRA_C_FLAGS=-DPG_PPU -I/usr/include/wcslib' \
    -Darch=linux -DCMAKE_BUILD_TYPE=Release \
    -DCXX11=1 ..
fi

# cmake -DUseCrashReporter=0 -DBoost_NO_BOOST_CMAKE=1 '-DEXTRA_C_FLAGS=-DPG_PPU' \
# -Darch=linux -DCMAKE_BUILD_TYPE=Release \
# -DCXX11=1 -DWCSLIB_ROOT_DIR=$CARTAWORKHOME/CARTAvis-externals/ThirdParty/wcslib ..

# it may build fail due to no official parallel build support of casa
make -j 2
make install

cd $CARTAWORKHOME/CARTAvis-externals/ThirdParty
mkdir imageanalysis
cd imageanalysis
ln -s $CARTAWORKHOME/CARTAvis-externals/ThirdParty/casa/trunk/linux/include/casacode/ include
ln -s $CARTAWORKHOME/CARTAvis-externals/ThirdParty/casa/trunk/linux/lib lib

cd ..
mkdir casacore
cd casacore
ln -s $CARTAWORKHOME/CARTAvis-externals/ThirdParty/casa/trunk/linux/include/ include
ln -s $CARTAWORKHOME/CARTAvis-externals/ThirdParty/casa/trunk/linux/lib lib
