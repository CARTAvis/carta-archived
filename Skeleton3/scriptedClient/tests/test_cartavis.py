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
    """
    Test pixel values on an image with known pixel values.
    """
    i[0].loadLocalFile('/scratch/Images/mexinputtest.fits')
    assert float(i[0].getPixelValue(0,0)[0]) == 0.5
    # Make sure that a blank value is being returned for a pixel outside the
    # image.
    assert i[0].getPixelValue(-1,-1)[0] == ''

def test_getChannelCount():
    """
    Test that the channel count is being returned properly for images with
    both one channel and multiple channels.
    """
    i[0].loadLocalFile('/scratch/Images/mexinputtest.fits')
    assert i[0].getChannelCount() == 1
    i[0].loadLocalFile('/scratch/Images/3D_fits/m31_cropped.fits')
    assert i[0].getChannelCount() == 3

def test_getPixelUnits():
    """
    Test that the pixel units are being returned properly for an image with
    known units.
    """
    i[0].loadLocalFile('/home/jeff/CARTA/Images/imagetestimage.fits')
    assert i[0].getPixelUnits()[0] == 'Jy/beam'

def test_getImageDimensions():
    """
    Test that the image dimensions are being returned properly for an image
    with known dimensions.
    """
    i[0].loadLocalFile('/scratch/Images/mexinputtest.fits')
    assert i[0].getImageDimensions() == [10, 10]

def test_zoomLevel():
    """
    Test that the zoom level is being set and returned properly.
    This is done by getting the zoom level, setting the zoom level to a
    multiple of the old zoom level, and then checking that the zoom level is
    indeed the same multiple of the old zoom level.
    """
    oldZoom = i[0].getZoomLevel()
    i[0].setZoomLevel(1.1 * oldZoom)
    assert i[0].getZoomLevel() == 1.1 * oldZoom
