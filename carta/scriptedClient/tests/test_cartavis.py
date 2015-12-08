import os
import pytest
import carta.cartavis as cartavis
from flaky import flaky
from PIL import Image, ImageChops
from astropy.coordinates import SkyCoord

def test_getPixelValue(cartavisInstance, cleanSlate):
    """
    Test pixel values on an image with known pixel values.
    """
    i = cartavisInstance.getImageViews()
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    assert float(i[0].getPixelValue(0,0)[0]) == 0.5
    # Make sure that a blank value is being returned for a pixel outside the
    # image.
    assert i[0].getPixelValue(-1,-1)[0] == ''

def test_getChannelCount(cartavisInstance, cleanSlate):
    """
    Test that the channel count is being returned properly for images
    with both one channel and multiple channels.
    """
    i = cartavisInstance.getImageViews()
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    assert i[0].getChannelCount() == 1
    i[0].loadFile(os.getcwd() + '/data/qualityimage.fits')
    assert i[0].getChannelCount() == 5
    # This next image actually has 4 dimensions and getChannelCount()
    # was previously returning the wrong value for it. This test case
    # is to ensure that the command has been properly fixed.
    i[0].loadFile(os.getcwd() + '/data/RaDecVel.fits')
    assert i[0].getChannelCount() == 7

def test_getPixelUnits(cartavisInstance, cleanSlate):
    """
    Test that the pixel units are being returned properly for an image
    with known units.
    """
    i = cartavisInstance.getImageViews()
    i[0].loadFile(os.getcwd() + '/data/imagetestimage.fits')
    assert i[0].getPixelUnits()[0] == 'Jy/beam'

def test_getImageDimensions(cartavisInstance, cleanSlate):
    """
    Test that the image dimensions are being returned properly for an
    image with known dimensions.
    """
    i = cartavisInstance.getImageViews()
    assert i[0].getImageDimensions()[0] == 'error'
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    assert i[0].getImageDimensions() == [10, 10]

def test_zoomLevel(cartavisInstance, cleanSlate):
    """
    Test that the zoom level is being set and returned properly.
    This is done by getting the zoom level, setting the zoom level to a
    multiple of the old zoom level, and then checking that the zoom
    level is indeed the same multiple of the old zoom level.
    """
    i = cartavisInstance.getImageViews()
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    oldZoom = i[0].getZoomLevel()
    i[0].setZoomLevel(1.1 * oldZoom)
    assert i[0].getZoomLevel() == 1.1 * oldZoom

@flaky(max_runs=10)
def test_getCoordinates(cartavisInstance, cleanSlate):
    """
    Test that the coordinate values are being returned properly in each
    coordinate system.
    Also try a fake coordinate system to ensure that error checking is
    working.
    """
    i = cartavisInstance.getImageViews()
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
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
    imageName = 'mexinputtest.png'
    i = cartavisInstance.getImageViews()
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    _saveFullImage(i[0], imageName, tempImageDir)

@pytest.mark.xfail(reason="Python colormaps may not be available.")
def test_setPythonColormap(cartavisInstance, tempImageDir, cleanSlate):
    """
    Test that a Python colormap can be applied properly.
    """
    i = cartavisInstance.getImageViews()
    c = cartavisInstance.getColormapViews()
    imageName = 'mexinputtest_copper.png'
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    c[0].setColormap('copper')
    _saveFullImage(i[0], imageName, tempImageDir)

def test_setCPPColormap(cartavisInstance, tempImageDir, cleanSlate):
    """
    Test that a C++ colormap can be applied properly.
    """
    i = cartavisInstance.getImageViews()
    c = cartavisInstance.getColormapViews()
    imageName = 'mexinputtest_CubeHelix1.png'
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    c[0].setColormap('CubeHelix1')
    _saveFullImage(i[0], imageName, tempImageDir)

def test_setDefaultColormap(cartavisInstance, tempImageDir, cleanSlate):
    """
    Test that the default colormap can be applied properly.
    """
    i = cartavisInstance.getImageViews()
    c = cartavisInstance.getColormapViews()
    imageName = 'mexinputtest_Gray.png'
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    c[0].setColormap('Gray')
    _saveFullImage(i[0], imageName, tempImageDir)

@pytest.mark.xfail(reason="saveImage() has been deprecated for now.")
def test_centerOnCoordinate(cartavisInstance, tempImageDir, cleanSlate):
    """
    Center an image on the coordinates of a nearby object, take a
    snapshot of the image view, and compare it to a reference snapshot.
    NOTE: this test was failing for me at one point because SkyCoord
    could not find COMBO-17 44244.
    """
    imageName = 'mexinputtest_centerOnCoordinate.png'
    i = cartavisInstance.getImageViews()
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    c = SkyCoord.from_name("COMBO-17 44244")
    i[0].centerOnCoordinate(c)
    i[0].saveImage(tempImageDir + '/' + imageName)
    reference = Image.open(os.getcwd() + '/data/' + imageName)
    comparison = Image.open(tempImageDir + '/' + imageName)
    assert list(reference.getdata()) == list(comparison.getdata())

@pytest.mark.xfail(reason="saveImage() has been deprecated for now.")
def test_saveImage(cartavisInstance, tempImageDir, cleanSlate):
    """
    Test that the saveImage() command works properly.
    NOTE: this test currently needs to be run in release mode because in
    dev mode the "Cached" text sometimes appears on the image, causing
    the test to fail.
    """
    imageName = 'mexinputtest_saveImage.png'
    i = cartavisInstance.getImageViews()
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    i[0].saveImage(tempImageDir + '/' + imageName)
    reference = Image.open(os.getcwd() + '/data/' + imageName)
    comparison = Image.open(tempImageDir + '/' + imageName)
    assert list(reference.getdata()) == list(comparison.getdata())

@pytest.mark.xfail(reason="saveImage() has been deprecated for now.")
def test_centerOnPixel(cartavisInstance, tempImageDir, cleanSlate):
    """
    Test that the centerOnPixel() command works properly.
    """
    imageName = 'mexinputtest_centerOnPixel.png'
    i = cartavisInstance.getImageViews()
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    i[0].centerOnPixel(0,0)
    i[0].saveImage(tempImageDir + '/' + imageName)
    reference = Image.open(os.getcwd() + '/data/' + imageName)
    comparison = Image.open(tempImageDir + '/' + imageName)
    assert list(reference.getdata()) == list(comparison.getdata())

def test_setChannel(cartavisInstance, tempImageDir, cleanSlate):
    """
    Test that the animator is setting the channel properly.
    """
    image1 = 'RaDecVel_channel1.png'
    image2 = 'RaDecVel_channel2.png'
    i = cartavisInstance.getImageViews()
    a = cartavisInstance.getAnimatorViews()
    i[0].loadFile(os.getcwd() + '/data/RaDecVel.fits')
    channels = i[0].getChannelCount()
    a[0].setChannel(0)
    _saveFullImage(i[0], image1, tempImageDir)
    a[0].setChannel(1)
    _saveFullImage(i[0], image2, tempImageDir)
    # Also check that invalid channel values yield error messages
    assert a[0].setChannel(channels+10) != ['']
    assert a[0].setChannel(-10) != ['']

def test_setImage(cartavisInstance, tempImageDir, cleanSlate):
    """
    Test that the animator is setting the image properly.
    """
    i = cartavisInstance.getImageViews()
    a = cartavisInstance.getAnimatorViews()
    _setImage(i[0], a[0], tempImageDir)

@pytest.mark.xfail(reason="Python colormaps may not be available.")
def test_invertColormap(cartavisInstance, tempImageDir, cleanSlate):
    """
    Test that the colormap is inverted properly.
    """
    imageName = 'mexinputtest_cubehelix_inverted.png'
    i = cartavisInstance.getImageViews()
    c = cartavisInstance.getColormapViews()
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    c[0].setColormap('cubehelix')
    c[0].invertColormap(True)
    _saveFullImage(i[0], imageName, tempImageDir)

@pytest.mark.xfail(reason="Python colormaps may not be available.")
def test_reverseColormap(cartavisInstance, tempImageDir, cleanSlate):
    """
    Test that the colormap is reversed properly.
    """
    imageName = 'mexinputtest_cubehelix_reversed.png'
    i = cartavisInstance.getImageViews()
    c = cartavisInstance.getColormapViews()
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    c[0].setColormap('cubehelix')
    c[0].reverseColormap(True)
    _saveFullImage(i[0], imageName, tempImageDir)

def test_setColorMix(cartavisInstance, tempImageDir, cleanSlate):
    """
    Test that the color mix is being set properly.
    """
    imageName = 'mexinputtest_colormix.png'
    i = cartavisInstance.getImageViews()
    c = cartavisInstance.getColormapViews()
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    c[0].setColorMix(0.7, 0.3, 0.8)
    _saveFullImage(i[0], imageName, tempImageDir)
    # Check that invalid values cause error information to be returned.
    assert c[0].setColorMix(-1,-1,-1)[0] != ''

def test_setDataTransform(cartavisInstance, tempImageDir, cleanSlate):
    """
    Test that data transforms can be set properly.
    """
    imageName = 'mexinputtest_datatransform.png'
    i = cartavisInstance.getImageViews()
    c = cartavisInstance.getColormapViews()
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    c[0].setDataTransform('square root')
    _saveFullImage(i[0], imageName, tempImageDir)
    # Check that invalid values cause error information to be returned.
    assert c[0].setDataTransform('squarepants')[0] != ''

def test_setGamma(cartavisInstance, tempImageDir, cleanSlate):
    """
    Test that the gamma value can be set properly.
    """
    imageName = 'mexinputtest_gamma.png'
    i = cartavisInstance.getImageViews()
    c = cartavisInstance.getColormapViews()
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    c[0].setGamma(0.25)
    _saveFullImage(i[0], imageName, tempImageDir)

@pytest.mark.xfail(reason="The behaviour of the saveHistogram() function\
                   has changed.")
def test_saveHistogram(cartavisInstance, tempImageDir, cleanSlate):
    """
    Test that an image of the histogram can be saved.
    """
    imageName = 'histogram.png'
    i = cartavisInstance.getImageViews()
    h = cartavisInstance.getHistogramViews()
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    h[0].setPlaneMode('all')
    h[0].saveHistogram(tempImageDir + '/' + imageName, 200, 200)
    reference = Image.open(os.getcwd() + '/data/' + imageName)
    comparison = Image.open(tempImageDir + '/' + imageName)
    assert list(reference.getdata()) == list(comparison.getdata())

@pytest.mark.skipif(True, reason="The histogram does not currently\
                    update properly.")
def test_setBinCount(cartavisInstance, tempImageDir, cleanSlate):
    """
    Test that an image of the histogram can be saved.
    """
    imageName = 'histogram_100bin.png'
    i = cartavisInstance.getImageViews()
    h = cartavisInstance.getHistogramViews()
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    h[0].setPlaneMode('all')
    h[0].setBinCount(100)
    h[0].saveHistogram(tempImageDir + '/' + imageName, 200, 200)
    reference = Image.open(os.getcwd() + '/data/' + imageName)
    comparison = Image.open(tempImageDir + '/' + imageName)
    assert list(reference.getdata()) == list(comparison.getdata())

def test_closeImage(cartavisInstance, cleanSlate):
    """
    Test that an image can be closed.
    """
    i = cartavisInstance.getImageViews()
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    i[0].loadFile(os.getcwd() + '/data/qualityimage.fits')
    imagesBefore = i[0].getImageNames()
    i[0].closeImage(imagesBefore[0])
    imagesAfter = i[0].getImageNames()
    assert imagesBefore[0] not in imagesAfter

def test_getImageNames(cartavisInstance, cleanSlate):
    """
    Test that the list of names of open images can be obtained.
    """
    i = cartavisInstance.getImageViews()
    imageToLoad = (os.getcwd() + '/data/mexinputtest.fits')
    imagesBefore = i[0].getImageNames()
    assert imageToLoad not in imagesBefore
    i[0].loadFile(imageToLoad)
    imagesAfter = i[0].getImageNames()
    assert imageToLoad in imagesAfter

def test_getLinkedAnimators(cartavisInstance, tempImageDir, cleanSlate):
    """
    Test that the list of animators linked to the image view can be
    obtained.
    This can be accomplished by performing an operation with one of the
    linked animators and confirming that the operation was successful.
    """
    i = cartavisInstance.getImageViews()
    a = i[0].getLinkedAnimators()
    _setImage(i[0], a[0], tempImageDir)

@pytest.mark.xfail(reason="Python colormaps may not be available.")
def test_getLinkedColormaps(cartavisInstance, tempImageDir, cleanSlate):
    """
    Test that the list of colormap views linked to the image view can be
    obtained.
    This can be accomplished by performing an operation with one of the
    linked colormap views and confirming that the operation was
    successful.
    """
    i = cartavisInstance.getImageViews()
    c = i[0].getLinkedColormaps()
    imageName = 'mexinputtest_cubehelix.png'
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    c[0].setColormap('cubehelix')
    _saveFullImage(i[0], imageName, tempImageDir)

def test_loadFile(cartavisInstance, tempImageDir, cleanSlate):
    """
    Test that a file can be loaded into an image view.
    """
    i = cartavisInstance.getImageViews()
    # First, check that trying to load a nonexistent file generates an
    # error.
    nonexistentLoadResult = i[0].loadFile('nonexistentFile.fits')
    assert nonexistentLoadResult[0] == 'error'
    # Next, check that trying to load an existing file does not generate
    # an error.
    loadResult = i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    assert loadResult[0] != 'error'
    # Finally, check that the image that has been loaded is actually
    # the image we expect.
    _saveFullImage(i[0], 'mexinputtest.png', tempImageDir)

def test_getIntensity(cartavisInstance, cleanSlate):
    """
    Test that the intensity of an image can be obtained.
    """
    i = cartavisInstance.getImageViews()
    loadResult = i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    intensity = i[0].getIntensity(0, 0, 0.5)
    assert intensity == 49.5

def test_getMaxImageCount(cartavisInstance, cleanSlate):
    """
    Test that the animator can return the number of images currently
    being managed.
    """
    i = cartavisInstance.getImageViews()
    a = cartavisInstance.getAnimatorViews()
    assert a[0].getMaxImageCount() == 0
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    assert a[0].getMaxImageCount() == 1
    i[0].loadFile(os.getcwd() + '/data/qualityimage.fits')
    assert a[0].getMaxImageCount() == 2
    for f in (i[0].getImageNames()):
        i[0].closeImage(f)
    assert a[0].getMaxImageCount() == 0

def test_getColormaps(cartavisInstance, cleanSlate):
    """
    Test that the list of available colormaps can be obtained.
    """
    cm = cartavisInstance.getColormaps()
    assert cm

def test_getEmptyWindowCount(cartavisInstance, cleanSlate):
    """
    Test that the number of empty windows can be obtained.
    """
    rows = 3
    cols = 3
    cartavisInstance.setImageLayout()
    emptyCount = cartavisInstance.getEmptyWindowCount()
    assert emptyCount == 0
    cartavisInstance.setCustomLayout(rows, cols)
    newEmptyCount = cartavisInstance.getEmptyWindowCount()
    assert newEmptyCount == rows * cols - 1

def test_getAnimatorViews(cartavisInstance, cleanSlate):
    """
    Test that the animator views can be obtained.
    """
    cartavisInstance.setImageLayout()
    assert not cartavisInstance.getAnimatorViews()
    cartavisInstance.setAnalysisLayout()
    assert cartavisInstance.getAnimatorViews()

def test_getColormapViews(cartavisInstance, cleanSlate):
    """
    Test that the colormap views can be obtained.
    """
    cartavisInstance.setImageLayout()
    assert not cartavisInstance.getColormapViews()
    cartavisInstance.setAnalysisLayout()
    assert cartavisInstance.getColormapViews()

def test_getHistogramViews(cartavisInstance, cleanSlate):
    """
    Test that the histogram views can be obtained.
    """
    cartavisInstance.setImageLayout()
    assert not cartavisInstance.getHistogramViews()
    cartavisInstance.setAnalysisLayout()
    assert cartavisInstance.getHistogramViews()

def test_getImageViews(cartavisInstance, cleanSlate):
    """
    Test that the image views can be obtained.
    """
    cartavisInstance.setImageLayout()
    assert cartavisInstance.getImageViews()
    cartavisInstance.setCustomLayout(2, 2)
    cartavisInstance.setPlugins(['Empty', 'Empty', 'Empty', 'Empty'])
    assert not cartavisInstance.getImageViews()

def test_setImageLayout(cartavisInstance, cleanSlate):
    """
    Test that the image layout can be set.
    """
    cartavisInstance.setImageLayout()
    plugins = cartavisInstance.getPluginList()
    assert sorted(plugins) == ['CasaImageLoader', 'Hidden']

def test_setAnalysisLayout(cartavisInstance, cleanSlate):
    """
    Test that the analysis layout can be set.
    """
    cartavisInstance.setAnalysisLayout()
    plugins = cartavisInstance.getPluginList()
    assert sorted(plugins) == ['Animator', 'CasaImageLoader', 'Colormap',
                              'Hidden', u'Histogram']

def test_removeLink(cartavisInstance, cleanSlate):
    """
    Test that a link between an image view and other view types can be
    removed.
    Since link a link can be removed directly from an Image object or
    from the Cartavis class, both methods will be tested.
    """
    i = cartavisInstance.getImageViews()
    # First, the Image way
    laBefore = i[0].getLinkedAnimators()
    assert len(laBefore) > 0
    i[0].removeLink(laBefore[0])
    laAfter = i[0].getLinkedAnimators()
    assert len(laAfter) == len(laBefore) - 1
    # Second, the Cartavis way
    lcBefore = i[0].getLinkedColormaps()
    assert len(lcBefore) > 0
    cartavisInstance.removeLink(lcBefore[0], i[0])
    lcAfter = i[0].getLinkedColormaps()
    assert len(lcAfter) == len(lcBefore) - 1

def test_addLink(cartavisInstance, cleanSlate):
    """
    Test that a link between an image view and another view type can be
    added.
    Since link a link can be added directly from an Image object or from
    the Cartavis class, both methods will be tested.
    """
    cartavisInstance.setImageLayout()
    cartavisInstance.setCustomLayout(2, 2)
    cartavisInstance.setPlugins(['Animator', 'CasaImageLoader', 'Colormap',
                         'Histogram'])
    i = cartavisInstance.getImageViews()
    a = cartavisInstance.getAnimatorViews()
    c = cartavisInstance.getColormapViews()
    # First, the Image way
    laBefore = i[0].getLinkedAnimators()
    i[0].addLink(a[0])
    laAfter = i[0].getLinkedAnimators()
    assert len(laAfter) == len(laBefore) + 1
    # Second, the Cartavis way
    lcBefore = i[0].getLinkedColormaps()
    cartavisInstance.addLink(c[0], i[0])
    lcAfter = i[0].getLinkedColormaps()
    assert len(lcAfter) == len(lcBefore) + 1

def test_setEmptyWindowPlugin(cartavisInstance, cleanSlate):
    """
    Test that an empty window can be set to a plugin.
    """
    cartavisInstance.setImageLayout()
    cartavisInstance.setCustomLayout(2, 2)
    plugins = cartavisInstance.getPluginList()
    # Even though the first argument to setEmptyWindowPlugin() is
    # relative to the number of empty windows (e.g. 0 means that the
    # first empty window will be changed), we will keep track of the
    # index of the first empty window so that we can later check that it
    # has actually changed.
    firstEmpty = plugins.index('Empty')
    cartavisInstance.setEmptyWindowPlugin(0, 'CasaImageLoader')
    pluginsAfter = cartavisInstance.getPluginList()
    assert pluginsAfter[firstEmpty] != 'Empty'

def test_getCenterPixel(cartavisInstance, cleanSlate):
    """
    Test that the currently centered image pixel can be obtained.
    """
    i = cartavisInstance.getImageViews()
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    assert i[0].getCenterPixel() == [5.0, 5.0]

def test_setCustomLayout(cartavisInstance, cleanSlate):
    """
    Test that a custom layout can be set.
    """
    rows = 3
    cols = 4
    assert cartavisInstance.setCustomLayout(rows, cols) == ['']
    # Does the right number of plugins appear?
    assert len(cartavisInstance.getPluginList()) == rows * cols
    # Does an error message get returned if we try setting an invalid
    # value?
    assert cartavisInstance.setCustomLayout(-1, cols) != ['']

def test_centerImage(cartavisInstance, cleanSlate):
    """
    Test that the image can be centered properly.
    """
    cartavisInstance.setImageLayout()
    i = cartavisInstance.getImageViews()
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    dim = i[0].getImageDimensions()
    center = [dim[0]/2.0, dim[1]/2.0]
    # First, deliberately set the image to off center
    i[0].centerOnPixel(-1,-1)
    assert i[0].getCenterPixel() != center
    # Now, center the image and check that it is centered properly.
    i[0].centerImage()
    assert i[0].getCenterPixel() == center

def test_getPluginList(cartavisInstance, cleanSlate):
    """
    Test that the list of plugins can be obtained.
    """
    cartavisInstance.setImageLayout()
    plugins = cartavisInstance.getPluginList()
    assert sorted(plugins) == ['CasaImageLoader', 'Hidden']
    cartavisInstance.setAnalysisLayout()
    plugins = cartavisInstance.getPluginList()
    assert sorted(plugins) == ['Animator', 'CasaImageLoader', 'Colormap',
                              'Hidden', u'Histogram']

@pytest.mark.skipif(True, reason="Seems to be causing problems currently.")
def test_getChannelIndex(cartavisInstance, cleanSlate):
    """
    Test that the channel index can be obtained from the animator.
    """
    i = cartavisInstance.getImageViews()
    a = cartavisInstance.getAnimatorViews()
    i[0].loadFile(os.getcwd() + '/data/qualityimage.fits')
    assert a[0].getChannelIndex() == 0
    a[0].setChannel(4)
    assert a[0].getChannelIndex() == 4

def test_saveHistogramConsistency(cartavisInstance, cleanSlate, tempImageDir):
    """
    Test that saved histogram images are of consistent size and content
    regardless of what is currently shown in the GUI.
    This is a regression test to ensure that issue #96 has been dealt
    with properly.
    """
    i = cartavisInstance.getImageViews()
    h = cartavisInstance.getHistogramViews()
    file1 = tempImageDir + '/' + 'hist1.png'
    file2 = tempImageDir + '/' + 'hist2.png'
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    h[0].saveHistogram(file1, 200, 200)
    cartavisInstance.setCustomLayout(3,3)
    h[0].saveHistogram(file2, 200, 200)
    image1 = Image.open(file1)
    image2 = Image.open(file2)
    assert list(image1.getdata()) == list(image2.getdata())

def test_isEmpty(cartavisInstance, cleanSlate):
    """
    Test that the isEmpty() Image method returns correct values.
    """
    i = cartavisInstance.getImageViews()
    assert i[0].isEmpty()
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    assert not i[0].isEmpty()

@pytest.mark.skipif(not os.path.isdir(os.path.expanduser(
                    '~/CARTA/Images/CARTAImages/BigImageTest')),
                    reason="Directory does not exist.")
@pytest.mark.skipif(not os.path.isdir(os.path.expanduser(
                    '~/CARTA/Images/CARTAImages/TransposeTest')),
                    reason="Directory does not exist.")
def test_saveImageWithoutCrashing(cartavisInstance, cleanSlate, tempImageDir):
    """
    Tests that images can be saved without the application crashing.
    This is a regression test to ensure that issue #94 has been dealt
    with properly.
    """
    i = cartavisInstance.getImageViews()
    i[0].loadFile(os.path.expanduser(
        '~/CARTA/Images/CARTAImages/TransposeTest/RaDecVel.fits'))
    i[0].saveFullImage(tempImageDir + '/' + 'RaDecVel.png')
    i[0].loadFile(os.path.expanduser(
        '~/CARTA/Images/CARTAImages/BigImageTest/h_m51_b_s05_drz_sci.fits'))
    i[0].saveFullImage(tempImageDir + '/' + 'BigImageTest.png')

@pytest.mark.skipif(True, reason="At least one image in this directory does not\
                    currently load")
def test_loadFile_BigImageTest(cartavisInstance, cleanSlate):
    """
    Test that the image(s) in CARTAImages/BigImageTest can be loaded.
    """
    i = cartavisInstance.getImageViews()
    _loadFilesFromDirectory(i[0],
        os.path.expanduser( '~/CARTA/Images/CARTAImages/BigImageTest'))

@pytest.mark.skipif(not os.path.isdir(os.path.expanduser(
                    '~/CARTA/Images/CARTAImages/SmallMultiplesTest')),
                    reason="Directory does not exist.")
def test_loadFile_SmallMultiplesTest(cartavisInstance, cleanSlate):
    """
    Test that the image(s) in CARTAImages/SmallMultiplesTest can be
    loaded.
    Note that this test will be skipped if the directory does not exist.
    If it is being skipped and you would like it to run, just make sure
    that the CARTA/Images/CARTAImages/SmallMultiplesTest directory
    exists under your home directory and that it is populated with the
    appropriate images from the CARTAImages repository.
    """
    i = cartavisInstance.getImageViews()
    _loadFilesFromDirectory(i[0],
        os.path.expanduser( '~/CARTA/Images/CARTAImages/SmallMultiplesTest'))

@pytest.mark.skipif(not os.path.isdir(os.path.expanduser(
                    '~/CARTA/Images/CARTAImages/TransposeTest')),
                    reason="Directory does not exist.")
@pytest.mark.skipif(True, reason="At least one image in this directory does not\
                    currently load")
def test_loadFile_TransposeTest(cartavisInstance, cleanSlate):
    """
    Test that the image(s) in CARTAImages/CARTAImages/TransposeTest can
    be loaded.
    Note that this test will be skipped if the directory does not exist.
    If it is being skipped and you would like it to run, just make sure
    that the CARTA/Images/CARTAImages/TransposeTest directory exists
    under your home directory and that it is populated with the
    appropriate images from the CARTAImages repository.
    """
    i = cartavisInstance.getImageViews()
    _loadFilesFromDirectory(i[0],
        os.path.expanduser( '~/CARTA/Images/CARTAImages/TransposeTest'))

@pytest.mark.skipif(not os.path.isdir(os.path.expanduser(
                    '~/CARTA/Images/CARTAImages/CubesTest')),
                    reason="Directory does not exist.")
@pytest.mark.skipif(True, reason="At least one image in this directory does not\
                    currently load")
def test_loadFile_CubesTest(cartavisInstance, cleanSlate):
    """
    Test that the image(s) in CARTAImages/CARTAImages/CubesTest can be
    loaded.  Note that this test will be skipped if the directory does
    not exist.  If it is being skipped and you would like it to run,
    just make sure that the CARTA/Images/CARTAImages/CubesTest directory
    exists under your home directory and that it is populated with the
    appropriate images from the CARTAImages repository.
    """
    i = cartavisInstance.getImageViews()
    _loadFilesFromDirectory(i[0],
        os.path.expanduser( '~/CARTA/Images/CARTAImages/CubesTest'))

@pytest.mark.skipif(not os.path.isdir(os.path.expanduser(
                    '~/CARTA/Images/CARTAImages/AstrometryTest')),
                    reason="Directory does not exist.")
def test_loadFile_AstrometryTest(cartavisInstance, cleanSlate):
    """
    Test that the image(s) in CARTAImages/CARTAImages/AstrometryTest can
    be loaded.  Note that this test will be skipped if the directory
    does not exist.  If it is being skipped and you would like it to
    run, just make sure that the CARTA/Images/CARTAImages/AstrometryTest
    directory exists under your home directory and that it is populated
    with the appropriate images from the CARTAImages repository.
    """
    i = cartavisInstance.getImageViews()
    _loadFilesFromDirectory(i[0],
        os.path.expanduser( '~/CARTA/Images/CARTAImages/AstrometryTest'))

def _setImage(imageView, animatorView, tempImageDir):
    """
    A common private function for commands that need to test that an
    image can be set by an animator.
    """
    image1 = 'mexinputtest.png'
    image2 = 'qualityimage.png'
    imageView.loadFile(os.getcwd() + '/data/mexinputtest.fits')
    imageView.loadFile(os.getcwd() + '/data/qualityimage.fits')
    animatorView.setImage(0)
    _saveFullImage(imageView, image1, tempImageDir)
    animatorView.setImage(1)
    _saveFullImage(imageView, image2, tempImageDir)

def _saveFullImage(imageView, imageName, tempImageDir):
    """
    A common private function for commands that need to save a full
    image and test that it has been saved properly.
    """
    imageView.saveFullImage(tempImageDir + '/' + imageName)
    reference = Image.open(os.getcwd() + '/data/' + imageName)
    comparison = Image.open(tempImageDir + '/' + imageName)
    assert list(reference.getdata()) == list(comparison.getdata())

def _loadFilesFromDirectory(imageView, directory):
    """
    Attempts to lead each of the files in a directory.
    """
    for fileName in os.listdir(directory):
        assert imageView.loadFile(directory + '/' + fileName) == ['']
