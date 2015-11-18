import carta.cartavis as cartavis
import os
import time
import pyautogui
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

# Verify that the number of animators that are visible is equal to expected count 
def _verifyAnimationCount(self, expectedCount):
    animatorCount = len(cartavisInstance.getAnimatorViews())
    print 'Animator list count=', animatorCount, 'Expected Count=', expectedCount
    assert animatorCount == expectedCount, 'Number of animators did not match the expected count'

# Go to the first channel value of the test image 
def _getFirstValue(self):
    firstValueButton = ImageUtil.locateCenterOnScreen("test_images/firstValueButton.png")
    assert firstValueButton != None, 'Could not find the firstValueButton on the current display'
    pyautogui.click( x=firstValueButton[0], y=firstValueButton[1])
    time.sleep(2)

# Go to the last channel value of the test image
def _getLastValue(self):
    lastValueButton = ImageUtil.locateCenterOnScreen("test_images/lastValueButton.png")
    assert lastValueButton != None, 'Could not find the lastValueButton on the current display'
    pyautogui.click( x=lastValueButton[0], y=lastValueButton[1])
    time.sleep(2)

# Go to the next value 
def _getNextValue(self):
    incrementButton = ImageUtil.locateCenterOnScreen( "test_images/incrementButton.png")
    assert incrementButton != None, 'Could not find the incrementButton on the current display'
    pyautogui.click( x=incrementButton[0], y=incrementButton[1])
    time.sleep(2)

# Click the forward animation button
def _animateFoward(self):
    forwardAnimateButton = ImageUtil.locateCenterOnScreen("test_images/forwardAnimation.png")
    assert forwardAnimateButton != None, 'Could not find the forward animation button on the current display'
    pyautogui.click( x=forwardAnimateButton[0], y=forwardAnimateButton[1])

# Click the stop button on the tape deck
def _stopAnimation(self):
    stopButton = ImageUtil.get_match_coordinates( "test_images/stopButton.png", "desktop.png") 
    assert stopButton != None, 'Could not find the stop button on the current display'
    pyautogui.click( x=stopButton[0], y=stopButton[1])

# Click on the animator settings checkbox
def _openAnimatorSettings(self):
    settingsButton = ImageUtil.locateCenterOnScreen( "test_images/settingsCheckBox.png")
    assert settingsButton != None, 'Could not find the animator settings checkbox on the current display'
    pyautogui.click( x=settingsButton[0], y=settingsButton[1])

def test_animatorRemoveImage(cartavisInstance, cleanSlate):
    """
    Test that the Animator will revert to default values after an image is removed
    """
    i = cartavisInstance.getImageViews()
    a = cartavisInstance.getAnimatorViews()

    # Load an image, then close the image
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')
    i[0].closeImage('N15693D.fits')

    # Get the channel upper spin value 
    self._getLastValue()
    upperBoundValue = a[0].getChannelIndex()
    assert upperBoundValue == 0
    assert a[0].getMaxImageCount() == 0

def test_animatorAddImage(cartavisInstance, cleanSlate):
    """
    Test that the Animator will update after an image is loaded in the image window
    """
    i = cartavisInstance.getImageViews()

    # Load an image 
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')

    # Record the upper bound spin box value of the first image 
    self._getLastValue()
    upperBound = a[0].getChannelIndex()

    # In the same window, load a different image 
    # The image should have a different number of channels
    i[0].loadLocalFile(os.getcwd + '/data/aH.fits')

    self._getLastValue()
    newUpperBound = a[0].getChannelIndex()
    assert upperBound != newUpperBound, 'Animator did not update an an image was added in image window'
    assert a[0].getMaxImageCount() == 2

def test_animatorChangeLink(cartavisInstance, cleanSlate):
    """
    Test that the Animator will update when linked to an image in a separate window.
    """
    i = cartavisInstance.getImageViews()
    a = cartavisInstance.getAnimatorViews()

    # Remove the link from the image viewer to the animator
    i[0].removeLink(a[0])

    # Load an image in a different window 
    self.new_image_window()
    i[1].loadLocalFile(os.getcwd + '/data/N15693D.fits')

    # Add a link to the new image window 
    i[1].addLink(a[0])

    # Check that the animator updates to the second image 
    lastChannel = i[1].getChannelCount() - 1
    self._getLastValue()
    assert a[0].getChannelIndex() == lastChannel, 'The animator was not linked to the second image window'

def test_animatorRemoveLink(cartavisInstance, cleanSlate):
    """
    Test that we can remove a linked image from the Animator.
    """
    i = cartavisInstance.getImageViews()

    # Remove the link from the image viewer to the animator
    i[0].removeLink(a[0])

    # Load an image
    i[0].loadLocalFile(o0s.getcwd + '/data/N15693D.fits')
 
    # Check that the animator does not update 
    self._getLastValue()
    assert a[0].getChannelIndex() == 0, 'The animator is still linked to the image viewer after the link was removed.'

   def test_channelAnimatorChangeImage(cartavisInstance, cleanSlate):
    """
    Test that the Channel Animator will update when the window image is switched.
    """
    i = cartavisInstance.getImageViews() 
    a = cartavisInstance.getAnimatorViews()

    # Load an image
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')

    # Load a different image 
    i[0].loadLocalFile(os.getcwd + '/data/aH.fits')

    # Get the upper spinbox value of the second image
    # Go to the last value and get the channel value  
    lastValueButton = ImageUtil.locateCenterOnScreen('test_images/lastValueButton.png')
    pyautogui.click( x=lastValueButton[0], y=lastValueButton[1])
    time.sleep(2)
    upperBound = a[0].getChannelIndex()

    # Change back to the first image and get the upper spinbox value
    a[0].setImage(0)
    pyautogui.click( x=lastValueButton[0], y=lastValueButton[1])
    newUpperBound = a[0].getChannelIndex()
    assert upperBound != newUpperBound

def test_animatorJump(cartavisInstance, cleanSlate):
    """
    Test that the Animator jump setting animates the first and last channel values.
    """
    i = cartavisInstance.getImageViews()
    a = cartavisInstance.getAnimatorViews()

    # Load an image
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')

    # Get the last channel value (should be the number of channels - 1)
    lastChannel = i[0].getChannelCount() - 1
    
    # Open animator settings 
    self._openAnimatorSettings()

    # Click the jump radio button
    jumpButton = ImageUtil.locateCenterOnScreen('test_images/jumpButton.png')
    assert jumpButton != None
    pyautogui.click( x=jumpButton[0], y=jumpButton[1])

    # Click the increment button and get the current channel
    self._getNextValue()
    assert a[0].getChannelIndex() == lastChannel
    self._getNextValue()
    assert a[0].getChannelIndex() == 0

    # Close animator settings 
    self._openAnimatorSettings()

def test_animatorWrap(cartavisInstance, cleanSlate):
    """
    Test that the Animator wrap setting returns to the first channel value after animating
    the last channel.
    """
    i = cartavisInstance.getImageViews() 
    a = cartavisInstance.getAnimatorViews()

    # Load an image
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')

    # Get the last channel value (should be the number of channels - 1)
    lastChannel = i[0].getChannelCount() - 1
    
    # Open animator settings 
    self._openAnimatorSettings()

    # Click the wrap radio button
    wrapButton = ImageUtil.locateCenterOnScreen('test_images/wrapButton.png')
    assert wrapButton != None
    pyautogui.click( x=wrapButton[0], y=wrapButton[1])

    # Set the image to the last channel value
    a[0].setChannel( lastChannel )

    # Click the increment button and get the current channel
    self._getNextValue()
    assert a[0].getChannelIndex() == 0
    self._getNextValue()
    assert a[0].getChannelIndex() == 1

    # Close animator settings 
    self._openAnimatorSettings()

def test_animatorReverse(cartavisInstance, , cleanSlate):
    """
    Test the Animator reverse setting.
    """
    i = cartavisInstance.getImageViews() 
    a = cartavisInstance.getAnimatorViews()

    # Load an image
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')

    # Get the last channel value (should be the number of channels - 1)
    lastChannel = i[0].getChannelCount() - 1
    
    # Open animator settings 
    self._openAnimatorSettings()

    # Click the reverse radio button
    reverseButton = ImageUtil.locateCenterOnScreen('test_images/reverseButton.png')
    assert reverseButton != None
    pyautogui.click( x=reverseButton[0], y=reverseButton[1])

    # Set the image to the last channel value
    a[0].setChannel( lastChannel )

    # Click the forward animate button on the tape deck
    self._animateForward()
    time.sleep(2)
    assert a[0].getChannelIndex() < lastChannel
    self._stopAnimation()

    # Close animator settings 
    self._openAnimatorSettings()
 
def test_animatorStepIncrement(cartavisInstance, cleanSlate):
    """
    Test that the Animator can be set to different step increment values.
    """
    i = cartavisInstance.getImageViews() 
    a = cartavisInstance.getAnimatorViews()

    # Load an image
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')

    # Open animator settings
    self._openAnimatorSettings()

    # Find the step increment spin box and change the step increment to 2
    stepIncrement = ImageUtil.locateCenterOnScreen('test_images/stepIncrement.png')
    pyautogui.click(x=stepIncrement[0]+40, y=stepIncrement[1])
    pyautogui.press('delete')
    pyautogui.typewrite('2')
    pyautougi.press('return')

    # Go to the next channel value 
    a[0].setChannel(0)
    self._getNextValue()
    assert a[0].getChannelIndex() == 2

    # Close animator settings 
    self._openAnimatorSettings()

def test_animatorForwardAnimation(cartavisInstance, cleanSlate):
    """
    Test that we can animate in the forward direction.
    """
    i = cartavisInstance.getImageViews() 
    a = cartavisInstance.getAnimatorViews()

    # Load an image
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')

    # Click on the foward animate button 
    self._animateFoward()
    time.sleep(2)
    assert a[0].getChannelIndex() > 0 
    self._stopAnimation()

def test_animatorReverseAnimation(cartavisInstance, cleanSlate):
    """
    Test that we can animate in the reverse directon.
    """
    i = cartavisInstance.getImageViews() 
    a = cartavisInstance.getAnimatorViews()

    # Load an image
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')

    # Get the last channel value (should be the number of channels - 1)
    lastChannel = i[0].getChannelCount() - 1

    # Set the image to the last channel value
    a[0].setChannel( lastChannel )

    # Click the reverse animate button on the tape deck
    reverseButton = ImageUtil.locateCenterOnScreen('test_images/reverseAnimation.png')
    pyautogui.click( x=reverseButton[0], y=reverseButton[1])
    time.sleep(2)
    assert a[0].getChannelIndex() < lastChannel
    self._stopAnimation()

def test_animatorStopAnimation(cartavisInstance, cleanSlate):
    """
    Test that we can stop animation.
    """
    i = cartavisInstance.getImageViews() 
    a = cartavisInstance.getAnimatorViews()

    # Load an image
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')

    # Click on the animate button
    self._animateFoward()
    time.sleep(2)
    self._stopAnimation()

    # Get the current channel value and check that it doesn't change
    channel = a[0].getChannelIndex()
    time.sleep(2)
    assert channel == a[0].getChannelIndex()

def test_animatorFrameIncrement(cartavisInstance, cleanSlate):
    """
    Test that we can increment the channel by one.
    """
    i = cartavisInstance.getImageViews() 
    a = cartavisInstance.getAnimatorViews()

    # Load an image
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')

    # Click the increment button 
    incrementButton = ImageUtil.locateCenterOnScreen('test_images/incrementButton.png')
    pyautogui.click( x=incrementButton[0], y=incrementButton[1])

    assert a[0].getChannelIndex() == 1

def test_animatorFrameDecrement(cartavisInstance, cleanSlate):
    """
    Test that we can decrement the channel by one.
    """
    i = cartavisInstance.getImageViews() 
    a = cartavisInstance.getAnimatorViews()

    # Load an image
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')

    # Get the last channel value (should be the number of channels - 1)
    lastChannel = i[0].getChannelCount() - 1

    # Click the decrement button 
    decrementButton = ImageUtil.locateCenterOnScreen('test_images/decrementButton.png')
    pyautogui.click( x=decrementButton[0], y=decrementButton[1])
    time.sleep(2)
    assert a[0].getChannelIndex() == (lastChannel - 1)

def test_animatorLastValue(cartavisInstance, cleanSlate):
    """
    Test that we can go to the last frame value of the image.
    """
    i = cartavisInstance.getImageViews() 
    a = cartavisInstance.getAnimatorViews()

    # Load an image
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')

    # Get the last channel value (should be the number of channels - 1)
    lastChannel = i[0].getChannelCount() - 1

    # Click on the last value button
    self.__getLastValue()
    assert a[0].getChannelIndex() == lastChannel 

def test_animatorFirstValue(cartavisInstance, cleanSlate):
    """
    Test that we can go to the first frame value of the image.
    """
    i = cartavisInstance.getImageViews() 
    a = cartavisInstance.getAnimatorViews()

    # Load an image
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')

    # Click on the first channel value 
    self._getFirstValue()
    assert a[0].getChannelIndex() == 0 
