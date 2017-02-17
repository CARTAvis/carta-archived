## create working folder, $CARTAWORKHOME
# mkdir ~/src
# cd ~/src

CARTAWORKHOME=`pwd`

yum -y install wget
yum -y install mesa-libGL-devel

## Later, we can change to use latest qt online installer to install latest creator + qt 5.3.2 choosed
# possible to use yum to install qt 5.3.2 ? ref: qt5-qtbase-5.2.0-4.fc20.x86_64?
mkdir -p ~/download
cd ~/download
## or http://download.qt.io/archive/qt/5.8/5.8.0/qt-opensource-linux-x64-5.8.0.run
wget http://download.qt.io/archive/qt/5.3/5.3.2/qt-opensource-linux-x64-5.3.2.run
chmod 755 qt-opensource-linux-x64-5.3.2.run
# http://doc.qt.io/qt-5/linux.html
mkdir -p $CARTAWORKHOME/CARTAvis-externals/ThirdParty/Qt5.3.2 ## include qt creaetor
./qt-opensource-linux-x64-5.3.2.run ## will prompt UI to install, but font is missing
# choose to install in  ~/src/CARTAvis-externals/ThirdParty/Qt5.3.2/,
# can change to use default location, ~/Qt/?
