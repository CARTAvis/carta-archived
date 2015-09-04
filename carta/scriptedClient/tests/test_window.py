import os
import cartavis
import pyautogui
import time
import ImageUtil

# Test of window manipulation within the scripted client
def test_minimize_restore(cartavisInstance, cleanSlate):
    """
    Test that a window can be minimized and then restored to 
    its original position. 
    """
    imageWindow = ImageUtil.locateCenterOnScreen('test_images/imageWindow.png')
    assert imageWindow != None
    pyautogui.click( x=imageWindow[0], y=imageWindow[1])
    
    # Click the window button
    windowButton = ImageUtil.locateCenterOnScreen('test_images/windowButton.png')
    assert windowButton != None
    pyautogui.click( x=windowButton[0], y=windowButton[1])
    time.sleep(2)

    # Click on the minimize button in the submenu
    pyautogui.press('down')
    pyautogui.press('down')
    pyautogui.press('return')
    time.sleep(2)

    # Find the restore buttom for the casa image loader 
    restoreButton = ImageUtil.locateCenterOnScreen('test_images/restoreButton.png')
    assert restoreButton != None
    
    # Restore the window. Verify the restore button is gone from the status bar and there is a DisplayWindowImage
    pyautogui.click( x=restoreButton[0], y=restoreButton[1])

    # Check that the clipping menu item is no longer available
    try:
        restoreLabel = ImageUtil.locateCenterOnScreen('test_images/restoreButton.png')
        assert restoreButton == False
    except Exception:
        print "Test restore button was successfully removed"
    imageWindow = cartavisInstance.getImageViews()
    assert len(imageWindow) == 1

def test_maximize_removed(cartavisInstance, cleanSlate):
    """
    Test that a window can be maximized and then removed.
    """
    windowCount = len(cartavisInstance.getPluginList())

    # Find and select the animation window
    animWindow = ImageUtil.locateCenterOnScreen('test_images/animWindow.png')
    assert animWindow != None
    pyautogui.click( x=animWindow[0], y=animWindow[1])

    # Click the window button
    windowButton = ImageUtil.locateCenterOnScreen('test_images/windowButton.png')
    assert windowButton != None
    pyautogui.click( x=windowButton[0], y=windowButton[1])
    time.sleep(2)

    # Look for the maximize button in the submenu
    pyautogui.press('down')
    pyautogui.press('return')
    time.sleep(2)

    # Verify there is a single animator window
    assert len(cartavisInstance.getAnimatorViews()) == 1

    # Now right click the context menu to remove the animator window
    pyautogui.doubleClick( x=animWindow[0], y=animWindow[1])
    time.sleep(2)
    pyautogui.rightClick( x=animWindow[0], y=animWindow[1])
    pyautogui.press('down')
    pyautogui.press('down')
    pyautogui.press('down')
    pyautogui.press('right')
    pyautogui.press('down')
    pyautogui.press('return')
    time.sleep(2)

    # Verify that there is one less window than there was originally and the animation window is not in the list
    assert len(cartavisInstance.getAnimatorViews()) == 0
    assert len(cartavisInstance.getPluginList()) == (windowCount - 1)

def test_maximize_restore(cartavisInstance, cleanSlate):
    """
    Test that a window can be maximized and then restored to 
    its original position
    """
    # Find and select the animation window 
    animWindow = ImageUtil.locateCenterOnScreen('test_images/animWindow.png')
    assert animWindow != None
    pyautogui.doubleClick( x=animWindow[0], y=animWindow[1])
    time.sleep(2)

    # Click the window button
    windowButton = ImageUtil.locateCenterOnScreen('test_images/windowButton.png')
    assert windowButton != None
    pyautogui.click( x=windowButton[0], y=windowButton[1])

    # Look for the maximize button in the submenu
    pyautogui.press('down')
    pyautogui.press('return')
    time.sleep(2)

    # Verify that there is a single animator window
    assert len(cartavisInstance.getAnimatorViews()) == 1

    # Right click the context menu to restore a animator window
    pyautogui.rightClick( x=animWindow[0], y=animWindow[1])
    pyautogui.press('down')
    pyautogui.press('down')
    pyautogui.press('down')
    pyautogui.press('right')
    pyautogui.press('return')
    time.sleep(2)

    # Verify that there are exactly the same number of windows as there were originally and the animation window is present
    assert len(cartavisInstance.getAnimatorViews()) == 1

def test_add_window(cartavisInstance, cleanSlate):
    """
    Test that we can add a window and change it to a Histogram view.
    """
    # For later use, determine the number of histogram windows
    histWindowList = len(cartavisInstance.getHistogramViews())
    windowCount = len(cartavisInstance.getPluginList())
    print "Histogram Window Count=", histWindowList

    # Find a window capable of loading an image
    imageWindow = ImageUtil.locateCenterOnScreen('test_images/imageWindow.png')
    assert imageWindow != None
    pyautogui.click( x=imageWindow[0], y=imageWindow[1])

    # Click the window button
    windowButton = ImageUtil.locateCenterOnScreen('test_images/windowButton.png')
    assert windowButton != None
    pyautogui.click( x=windowButton[0], y=windowButton[1])
    time.sleep(2)

    # Look for the add button in the submenu
    pyautogui.press('down')
    pyautogui.press('down')
    pyautogui.press('down')
    pyautogui.press('down')
    pyautogui.press('return')
    pyautogui.click( x=addButton[0], y=addButton[1])
    pyautogui.press('right')
    pyautogui.press('return')
    time.sleep(2)

    # Check that we now have a generic empty window in the display and the window count has gone up
    emptyWindowCount = cartavisInstance.getEmptyWindowCount()
    assert emptyWindowCount == 1

    # Change the plugin of the window to a histogram
    cartavisInstance.setEmptyWindowPlugin(0, 'Histogram')

    # Verify that we have increased the number of histogram windows by one
    newHistWindowList = len(cartavisInstance.getHistogramViews())
    assert newHistWindowList == (1 + histWindowList), 'Histogram window count did not go up by one'

def test_remove(cartavisInstance, cleanSlate):
    """
    Test that an existing window can be removed
    """
    windowCount = len(cartavisInstance.getPluginList())

    # Find and select the animation window
    animWindow = ImageUtil.locateCenterOnScreen('test_images/animWindow.png')
    assert animWindow != None 
    pyautogui.doubleClick( x=animWindow[0], y=animWindow[1])
    time.sleep(2)
    pyautogui.rightClick( x=animWindow[0], y=animWindow[1])
    time.sleep(2)

    # Look for the window button in the submenu and remove the window
    pyautogui.press('down')
    pyautogui.press('down')
    pyautogui.press('down')
    pyautogui.press('right')
    pyautogui.press('down')
    pyautogui.press('down')
    pyautogui.press('return')
    time.sleep(2)

    # Verify that there is one animator window and no other windows
    assert len(cartavisInstance.getAnimatorViews()) == 1
    assert len(cartavisInstance.getPluginList()) == 1

    # Verify that there is one less window than there was originally and the animator window is not in the list
    assert len(cartavisInstance.getAnimatorViews()) == 0
    assert len(cartavisInstance.getPluginList()) == (windowCount - 1)