import cartavis
import test_cartavis

instance  = cartavis.Cartavis("/Users/grimmer/cartabuild/build/cpp/desktop/desktop.app/Contents/MacOS/desktop","/Users/grimmer/.cartavis/config.json",1999,"/Users/grimmer/cartabuild/CARTAvis-1411/carta/VFS/DesktopDevel/desktop/desktopIndex.html","/Users/grimmer/CARTA/Images/aJ.fits")
test_cartavis.test_getChannelCount(instance)
