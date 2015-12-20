import os 
import time
import Image
import carta.cartavis as cartavis
import pyautogui
import ImageUtil

def test_saveImage(cartavisInstance, cleanSlate):
    """
    Test that an image can be saved from the GUI.
    """
    cartavisInstance.setImageLayout()
    i = cartavisInstance.getImageViews()
    filename = '/tmp/saveImage.png'
    imageWindow = ImageUtil.locateCenterOnScreen('test_images/imageWindow.png')
    assert imageWindow != None
    i[0].loadFile(os.getcwd() + '/data/mexinputtest.fits')
    _saveImage(imageWindow)
    # Verify that the output image is of the desired size given the save
    # parameters.
    image = Image.open(filename)
    # The default size of the saved image is 400x500, but the aspect
    # ratio mode is Keep, so the output size should be 400x400.
    assert image.size == (400,400)
    os.remove(filename)
    # This next save is a regression test for issue #101.
    zoomLevel = 0.2 
    i[0].setZoomLevel(zoomLevel)
    _saveImage(imageWindow)
    image = Image.open(filename)
    assert image.size == (400*zoomLevel, 400*zoomLevel)

def _saveImage(imageWindow):
    """
    Brings up the save image dialog, presses the Save button, and then
    closes the dialog.

    Parameters
    ----------
    imageWindow:
        The image window which has already been located.
    """
    pyautogui.rightClick(x=imageWindow[0], y=imageWindow[1])
    pyautogui.press('down')
    pyautogui.press('down')
    pyautogui.press('down')
    pyautogui.press('down')
    pyautogui.press('down')
    pyautogui.press('down')
    pyautogui.press('return')
    saveButton = ImageUtil.locateCenterOnScreen(
                    'test_images/saveImageButton.png')
    assert saveButton != None
    pyautogui.click( x=saveButton[0], y=saveButton[1] )
    closeButton = ImageUtil.locateCenterOnScreen('test_images/closeButton.png')
    assert closeButton != None
    pyautogui.click( x=closeButton[0], y=closeButton[1] )
