#!/bin/bash

DIR=""
DIR=`dirname "$0"`; DIR=`eval "cd \"$DIR\" && pwd"`
export cartawork=$DIR/../../..
echo "CARTA working folder is $cartawork"
source $cartawork/CARTAvis/carta/scripts/ci_mac_common.sh

#libsakura?

#flex's gettext, not sure gettext is needed or not
# su $SUDO_USER <<EOF
# brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/d407fb8563f480391d918e1f1160cb7e244a1a12/Formula/gettext.rb
# EOF

# #qt
# echo "step4-1: Install Qt for CARTA if you use default homebrew-qt"
# installqt
#
# # gfortran
# echo "step4-0:install homebrew's gcc to get gfortran for ast, casa libs"
# installgfortran
#
# #gsl
# echo "step4-5: install gsl from homebrew"
# installgsl
#
# #bison, fftw, boost-python(<-not sure if we need )
# su $SUDO_USER <<EOF
# brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/49887a8f215bd8b365c28c6ae5ea62bb1350c893/Formula/bison.rb
# brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/cb79fc2ec2c5fcebf35f7bf26bb7459c9f87ae0b/Formula/fftw.rb
# brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/198f8903db0bb77f1b84e19e020c6f825210433d/Formula/boost-python.rb
# EOF
installforBuildCARTAandPackaging

echo "check everything before building carta"
cd $cartawork/CARTAvis-externals/ThirdParty
sudo su $SUDO_USER -c "brew list"
checkAllforCARTA
echo "list ThirdParty"
ls -l # print again
echo "list ThirdParty end"

# ### setup QtWebkit
# echo "step7: setup QtWebkit"
# pause
# su $SUDO_USER <<EOF
# cd $cartawork/CARTAvis-externals/ThirdParty
# curl -O -L $qtwebkitlink
# tar -xvzf $qtwebkit.tar.xz > /dev/null
# # copy include
# cp -r $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/include/QtWebKit $QT5PATH/include/
# cp -r $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/include/QtWebKitWidgets $QT5PATH/include/
# # copy lib
# cp $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/lib/libqt* $QT5PATH/lib/
# cp -r $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/lib/QtWebKit.framework $QT5PATH/lib/
# cp -r $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/lib/QtWebKitWidgets.framework $QT5PATH/lib/
# cp -r $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/lib/cmake/* $QT5PATH/lib/cmake/
# cp $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/lib/pkgconfig/* $QT5PATH/lib/pkgconfig/
# # copy mkspecs
# cp $cartawork/CARTAvis-externals/ThirdParty/$qtwebkit/mkspecs/modules/* $QT5PATH/mkspecs/modules/
# echo "QT.webkit.module = QtWebKit" >> $QT5PATH/mkspecs/modules/qt_lib_webkit.pri
# perl -pi.bak -e 's/QT.webkit.module_config =/QT.webkit.module_config = v2 lib_bundle/g' $QT5PATH/mkspecs/modules/qt_lib_webkit.pri
# echo "QT.webkitwidgets.module = QtWebKitWidgets" >> $QT5PATH/mkspecs/modules/qt_lib_webkitwidgets.pri
# perl -pi.bak -e 's/QT.webkitwidgets.module_config =/QT.webkitwidgets.module_config = v2 lib_bundle /g' $QT5PATH/mkspecs/modules/qt_lib_webkitwidgets.pri
# EOF
# ###
# printDuration

### build UI of CARTA
# echo "step8: Build UI of CARTA"
# pause
# su $SUDO_USER <<EOF
# echo $cartawork
# cd $cartawork/CARTAvis/carta/html5/common/skel
# ./generate.py
# ###
# EOF
# printDuration

### build CARTA
echo "step9: Build CARTA"
pause
su $SUDO_USER <<EOF
echo $QT5PATH
export PATH=$QT5PATH/bin:$PATH
mkdir -p $CARTABUILDHOME
cd $CARTABUILDHOME
if [ "$TRAVIS_BRANCH" != "" ] && [[ $TRAVIS_BRANCH == *"release"* ]]; then
  echo "carta build is release type"
  qmake -config release NOSERVER=1 CARTA_BUILD_TYPE=release $cartawork/CARTAvis/carta -r
else
  echo "carta build is dev type"
  qmake NOSERVER=1 CARTA_BUILD_TYPE=dev $cartawork/CARTAvis/carta -r
fi
make -j2
###
EOF
printDuration

echo "buld folder size:"
du -sh $CARTABUILDHOME
