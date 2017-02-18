CARTAWORKHOME=`pwd`

## do not need this key? nrao-carta does not mention but nrao-casa mention
# curl -O -L  https://dl.fedoraproject.org/pub/epel/RPM-GPG-KEY-EPEL-7
# mv RPM-GPG-KEY-EPEL-7  /etc/pki/rpm-gpg/RPM-GPG-KEY-EPEL-7

## need to check if we really need this
## https://safe.nrao.edu/wiki/bin/view/Software/CASA/CartaBuildInstructionsForEL7
cat > "/etc/yum.repos.d/casa.repo" <<EOF
[casa]
name=CASA RPMs for RedHat Enterprise Linux 7 (x86_64)
baseurl=http://svn.cv.nrao.edu/casa/repo/el7/x86_64
gpgkey=http://svn.cv.nrao.edu/casa/RPM-GPG-KEY-casa http://www.jpackage.org/jpackage.asc http://svn.cv.nrao.edu/casa/repo/el7/RPM-GPG-KEY-redhat-release http://svn.cv.nrao.edu/casa/repo/el7/RPM-GPG-KEY-EPEL
EOF

## Build Qt 4.8.5 (slow, need improvement)
wget https://download.qt.io/archive/qt/4.8/4.8.5/qt-everywhere-opensource-src-4.8.5.zip
# sudo yum -y install unzip
# not test but should work
unzip -a qt-everywhere-opensource-src-4.8.5.zip -d $CARTAWORKHOME/CARTAvis-externals/ThirdParty/Qt4.8.5
cd $CARTAWORKHOME/CARTAvis-externals/ThirdParty/Qt4.8.5/qt-everywhere-opensource-src-4.8.5
# ./configure --prefix $CARTAWORKHOME/CARTAvis-externals/ThirdParty/Qt4.8.5 -> fail
./configure # some interactive questioin. "o", "yes" !!
printf 'o\nyes\n' | ./configure
gmake
gmake install # /usr/local/Trolltech/Qt-4.8.5/

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

# sudo yum -y install cmake
sudo yum -y install boost
sudo yum -y install boost-devel
sudo yum -y install numpy

## https://safe.nrao.edu/wiki/bin/view/Software/CASA/CartaBuildInstructionsForEL7
# Xerces-C++ is a validating XML parser
sudo yum -y install casa01-dbus-cpp casa01-dbus-cpp-devel casa01-mpi4py.x86_64 \
casa01-openmpi.x86_64 casa01-python.x86_64 casa01-python-devel.x86_64 casa01-python-tools.x86_64 \
libsakura pgplot-devel pgplot-demos pgplot-motif \
lapack-devel xerces-c-devel

sudo yum -y install fftw fftw-devel

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

cmake -DUseCrashReporter=0 -DBoost_NO_BOOST_CMAKE=1 -DCASA_BUILD=1 -DBUILD_TESTING=OFF \
-DCMAKE_INSTALL_PREFIX=../../linux -DBUILD_PYTHON=1 -DPYTHON_INCLUDE_DIR=/opt/casa/01/include/python2.7/ \
-DPYTHON_LIBRARY=/opt/casa/01/lib/libpython2.7.so -DBOOST_ROOT=/usr/lib64/casa/01 -DCMAKE_BUILD_TYPE=Release \
-DCXX11=1 -DCMAKE_CXX_COMPILER=/opt/rh/devtoolset-3/root/usr/bin/g++ \
-DCMAKE_C_COMPILER=/opt/rh/devtoolset-3/root/usr/bin/gcc \
-DCMAKE_Fortran_COMPILER=/opt/rh/devtoolset-3/root/usr/bin/gfortran ..

# do not use make -j, not officiall support, may build fail
make
make install

### code
# -- Looking for GSLCBLAS library gslcblas
sudo yum -y install gsl gsl-devel # 1.15, duplicate install since we have our own gsl installed, fix later?
sudo yum -y install java
sudo yum -y install libxml2-devel libxslt-devel
sudo yum -y install rpfits readline-devel

cd ../../code
mkdir build && cd build
cmake -DUseCrashReporter=0  -DBoost_NO_BOOST_CMAKE=1 '-DEXTRA_C_FLAGS=-DPG_PPU -I/usr/include/wcslib' \
-Darch=linux -DBoost_NO_BOOST_CMAKE=1 -DCMAKE_BUILD_TYPE=Release -DCXX11=1 \
-DCMAKE_CXX_COMPILER=/opt/rh/devtoolset-3/root/usr/bin/g++ \
-DCMAKE_C_COMPILER=/opt/rh/devtoolset-3/root/usr/bin/gcc \
-DCMAKE_Fortran_COMPILER=/opt/rh/devtoolset-3/root/usr/bin/gfortran ..
make
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
