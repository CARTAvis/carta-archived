import os
import cartavis
import pyautogui
import time
import ImageUtil

def test_load_image(cartavisInstance, cleanSlate):
    """
    Test that an image can be loaded and then closed
    """
    # Load an image in the image viewer
    i = cartavisInstance.getImageViews()
    a = cartavisInstance.getAnimatorViews()
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')

    # Check that the channel count is above 1
    channelCount = i[0].getChannelCount() 
    # Check that we can set the animator channel value 
    assert a[0].setChannel( int(channelCount)-1 ) != [u'']

    i[0].closeImage('N15693D.fits')

def test_animator_to_casaimageloader(cartavisInstance, cleanSlate):
    """
    Test that we can change an animator to a CasaImageLoader
    """
    # Get the animation window count and make sure it is non-zero
    animWindowList = len(cartavisInstance.getAnimatorViews())
    assert animWindowList > 0 

    # Get the image window count 
    imageWindowList = len(cartavisInstance.getImageViews())

    # Locate an animator window and bring up the right-context menu,
    # changing to a CasaImageLoader.
    animatorWindow = ImageUtil.locateCenterOnScreen('test_images/animWindow.png')
    pyautogui.doubleClick( x=animatorWindow[0], y=animatorWindow[1])
    time.sleep(2)
    pyautogui.rightClick( x=animatorWindow[0], y=animatorWindow[1])
    pyautogui.press('right')
    pyautogui.press('right')
    pyautogui.press('return')
    time.sleep(2)

    # Verify that the animation count has gone down by one and the image count 
    # has increased by one
    newAnimWindowCount = len(cartavisInstance.getAnimatorViews())
    newImageWindowCount = len(cartavisInstance.getImageViews())
    assert newAnimWindowCount == animWindowList - 1
    assert newImageWindowCount == imageWindowList + 1