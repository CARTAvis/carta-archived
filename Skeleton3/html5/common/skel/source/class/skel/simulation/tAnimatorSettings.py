import Util
import time
import unittest
import tAnimator
import selectBrowser
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.by import By

# Tests of Animator Settings functionality
class tAnimatorSettings(tAnimator.tAnimator):

    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp( self, browser )

    # Test that we can add the add/remove Animator buttons to the toolbar if they are 
    # not already there. Then test that we can check/uncheck them and have the corresponding
    # animator added/removed
    def test_animatorAddRemove(self):
        driver = self.driver

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Click on Animator window so its actions will be enabled
        animWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")))
        self.assertIsNotNone( animWindow, "Could not find Animator window")
        ActionChains(driver).click( animWindow ).perform()
        # Make sure the Animation window is enabled by clicking an element within the window
        channelText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ChannelIndexText")))
        ActionChains(driver).click( channelText ).perform()

        # Right click the toolbar to bring up the context menu 
        toolBar = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Menu.ToolBar']")))
        self.assertIsNotNone( toolBar, "Could not find the tool bar")
        ActionChains(driver).context_click(toolBar).perform()

        # Click the customize item on the menu
        customizeButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Customize...']/..")))
        self.assertIsNotNone( customizeButton, "Could not find the customize button in context")
        ActionChains(driver).click( customizeButton ).perform()

        # First make sure animator is checked 
        animateButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Animate']/preceding-sibling::div/div")))
        self.assertIsNotNone( animateButton, "Could not find animate button in customize dialog")
        styleAtt = animateButton.get_attribute( "style");
        if not "checked.png" in styleAtt:
            print "Clicking animate to make buttons visible on tool bar"
            animateParent = animateButton.find_element_by_xpath( '..' )
            driver.execute_script( "arguments[0].scrollIntoView(true);", animateParent)
            ActionChains(driver).click( animateParent ).perform()

        # Verify both the channel and image checkboxes are on the toolbar
        channelCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Channel']/following-sibling::div[@class='qx-checkbox']")))
        self.assertIsNotNone( channelCheck, "Could not find animate channel check box on tool bar")
        animateCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Image']/following-sibling::div[@class='qx-checkbox']")))
        self.assertIsNotNone( animateCheck, "Could not find animate image check box on tool bar")

        # Uncheck both buttons
        channelChecked = self._isChecked( channelCheck )
        print 'Channel checked', channelChecked
        if channelChecked:
            self._click( driver, channelCheck )
        animateChecked = self._isChecked( animateCheck )
        print 'Animate checked', animateChecked
        if animateChecked:
            self._click( driver, animateCheck )
            
        # Verify that the animation window has no animators.
        self._verifyAnimationCount( animWindow, 0)
        
        # Check the image animate button and verify that the image animator shows up
        self._click( driver, animateCheck )
        imageAnimator = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.boundWidgets.Animator']/div/div[text()='Image']")))
        self.assertIsNotNone( imageAnimator, "Image animator did not appear")
        self._verifyAnimationCount( animWindow, 1)
        
        # Check the channel animator button and verify there are now two animators, one channel, one image.
        self._click( driver, channelCheck )
        channelAnimator = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.boundWidgets.Animator']/div/div[text()='Channel']")))
        self.assertIsNotNone( channelAnimator, "Channel animator did not appear")
        self._verifyAnimationCount( animWindow, 2 )

    # Test that the Channel Animator will update when the window image is switched
    def test_channelAnimatorChangeImage(self):
        driver = self.driver 
        timeout = selectBrowser._getSleep()

        # Load two images
        # The images have different numbers of channels
        Util.load_image( self, driver, "Default")
        Util.load_image( self, driver, "m31_cropped.fits")

        # Show the Image Animator
        channelText = driver.find_element_by_id("ChannelIndexText")
        ActionChains(driver).context_click( channelText ).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
        time.sleep( timeout )

        # Go to the first image 
        firstValueButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.boundWidgets.Animator'][2]/div[3]/div/div[1]")))
        self.assertIsNotNone( firstValueButton, "Could not find button to go to the first valid value")
        driver.execute_script( "arguments[0].scrollIntoView(true);", firstValueButton)
        ActionChains(driver).click( firstValueButton ).perform()
        time.sleep( timeout )

        # Go to the last channel of the image
        lastChannelButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.boundWidgets.Animator'][1]/div[3]/div/div[7]")))
        self.assertIsNotNone( lastChannelButton, "Could not find button to go to the first valid value")
        driver.execute_script( "arguments[0].scrollIntoView(true);", lastChannelButton)
        ActionChains(driver).click( lastChannelButton ).perform()
        time.sleep( timeout )

        # Get the last channel value of the first image
        firstImageChannelValue = self._getChannelValue( driver )

        # Go to the next image
        nextImageButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ImageTapeDeckIncrement")))
        self.assertIsNotNone( nextImageButton, "Could not find Image Tape Deck Increment button")
        driver.execute_script( "arguments[0].scrollIntoView(true);", nextImageButton)
        nextImageButton.click()
        time.sleep( timeout )

        # Go to the last channel of the image
        lastChannelButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.boundWidgets.Animator'][1]/div[3]/div/div[7]")))
        self.assertIsNotNone( lastChannelButton, "Could not find button to go to the first valid value")
        driver.execute_script( "arguments[0].scrollIntoView(true);", lastChannelButton)
        ActionChains(driver).click( lastChannelButton ).perform()
        time.sleep( timeout )

        # Get the channel upper spin box value of the second image
        # Check that the upper spin box value updated
        # Get the channel upper spin box value of the first image
        secondImageChannelValue = self._getChannelValue( driver )
        self.assertNotEqual( int(secondImageChannelValue), int(firstImageChannelValue), "Channel value did not update after changing image in window")

    # Test that the Animator jump setting animates the first and last channel values
    # Under default settings, it takes roughly 2 seconds for the channel to change by 1
    def test_animatorJump(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "aJ.fits")
        Util.load_image( self, driver, "Default")

        # Record last channel value of the test image
        self._getLastValue( driver )
        lastChannelValue = self._getChannelValue( driver )

        # Record the first channel value of the test image
        self._getFirstValue( driver )
        firstChannelValue = self._getChannelValue( driver )

        print "Testing Channel Animator Jump Setting..."
        print "First channel value:", firstChannelValue, "Last channel value:", lastChannelValue

        # Open settings
        self._openSettings( driver )

        # In settings, click the Jump radio button. Scroll into view if button is not visible
        jumpButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.RadioButton']/div[text()='Jump']/following-sibling::div")))
        self.assertIsNotNone( jumpButton, "Could not find the jump radio button in settings")
        driver.execute_script( "arguments[0].scrollIntoView(true);", jumpButton)
        ActionChains(driver).click( jumpButton ).perform()

        # Click the forward animate button
        # Allow the image to animate for 2 seconds
        self._animateForward( driver )
        time.sleep(2)

        # Check that the channel is at the last channel value
        currChannelValue = self._getChannelValue( driver )
        print "Current channel", currChannelValue
        self.assertEqual( int(lastChannelValue), int(currChannelValue), "Channel Animator did not jump to last channel value")

        # Allow the image to animate for another 2 seconds
        time.sleep(2)
        currChannelValue = self._getChannelValue( driver ) 
        print "Current channel", currChannelValue
        self.assertEqual( int(firstChannelValue), int(currChannelValue), "Channel Animator did not jump to first channel value")

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Open settings
        self._openSettings( driver )

        # Record the last image value
        self._getLastValue( driver )
        lastImageValue = self._getImageValue( driver )

        # Record the first image value
        self._getFirstValue( driver )
        firstImageValue = self._getImageValue( driver )

        print "Testing Image Animator Jump Setting..."
        print "First image value:", firstImageValue, "Last image value:", lastImageValue

        # In settings, click the Jump radio button. Scroll into view if button is not visible
        jumpButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.RadioButton']/div[text()='Jump']/following-sibling::div")))
        self.assertIsNotNone( jumpButton, "Could not find the jump radio button in settings")
        driver.execute_script( "arguments[0].scrollIntoView(true);", jumpButton)
        ActionChains(driver).click( jumpButton ).perform()

        # Click the forward animate button
        # Allow the animation for 2 seconds
        self._animateForward( driver )
        time.sleep(2)

        # Check that the Animator is at the last image value
        currImageValue = self._getImageValue( driver )
        print "Current image", currImageValue
        self.assertEqual( int(lastImageValue), int(currImageValue), "Image Animator did not jump to last image" )

        # Allow the animation for another 2 seconds
        time.sleep(2)
        currImageValue = self._getImageValue( driver )
        print "Current image", currImageValue
        self.assertEqual( int(firstImageValue), int(currImageValue), "Image Animator did not jump to first image")

    # Test that the Animator wrap setting returns to the first channel value 
    # after animating the last channel. Under default settings, it takes roughly 2 
    # seconds for the channel to change by 1
    def test_animatorWrap(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "aJ.fits")
        Util.load_image( self, driver, "Default")

        # Open settings
        self._openSettings( driver )

        # Go to first channel value and record the first channel value of the test image
        self._getFirstValue( driver )
        firstChannelValue = self._getChannelValue( driver )

        # Go to last channel value and record the last channel value of the test image 
        self._getLastValue( driver )
        lastChannelValue = self._getChannelValue( driver )

        print "Testing Channel Animator Wrap Setting..."
        print "First channel value:", firstChannelValue, "Last channel value:", lastChannelValue

        # In settings, click the Wrap radio button. Scroll into view if button is not visible
        wrapButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.RadioButton']/div[text()='Wrap']/following-sibling::div")))
        self.assertIsNotNone( wrapButton, "Could not find wrap radio button in settings")
        driver.execute_script( "arguments[0].scrollIntoView(true);", wrapButton)
        ActionChains(driver).click( wrapButton ).perform()

        # Click the forward animate button
        # Allow the image to animate for 2 seconds
        self._animateForward( driver )
        time.sleep(2)

        # Check that the channel is at the first channel value
        currChannelValue = self._getChannelValue( driver )
        print "Current channel", currChannelValue
        self.assertEqual( int(firstChannelValue), int(currChannelValue), "Channel Animator did not wrap to first channel value")

        # Allow the image to animate for another 2 seconds
        time.sleep(2)
        currChannelValue = self._getChannelValue( driver ) 
        print "Current channel", currChannelValue
        self.assertGreater( int(currChannelValue), int(firstChannelValue), "Channel did not increase after animating first channel value")

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Open settings
        self._openSettings( driver )

        # Record the first image value
        self._getFirstValue( driver )
        firstImageValue = self._getImageValue( driver )

        # Go to the last image and record the last image value
        self._getLastValue( driver )
        lastImageValue = self._getImageValue( driver )

        print "Testing Image Animator Wrap..."
        print "First image value:", firstImageValue, "Last image value:", lastImageValue

        # In settings, click the Wrap radio button. Scroll into view if button is not visible
        wrapButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.RadioButton']/div[text()='Wrap']/following-sibling::div")))
        self.assertIsNotNone( wrapButton, "Could not find wrap radio button in settings")
        driver.execute_script( "arguments[0].scrollIntoView(true);", wrapButton)
        ActionChains(driver).click( wrapButton ).perform()

        # Click the forward animate button
        # Allow the animation for 2 seconds
        self._animateForward( driver )
        time.sleep(2)

        # Check that the animator is at the first image value
        currImageValue = self._getImageValue( driver )
        print "Current image", currImageValue
        self.assertEqual( int(firstImageValue), int(currImageValue), "Image Animator did not wrap to first image")

        # Allow the animation for another 2 seconds
        time.sleep(2)
        currImageValue = self._getImageValue( driver )
        print "Current image", currImageValue
        self.assertGreater( int(currImageValue), int(firstImageValue), "Image value did not increase after animating first image")

    # Test that the Animator reverse setting animates in the reverse direction after 
    # reaching the last channel value. Under default settings, it takes roughly 4 seconds 
    # for the channel to reverse direction from the last channel
    def test_animatorReverse(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "aJ.fits")
        Util.load_image( self, driver, "Default")

        # Open settings
        self._openSettings( driver )

        # Go to last channel value and record the last channel value of the test image 
        self._getLastValue( driver )
        lastChannelValue = self._getChannelValue( driver )

        print "Testing Channel Animator Reverse Setting..."
        print "Last channel value:", lastChannelValue

        # In settings, click the Reverse radio button. Scroll into view if button is not visible
        reverseButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.RadioButton']/div[text()='Reverse']/following-sibling::div")))
        self.assertIsNotNone( reverseButton, "Could not find reverse radio button in settings")
        driver.execute_script( "arguments[0].scrollIntoView(true);", reverseButton)
        ActionChains(driver).click( reverseButton ).perform()
        time.sleep(2)

        # Click the forward animate button
        # Allow the image to animate for 4 seconds (takes 4 seconds to reverse direction)
        self._animateForward( driver )
        time.sleep(4)

        # Check that the current channel value is less than the last channel value
        currChannelValue = self._getChannelValue( driver )
        print "Current channel", currChannelValue
        self.assertGreater( int(lastChannelValue), int(currChannelValue), "Channel Animator did not reverse direction after animating last channel value")

        # Stop animation. Scroll into view if stop button cannot be seen
        stopButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.Button'][3]")))
        self.assertIsNotNone( stopButton, "Could not find stop button")
        driver.execute_script( "arguments[0].scrollIntoView(true);", stopButton)
        ActionChains(driver).click( stopButton ).perform()

        # Go to first channel value and record the first channel value of the test image
        self._getFirstValue(driver)
        firstChannelValue = self._getChannelValue( driver )
        print "First channel value:", firstChannelValue

        # Click the forward animate button
        # Allow image to animate for 2 seconds
        self._animateForward(driver)
        time.sleep(2)

        # Check that the channel value is at a higher value than the first channel value
        currChannelValue = self._getChannelValue( driver )
        print "Current channel", currChannelValue
        self.assertGreater( int(currChannelValue), int(firstChannelValue), "Channel Animator did not increase channel after animating first channel value")

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Open settings
        self._openSettings( driver )

        # Go to the last image and record the last image value
        self._getLastValue( driver )
        lastImageValue = self._getImageValue( driver )

        print "Testing Image Animator Reverse Setting..."
        print "Last image value:", lastImageValue

        # In settings, click the Reverse radio button. Scroll into view if button is not visible
        reverseButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.RadioButton']/div[text()='Reverse']/following-sibling::div")))
        self.assertIsNotNone( reverseButton, "Could not find reverse radio button in settings")
        driver.execute_script( "arguments[0].scrollIntoView(true);", reverseButton)
        ActionChains(driver).click( reverseButton ).perform()

         # Click the forward animate button
        # Allow the image to animate for 4 seconds (takes 4 seconds to reverse direction)
        self._animateForward( driver )
        time.sleep(4)

        # Check that the current image value is less than the last image value
        currImageValue = self._getImageValue( driver )
        print "Current image", currImageValue
        self.assertGreater( int(lastImageValue), int(currImageValue), "Image Animator did not reverse direction after animating the last image")

        # Stop animation. Scroll into view if stop button cannot be seen
        stopButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.Button'][3]")))
        self.assertIsNotNone( stopButton, "Could not find stop button")
        driver.execute_script( "arguments[0].scrollIntoView(true);", stopButton)
        ActionChains(driver).click( stopButton ).perform()

        # Go to the first image and record the first image value
        self._getFirstValue(driver)
        firstImageValue = self._getImageValue( driver )
        print "First image value:", firstImageValue

        # Click the forward animate button
        # Allow the animation for 2 seconds
        self._animateForward( driver )
        time.sleep(2)

        # Check that the image value is higher than the first image value
        currImageValue = self._getImageValue( driver )
        print "Current image", currImageValue
        self.assertGreater( int(currImageValue), int(firstImageValue), "Image Animator did not increase image value after animating first image")

    # Test that adjustment of Animator rate will speed up/slow down channel animation
    # Under default settings, it takes roughly 2 seconds for the channel to change by 1
    def test_animatorChangeRate(self):
        driver = self.driver 

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "aJ.fits")
        Util.load_image( self, driver, "Default")

        # Open settings
        self._openSettings( driver )

        # Go to first channel value and record the first channel value of the test image
        self._getFirstValue( driver )
        firstChannelValue = self._getChannelValue( driver )

        print "Testing Channel Animator Rate Setting..."
        print "First channel value:", firstChannelValue
        print "Default Rate = 20, New Rate = 50"

        # Allow image to animate for 2 seconds
        self._animateForward( driver )
        time.sleep(2)
        defaultRateValue = self._getChannelValue( driver )
        print "defaultRateValue", defaultRateValue

        # Stop animation. Scroll into view if the stop button cannot be seen
        stopButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.Button'][3]")))
        self.assertIsNotNone( stopButton, "Could not find stop button")
        driver.execute_script( "arguments[0].scrollIntoView(true);", stopButton)
        ActionChains(driver).click( stopButton ).perform()

        # Change the rate to 50
        rateText = driver.find_element_by_xpath("//div[@qxclass='skel.boundWidgets.Animator']/div[4]/div[7]/input") 
        self.assertIsNotNone( rateText, "Could not find rate text to set the speed of the animation")
        driver.execute_script( "arguments[0].scrollIntoView(true);", rateText)
        rateValue = Util._changeElementText(self, driver, rateText, 50)

        # Go to first channel value and animate for 2 seconds
        self._getFirstValue( driver )
        self._animateForward( driver )
        time.sleep(2)

        # The channel should be at a higher channel value than the default rate value 
        newRateValue = self._getChannelValue( driver )
        print "newRateValue", newRateValue
        self.assertGreater( int(newRateValue), int(defaultRateValue), "Rate value did not increase speed of channel animation")

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Open settings
        self._openSettings( driver )

        # Go to the first image and record the first image value
        self._getFirstValue(driver)
        firstImageValue = self._getImageValue( driver )
        
        print "Testing Image Animator Rate Setting..."
        print "First image value:", firstImageValue
        print "Default Rate = 20, New Rate = 50"

        # Allow animation for 2 seconds
        self._animateForward( driver )
        time.sleep(2)
        defaultRateValue = self._getImageValue( driver )
        print "defaultRateValue:", defaultRateValue

        # Stop animation. Scroll into view if the stop button cannot be seen
        stopButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.Button'][3]")))
        self.assertIsNotNone( stopButton, "Could not find stop button")
        driver.execute_script( "arguments[0].scrollIntoView(true);", stopButton)
        ActionChains(driver).click( stopButton ).perform()

        # Change the rate to 50
        rateText = driver.find_element_by_xpath( "//div[@qxclass='skel.boundWidgets.Animator']/div[4]/div[7]/input")
        self.assertIsNotNone( rateText, "Could not find rate text to set the speed of the animation")
        driver.execute_script( "arguments[0].scrollIntoView(true);", rateText)
        rateValue = Util._changeElementText(self, driver, rateText, 45)

        # Go back to first image value and animate for 2 seconds
        self._getFirstValue(driver)
        self._animateForward(driver)
        time.sleep(2)

        # The image should be at a higher image value than the default image value
        newRateValue = self._getImageValue( driver )
        print "newRateValue:", newRateValue
        self.assertGreater( int(newRateValue), int(defaultRateValue), "Rate value did not increase speed of image animation")

    # Test that the Channel Animator Rate does not exceed boundary values 
    def test_animatorRateBoundary(self):
        driver = self.driver 

         # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Open settings
        self._openSettings( driver )

        # Find and click on the rate text. Scroll into view if not visible
        rateText = driver.find_element_by_xpath( "//div[@qxclass='skel.boundWidgets.Animator']/div[4]/div[7]/input") 
        self.assertIsNotNone( rateText, "Could not find rate text to set the speed of the animation")
        driver.execute_script( "arguments[0].scrollIntoView(true);", rateText)

        # Test that the animation rate does not exceed boundary values (1 to 100)
        # Test that the input of a negative value is not accepted
        rateValue = Util._changeElementText( self, driver, rateText, -32)
        self.assertGreaterEqual(int(rateValue), 0, "Rate value is negative")

        # Test that the input of a value over 100 is not accepted
        rateValue = Util._changeElementText( self, driver, rateText, 200)
        self.assertEqual(int(rateValue), 100, "Rate value is greater than 100")

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Open settings
        self._openSettings( driver )

        # Find and click on the rate text. Scroll into view if not visible
        rateText = driver.find_element_by_xpath( "//div[@qxclass='skel.boundWidgets.Animator']/div[4]/div[7]/input")
        self.assertIsNotNone( rateText, "Could not find rate text to set the speed of the animation")
        driver.execute_script( "arguments[0].scrollIntoView(true);", rateText)

        # Test that the animation rate does not exceed boundary values (1 to 100)
        # Test that the input of a negative value is not accepted
        rateValue = Util._changeElementText( self, driver, rateText, -32)
        self.assertGreaterEqual(int(rateValue), 0, "Rate value is negative")

        # Test that the input of a value over 100 is not accepted
        rateValue = Util._changeElementText( self, driver, rateText, 200)
        self.assertEqual(int(rateValue), 100, "Rate value is greater than 100")

    # Test that the Channel Animator Step Increment does not exceed boundary values 
    def test_animatorStepBoundary(self):
        driver = self.driver 

         # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Open settings
        self._openSettings( driver )

        # Find and click the step increment textbox
        stepIncrementText = driver.find_element_by_xpath( "//div[@qxclass='skel.boundWidgets.Animator']/div[4]/div[2]/input")
        self.assertIsNotNone( stepIncrementText, "Could not find step increment textbox")
        driver.execute_script( "arguments[0].scrollIntoView(true);", stepIncrementText)

        # Test that the animation rate does not exceed boundary values (1 to 100)
        # Test that the input of a negative value is not accepted
        stepValue = Util._changeElementText(self, driver, stepIncrementText, -50)
        self.assertGreaterEqual(int(stepValue), 0, "Step increment value is negative")

        # Test that the input of a value over 100 is not accepted
        stepValue = Util._changeElementText( self, driver, stepIncrementText, 200)
        self.assertEqual( int(stepValue), 100, "Step increment value is greater than 100")

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Open settings
        self._openSettings( driver )

        # Find and click the step increment textbox
        stepIncrementText = driver.find_element_by_xpath( "//div[@qxclass='skel.boundWidgets.Animator']/div[4]/div[2]/input")
        self.assertIsNotNone( stepIncrementText, "Could not find step increment textbox")
        driver.execute_script( "arguments[0].scrollIntoView(true);", stepIncrementText)

        # Test that the animation rate does not exceed boundary values (1 to 100)
        # Test that the input of a negative value is not accepted
        stepValue = Util._changeElementText(self, driver, stepIncrementText, -50)
        self.assertGreaterEqual(int(stepValue), 0, "Step increment value is negative")

        # Test that the input of a value over 100 is not accepted
        stepValue = Util._changeElementText( self, driver, stepIncrementText, 200)
        self.assertEqual( int(stepValue), 100, "Step increment value is greater than 100")

    # Test that the Channel Animator can be set to different step increment values
    def test_animatorStepIncrement(self):
        driver = self.driver 

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "aJ.fits")
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "Default") 

        # Open settings
        self._openSettings( driver )

        # Find and click the step increment textbox
        stepIncrementText = driver.find_element_by_xpath( "//div[@qxclass='skel.boundWidgets.Animator']/div[4]/div[2]/input")
        self.assertIsNotNone( stepIncrementText, "Could not find step increment textbox")
        driver.execute_script( "arguments[0].scrollIntoView(true);", stepIncrementText)

        # Change the step increment spin box value to 2
        stepValue = Util._changeElementText( self, driver, stepIncrementText, 2)

        # Go to first channel value and record the first channel value of the test image
        self._getFirstValue( driver )
        firstChannelValue = self._getChannelValue( driver )

        print "Testing Channel Animator Step Increment Setting..."
        print "First channel value:", firstChannelValue
        print "Step Increment = 2"

        # Allow image to animate for 2 seconds
        self._animateForward( driver )
        time.sleep(2)

        # Check that the channel value increases by a step increment of 2 
        currChannelValue = self._getChannelValue( driver )
        print "Current channel", currChannelValue
        self.assertEqual( int(currChannelValue), 2, "Channel Animator did not increase by a step increment of 2")

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Open settings
        self._openSettings( driver )

        # Find and click the step increment textbox
        stepIncrementText = driver.find_element_by_xpath( "//div[@qxclass='skel.boundWidgets.Animator']/div[4]/div[2]/input")
        self.assertIsNotNone( stepIncrementText, "Could not find step increment textbox")
        driver.execute_script( "arguments[0].scrollIntoView(true);", stepIncrementText)

        # Change the step increment spin box value to 2
        stepValue = Util._changeElementText( self, driver, stepIncrementText, 2)

        # Record the first image value
        self._getFirstValue( driver )
        firstImageValue = self._getImageValue( driver )

        print "Testing Image Animator Step Increment Setting..."
        print "First image value:", firstImageValue
        print "Step Increment = 2"

        # Allow animation for 2 seconds
        self._animateForward( driver )
        time.sleep(2)

        # Check that the image value increases by a step increment value of 2
        currImageValue = self._getImageValue( driver )
        print "Current image:", currImageValue
        self.assertEqual( int(currImageValue), 2, "Image Animator did not increase by a step value of 2")

    # Test that the Channel Animator increases by one frame when the increase frame button is pressed
    def test_animatorIncreaseFrame(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "aJ.fits")
        Util.load_image( self, driver, "Default")

        # Go to the first channel value and record the frame value
        self._getFirstValue( driver )
        firstChannelValue = self._getChannelValue( driver )

        # Find the increment by one button on the Channel Animator Tape Deck and click it
        incrementButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ChannelTapeDeckIncrement")))
        self.assertIsNotNone( incrementButton, "Could not find button to increment the channels")
        driver.execute_script( "arguments[0].scrollIntoView(true);", incrementButton)
        ActionChains(driver).click( incrementButton ).perform()
        time.sleep( timeout )

        # Check that the channel text box value is now 1
        currChannelValue = self._getChannelValue( driver )
        print "Check increase frame..."
        print "oldChannelValue= 0 newChannelValue=", currChannelValue
        self.assertEqual( int(currChannelValue), int(firstChannelValue)+1, "Failed to increment Channel Animator")

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Record the first image value
        self._getFirstValue( driver )
        firstImageValue = self._getImageValue( driver )

        # Find the increment by one button on the Animator Tape Deck and click it
        incrementButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ImageTapeDeckIncrement")))
        self.assertIsNotNone( incrementButton, "Could not find button to increment the image")
        driver.execute_script( "arguments[0].scrollIntoView(true);", incrementButton)
        ActionChains(driver).click( incrementButton ).perform()
        time.sleep( timeout )

        # Check that the image text box value is now 1
        currImageValue = self._getImageValue( driver )
        print "Check increase image..."
        print "oldImageValue=", firstImageValue, "newImageValue=", currImageValue
        self.assertEqual( int(currImageValue), int(firstImageValue)+1, "Failed to increment the Image Animator")

    # Test that the Channel Animator decreases by one frame when the decrease frame button is pressed
    def test_animatorDecreaseFrame(self):
        driver = self.driver 
        timeout = selectBrowser._getSleep()

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "aJ.fits")
        Util.load_image( self, driver, "Default")

        # Go to the last channel value and record the frame value
        self._getLastValue( driver )
        lastChannelValue = self._getChannelValue( driver )

        # Find the decrement by one button on the Channel Animator Tape Deck and click it
        decrementButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@class='qx-toolbar']/div[@qxclass='qx.ui.toolbar.Button'][2]")))
        self.assertIsNotNone( decrementButton, "Could not find button to decrement the channels")
        driver.execute_script( "arguments[0].scrollIntoView(true);", decrementButton)
        ActionChains(driver).click( decrementButton).perform()
        time.sleep( timeout )

        # Check that the channel text box value is one less that the last frame value
        currChannelValue = self._getChannelValue( driver )
        print "Check decrease frame..."
        print "oldChannelValue=", lastChannelValue, "newChannelValue=",currChannelValue
        self.assertEqual( int(currChannelValue), int(lastChannelValue)-1, "Failed to decrement the Channel Animator")

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Record the first image value
        self._getLastValue( driver )
        lastImageValue = self._getImageValue( driver )

        # Find the decrement by one button on the Animator Tape Deck and click it
        decrementButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@class='qx-toolbar']/div[@qxclass='qx.ui.toolbar.Button'][2]")))
        self.assertIsNotNone( decrementButton, "Could not find button to decrement the image")
        driver.execute_script( "arguments[0].scrollIntoView(true);", decrementButton)
        ActionChains(driver).click( decrementButton).perform()
        time.sleep( timeout )

        # Check that the image text box value is now 1
        currImageValue = self._getImageValue( driver )
        print "Check decrease image..."
        print "oldImageValue=", lastImageValue, "newImageValue=", currImageValue
        self.assertEqual( int(currImageValue), int(lastImageValue)-1, "Failed to decrement the Image Animator")

    def tearDown(self):
        # Close the browser
        self.driver.close()
        # Allow browser to fully close before continuing
        time.sleep(2)
        # Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()  
        
