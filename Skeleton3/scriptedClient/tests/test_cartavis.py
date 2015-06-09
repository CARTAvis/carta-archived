import os
import cartavis
from PIL import Image, ImageChops
from astropy.coordinates import SkyCoord

def test_getPixelValue(cartavisInstance):
    """
    Test pixel values on an image with known pixel values.
    """
    i = cartavisInstance.getImageViews()
    i[0].loadLocalFile(os.getcwd() + '/data/mexinputtest.fits')
    assert float(i[0].getPixelValue(0,0)[0]) == 0.5
    # Make sure that a blank value is being returned for a pixel outside the
    # image.
    assert i[0].getPixelValue(-1,-1)[0] == ''

def test_getChannelCount(cartavisInstance):
    """
    Test that the channel count is being returned properly for images
    with both one channel and multiple channels.
    """
    i = cartavisInstance.getImageViews()
    i[0].loadLocalFile(os.getcwd() + '/data/mexinputtest.fits')
    assert i[0].getChannelCount() == 1
    i[0].loadLocalFile(os.getcwd() + '/data/qualityimage.fits')
    assert i[0].getChannelCount() == 5

def test_getPixelUnits(cartavisInstance):
    """
    Test that the pixel units are being returned properly for an image
    with known units.
    """
    i = cartavisInstance.getImageViews()
    i[0].loadLocalFile(os.getcwd() + '/data/imagetestimage.fits')
    assert i[0].getPixelUnits()[0] == 'Jy/beam'

def test_getImageDimensions(cartavisInstance):
    """
    Test that the image dimensions are being returned properly for an
    image with known dimensions.
    """
    i = cartavisInstance.getImageViews()
    i[0].loadLocalFile(os.getcwd() + '/data/mexinputtest.fits')
    assert i[0].getImageDimensions() == [10, 10]

def test_zoomLevel(cartavisInstance):
    """
    Test that the zoom level is being set and returned properly.
    This is done by getting the zoom level, setting the zoom level to a
    multiple of the old zoom level, and then checking that the zoom
    level is indeed the same multiple of the old zoom level.
    """
    i = cartavisInstance.getImageViews()
    oldZoom = i[0].getZoomLevel()
    i[0].setZoomLevel(1.1 * oldZoom)
    assert i[0].getZoomLevel() == 1.1 * oldZoom

def test_getCoordinates(cartavisInstance):
    """
    Test that the coordinate values are being returned properly in each
    coordinate system.
    Also try a fake coordinate system to ensure that error checking is
    working.
    """
    i = cartavisInstance.getImageViews()
    i[0].loadLocalFile(os.getcwd() + '/data/mexinputtest.fits')
    assert i[0].getCoordinates(0, 0, 'j2000') ==\
        ['+3:32:15.956', '-27:42:46.800']
    assert i[0].getCoordinates(0, 0, 'b1950') ==\
        ['+3:30:10.257', '-27:52:52.719']
    assert i[0].getCoordinates(0, 0, 'galactic') ==\
        ['+223.400deg', '-54.467deg']
    assert i[0].getCoordinates(0, 0, 'ecliptic') ==\
        ['+41.113deg', '-45.085deg']
    assert i[0].getCoordinates(0, 0, 'icrs') ==\
        ['+3:32:15.954', '-27:42:46.784']
    assert i[0].getCoordinates(0, 0, 'fakesystem')[0] == 'error'

def test_saveFullImage(cartavisInstance, tempImageDir, cleanSlate):
    """
    Test that the saveFullImage() command works properly.
    """
    imageName = 'saveFullImage.png'
    i = cartavisInstance.getImageViews()
    i[0].loadLocalFile(os.getcwd() + '/data/mexinputtest.fits')
    i[0].saveFullImage(tempImageDir + '/' + imageName)
    reference = Image.open(os.getcwd() + '/data/' + imageName)
    comparison = Image.open(tempImageDir + '/' + imageName)
    assert list(reference.getdata()) == list(comparison.getdata())

def test_setColormap(cartavisInstance, tempImageDir, cleanSlate):
    """
    Test that a colormap is being applied properly.
    """
    imageName = 'setColormapCubehelix.png'
    i = cartavisInstance.getImageViews()
    c = cartavisInstance.getColormapViews()
    i[0].loadLocalFile(os.getcwd() + '/data/mexinputtest.fits')
    c[0].setColormap('cubehelix')
    i[0].saveFullImage(tempImageDir + '/' + imageName)
    reference = Image.open(os.getcwd() + '/data/' + imageName)
    comparison = Image.open(tempImageDir + '/' + imageName)
    assert list(reference.getdata()) == list(comparison.getdata())

# Putting this test on hold because SkyCoord cannot find COMBO-17 44244 all of
# a sudden.
def test_centerOnCoordinate(cartavisInstance, tempImageDir, cleanSlate):
    """
    Center an image on the coordinates of a nearby object, take a snapshot of
    the image view, and compare it to a reference snapshot.
    """
    imageName = 'centerOnCoordinate.png'
    i = cartavisInstance.getImageViews()
    i[0].loadLocalFile(os.getcwd() + '/data/mexinputtest.fits')
    c = SkyCoord.from_name("COMBO-17 44244")
    i[0].centerOnCoordinate(c)
    i[0].saveImage(tempImageDir + '/' + imageName)
    reference = Image.open(os.getcwd() + '/data/' + imageName)
    comparison = Image.open(tempImageDir + '/' + imageName)
    assert list(reference.getdata()) == list(comparison.getdata())

def test_saveImage(cartavisInstance, tempImageDir, cleanSlate):
    """
    Test that the saveImage() command works properly.
    NOTE: this test currently needs to be run in release mode because in
    dev mode the "Cached" text sometimes appears on the image, causing
    the test to fail.
    """
    imageName = 'saveImage.png'
    i = cartavisInstance.getImageViews()
    i[0].loadLocalFile(os.getcwd() + '/data/mexinputtest.fits')
    i[0].saveImage(tempImageDir + '/' + imageName)
    reference = Image.open(os.getcwd() + '/data/' + imageName)
    comparison = Image.open(tempImageDir + '/' + imageName)
    assert list(reference.getdata()) == list(comparison.getdata())
