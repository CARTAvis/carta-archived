import os 
import carta.cartavis as cartavis
import time
import pyautogui
import ImageUtil

def test_animator_channel(cartavisInstance, cleanSlate):
    """
    Set the channel animator to the last channel. Save a data snapshot.
    Set the channel animator back to 0, the first channel. Restore a data snapshot.
    """
    # Lad an image so there are a non-trivial number of channels.
    # At some point, this test will need to be rewritten to use a 
    # test image available where the tests are running.
    i = cartavisInstance.getImageViews()
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')
 
    # Find the last channel value 
    lastChannel = i[0].getChannelCount() - 1
    print 'lastChannel', lastChannel
    
    # Set the channel animator to the last channel 
    a = cartavisInstance.getAnimatorViews()
    a[0].setChannel( lastChannel )

    # Save a snapshot of the application. Make sure data is checked and 
    # layout and preferences are not checked
    s = cartavisInstance.newSnapshot('some_session_id', 'tSnapshotData', False, False, True, '')
    s.save()

    # Set the channel animator back to 0
    a[0].setChannel(0)

    # Restore the snapshot
    s[0].restore()
    time.sleep(2)

    # Check that the channel value is at the last channel
    assert a[0].getChannelIndex() == lastChannel
    s[0].delete()

def test_image_load(cartavisInstance, cleanSlate):
    """
    Load a particular image. Save a data snapshot.
    Load a new image. Restore a data snapshot.
    Test that the original image is loaded, but the second one is not.
    """
    # Show the image animator 
    a = cartavisInstance.getAnimatorViews()
    a[0].showImageAnimator()

    # Get the upper spin value of the image
    i = cartavisInstance.getImageViews()
    upperSpin = len(i[0].getImageNames()) - 1

    # Load an image 
    i[0].loadLocalFile(os.getcwd + '/data/N15693D.fits')

    # Save a snapshot of the application. Make sure data is checked and 
    # layout and preferences are not checked 
    s = cartavisInstance.newSnapshot('some_session_id', 'tSnapshotData', False, False, True, '')
    s.save()

    # Load another image 
    i[0].loadLocalFile(os.getcwd + '/data/aH.fits')

    # Verify that there are two images loaded 
    assert len(i[0].getImageNames()) == 2
    
    # Select tSnapshotData in the restore combo box
    s[0].restore()
    
    # Verify that only the original image is loaded
    assert len(i.getImageNames()) == 1
    s[0].delete()

def test_analysis_saveRestore(cartavisInstance, cleanSlate):
    """
    Take a snapshot of the analysis layout. Change to an image layout. Restore 
    the analysis layout
    """
    # Save a snapshot of the analysis layout. Make sure preferences and data are not checked;
    # layout is checked.
    s = cartavisInstance.newSnapshot('some_session_id', 'tSnapshotLayout', True, False, False, '')
    s.save()
    time.sleep(2)

    # Change to an image layout 
    cartavisInstance.setImageLayout()

    # Restore the analysis layout 
    s[0].restore()
    time.sleep(2)

    assert len(cartavisInstance.getAnimatorViews()) == 1
    assert len(cartavisInstance.getImageViews()) == 1
    assert len(cartavisInstance.getColormapViews()) == 1
    assert len(cartavisInstance.getHistogramViews()) == 1

def test_restore_missing(cartavisInstance, cleanSlate):
    """
    The purpose of this test is to test that if we try to restore preferences for 
    a widget that is not present there are no problems. 
    We verify an animator is present. We save the preferences. We remove the animator
    We verify that there is not a problem by checking error status is empty and window
    count remains the same with no animator.
    """
    # Verify that there is just one animator
    assert len(cartavisInstance.getAnimatorViews()) == 1

    # Save a snapshot. Make sure preferences are checked and 
    # layout and data are not checked
    s = cartavisInstance.newSnapshot('some_session_id','tSnapshotPreferences', False, True, False, '')
    s.save()

    # Locate the animator and bring up the right-context menu,
    # changing to a CasaImageLoader.
    animWindow = ImageUtil.locateCenterOnScreen('test_images/animWindow.png')
    pyautogui.rightClick( x=animWindow[0], y=animWindow[1])
    time.sleep(2)
    pyautogui.press('down')
    pyautogui.press('right')    
    pyautogui.press('return')
    time.sleep(2)

    # Verify that there are no animation windows 
    assert len(cartavisInstance.getAnimatorViews()) == 0

    # Restore the preferences
    s[0].restore()
    time.sleep(2)

    # No way to check for errors, hence, take a screenshot
    pyautogui.screenshot('layout_check/checkNoErrors.png')
    s[0].delete()

def test_animator_jump(cartavisInstance, cleanSlate):
    """
    Set the channel animator to jump end behaviour. Save a preference snapshot.
    Set the channel animator to wrap end behaviour. Restore the preference snapshot.
    Check that the animator reverts back to jump behaviour.
    """
    i = cartavisInstance.getImageViews()
    a = cartavisInstance.getAnimatorViews()

    # Set the animator to jump 
    settingsButton = ImageUtil.locateCenterOnScreen( "test_images/animatorSettingsCheckBox.png")
    assert settingsButton != None
    pyautogui.click( x=settingsButton[0], y=settingsButton[1])

    # Click the jump radio button
    jumpButton = ImageUtil.locateCenterOnScreen('test_images/jumpButton.png')
    assert jumpButton != None
    pyautogui.doubleClick( x=jumpButton[0], y=jumpButton[1])

    # Save a snapshot. Make sure preferences are checked and layout and data are not checked
    s = cartavisInstance.newSnapshot('some_session_id','tSnapshotPreferences', False, True, False, '')
    s.save()

    # Find the wrap radio button in the animator settings and click it 
    wrapButton = ImageUtil.locateCenterOnScreen('test_images/wrapButton.png')
    assert wrapButton != None
    pyautogui.click( x=wrapButton[0], y=wrapButton[1])

    # Restore the preferences
    s[0].restore()
    time.sleep(2)

    # Verify the animator jump end behaviour is checked by checking end behaviour
    a[0].setChannel(0)
    lastChannel = i[0].getChannelCount() - 1

    # Go to the next channel 
    incrementButton = ImageUtil.locateCenterOnScreen( "test_images/incrementButton.png")
    assert incrementButton != None 
    pyautogui.click( x=incrementButton[0], y=incrementButton[1])
    time.sleep(2)

    assert a[0].getChannelIndex() == lastChannel
    s[0].delete()

def test_global_prefs(cartavisInstance, cleanSlate):
    """
    Test that object specific settings can also act globally.
    Set the animator to jump. Save the preferences. Open two animators. Restore the preferences.
    Check the second animator is also set to jump.
    """
    # Set the animator to jump 
    settingsButton = ImageUtil.locateCenterOnScreen( "test_images/animatorSettingsCheckBox.png")
    assert settingsButton != None
    pyautogui.click( x=settingsButton[0], y=settingsButton[1])

    # Click the jump radio button
    jumpButton = ImageUtil.locateCenterOnScreen('test_images/jumpButton.png')
    assert jumpButton != None
    pyautogui.doubleClick( x=jumpButton[0], y=jumpButton[1])

    # Save a snapshot. Make sure preferences are checked and 
    # layout and data are not checked
    s = cartavisInstance.newSnapshot('some_session_id','tSnapshotPreferences', False, True, False, '')
    s.save()

    # Find an image window and change it into an animator
    imageWindow = ImageUtil.locateCenterOnScreen('test_images/imageWindow.png')
    assert imageWindow != None
    pyautogui.rightClick( x=imageWindow[0], y=imageWindow[1])
    pyautogui.press('right')
    pyautogui.press('right')
    pyautogui.press('down')
    pyautogui.press('return')
    time.sleep(2)

    # Find the settings button on the animator and click it so the jump will be visible 
    settingsButton = ImageUtil.locateCenterOnScreen('test_images/settingsCheckBox.png')
    assert settingsButton != None
    pyautogui.click( x=settingsButton[0], y=settingsButton[1])

    # Restore the preferences
    s[0].restore()

    # Check that both animators are not displaying jump
    # Verify that the animator jump end behaviour is checked in the screenshot after the tests have been run
    pyautogui.screenshot('layout_check/bothAnimatorsJump.png')
    s[0].delete()
