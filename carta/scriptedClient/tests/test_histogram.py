import os
import cartavis
import pyautogui
import time
import ImageUtil

# Create a new window, and change its plugin to a CasaImageLoader
# Return the coordinates of the new image window 
def new_image_window(self):
    # Click on an existing image window 
    imageWindow = ImageUtil.locateCenterOnScreen('test_images/imageWindow.png') 
    assert imageWindow != None, 'Could not find an image window on the current display'
    pyautogui.click( x=imageWindow[0], y=imageWindow[1])
    time.sleep(3)

    # Click on the window button
    windowButton = ImageUtil.locateCenterOnScreen('test_images/windowButton.png')
    assert windowButton != None, 'Could not find the window button on the menu bar'
    pyautogui.doubleClick( x=windowButton[0], y=windowButton[1])
    time.sleep(2)

    # Locate the add button and click on it 
    pyautogui.press('down')
    pyautogui.press('down')
    pyautogui.press('down')
    pyautogui.press('down')
    pyautogui.press('right')
    pyautogui.press('return')
    time.sleep(2)

    # Change the plugin of the empty window to a casa image loader
    cartavisInstance.setEmptyWindowPlugin(0, 'CasaImageLoader')

def test_zoom(cartavisInstance, cleanSlate):
    """
    Test that the Histogram min and max zoom value updates
    """
    i = cartavisInstance.getImageViews()
    h = cartavisInstance.getHistogramViews()

    # Load an image
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')

    # Look for the min and max zoom value and store their values
    clipRange = h[0].getClipRange()
    minZoomValue = clipRange[0]
    maxZoomValue = clipRange[1]
    
    # Increase min zoom value, decrease max zoom value by 40
    # Default values are 0 and 100 respectively
    h[0].setClipRangePercent(40, 60)

    # Get the new min and max zoom values
    clipRange = h[0].getClipRange()
    minZoomValue = clipRange[0]
    maxZoomValue = clipRange[1]

    # Check that the new min is larger than the old min
    print "oldMin", minZoomValue," newMin=", newMinZoomPercent
    assert float(newMinZoomValue) > float(minZoomValue)

    # Check that the new max is smaller than the old max
    print "oldMax", maxZoomValue," newMax=", newMaxZoomPercent
    assert float(newMaxZoomValue) < float(maxZoomValue)

def test_histogramAddImage(cartavisInstance, cleanSlate):
    """
    Test that histogram values will update when an additional image is loaded
    in the image window.
    """
    i = cartavisInstance.getImageViews()
    h = cartavisInstance.getHistogramViews()

    # Load an image
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')

    # Get the max zoom value of the first image 
    clipRange = h[0].getClipRange()
    maxZoomValue = clipRange[1]
    print "First image maxZoomValue:", maxZoomValue

    # Load a different image in the same window 
    i[0].loadLocalFile(os.getcwd + '/data/aH.fits')

    # Check that the new max zoom value updates 
    clipRange = h[0].getClipRange()
    newMaxZoomValue = clipRange[1]
    assert newMaxZoomValue != maxZoomValue, 'Histogram did not update after an additional image was loaded in the CasaImageLoader'
    print "Second image maxZoomValue:", newMaxZoomValue

def test_histogramRemoveImage(cartavisInstance, cleanSlate):
    """
    Test that the removal of an image will restore the Histogram to default values
    """
    i = cartavisInstance.getImageViews()
    h = cartavisInstance.getHistogramViews()

    # Load an image, then close the image
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')
    i[0].closeImage('N15693D.fits')

    # Check that the histogram values are restored to default values 
    clipRange = h[0].getClipRange()
    newMaxZoomValue = clipRange[1]
    print "Zoom max value=", newMaxZoomValue
    assert float(newMaxZoomValue) == 1.0, 'Removal of the image did not restore Histogram to default values'

def test_histogramChangeImage(cartavisInstance, cleanSlate):
    """
    Test that the histogram updates its values when the image is changed in the image window.
    """
    i = cartavisInstance.getImageViews()
    a = cartavisInstance.getAnimatorViews()
    h = cartavisInstance.getHistogramViews()

    # Load two images in the same image window
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')
    i[0].loadLocalFile(os.getcwd + '/data/aH.fits')

    # Record the Histogram max zoom value of the second image 
    a[0].setImage(1)
    clipRange = h[0].getClipRange()
    secondMaxZoomValue = clipRange[1]
    print "Second image maxZoomValue:", secondMaxZoomValue  
    print "Changing back to the first image..."

    # Record the Histogram max zoom value of the first image
    a[0].setImage(0)
    clipRange = h[0].getClipRange()
    firstMaxZoomValue = clipRange[1]
    print "First image maxZoomValue:", firstMaxZoomValue 

    # Check that the Histogram updates its values
    assert firstMaxZoomValue != secondMaxZoomValue

def test_histogramLinking(cartavisInstance, cleanSlate):
    """
    Test that the histogram is only able to link to one image. We first
    load an image in the image window. This image should be linked to the
    histogram window. We then open a different image in a separate window 
    and try to link the histogram to the second image. This should fail, 
    and the histogram value should not change. 
    """
    i = cartavisInstance.getImageViews()
    h = cartavisInstance.getHistogramViews()

    # Load an image 
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')

    # Create a new, empty window and change its plugin to an image window
    self.new_image_window()

    # Record the max zoom value of the first image
    clipRange = h[0].getClipRange()
    maxZoomValue = clipRange[1]
    print "First image max zoom value:", maxZoomValue

    # Try to add a link from the Histogram to the second image 
    # This should fail: no link should be made from the Histogram to the second image
    try:
        i[1].addLink( h[0] )
    except Exception:
        print "Test suceeded: no link should be made."

    # Check that the second image is not linked to the Histogram
    # Check that the max zoom value did not change from the linking attempt
    clipRange = h[0].getClipRange()
    newMaxZoomValue = clipRange[1]
    print "New maxZoomValue:", newMaxZoomValue
    assert float(maxZoomValue) == float(newMaxZoomValue)

def test_histogramLinkRemoval(cartavisInstance, cleanSlate):
    """
    Test removal of a link from the Histogram.
    """
    i = cartavisInstance.getImageViews()
    h = cartavisInstance.getHistogramViews()

    # Remove link from histogram to main image window 
    i[0].removeLink( h[0] )

    # Load an image in the main imagewindow 
    i = cartavisInstance.getImageViews()
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')

    # Check that the histogram values are default values 
    clipRange = h[0].getClipRange()
    newMaxZoomValue = clipRange[1]
    assert float(newMaxZoomValue) == 1.0, 'The Histogram is still linked to the image after the link was removed.'

def test_histogramChangeLinks(cartavisInstance, cleanSlate):
    """
    Test that we can change the linked image to the Histogram
    """
    i = cartavisInstance.getImageViews()
    h = cartavisInstance.getHistogramViews()

    # Remove the link from the Histogram to the main image window 
    i[0].removeLink(h[0])

    # Create a new image window 
    self.new_image_window()

    # Link the Histogram to the second image 
    i[1].addLink(h[0])

    # Check that the histogram values are not default values 
    clipRange = h[0].getClipRange()
    newMaxZoomValue = clipRange[1]
    assert float(newMaxZoomValue) != 1.0, 'The Histogram was not linked to the second image window.'