import os
import cartavis

os.chdir('/home/jeff/scratch/build/cpp/desktop')
v = cartavis.Cartavis('/home/jeff/scratch/build/cpp/desktop/desktop',
                      '/home/jeff/.cartavis/config.json',
                      9999,
                      '/home/jeff/dev/CARTAvis/Skeleton3/VFS/DesktopDevel/desktop/desktopIndex.html',
                      '/scratch/Images/mexinputtest.fits')
i = v.getImageViews()

def test_getPixelValue():
    i[0].loadLocalFile('/scratch/Images/mexinputtest.fits')
    assert float(i[0].getPixelValue(0,0)[0]) == 0.5
    assert i[0].getPixelValue(-1,-1)[0] == ''

def test_getChannelCount():
    i[0].loadLocalFile('/scratch/Images/mexinputtest.fits')
    assert i[0].getChannelCount() == 1
    i[0].loadLocalFile('/scratch/Images/3D_fits/m31_cropped.fits')
    assert i[0].getChannelCount() == 3

def test_getPixelUnits():
    i[0].loadLocalFile('/home/jeff/CARTA/Images/imagetestimage.fits')
    assert i[0].getPixelUnits()[0] == 'Jy/beam'

def test_getImageDimensions():
    i[0].loadLocalFile('/scratch/Images/mexinputtest.fits')
    assert i[0].getImageDimensions() == [10, 10]
