import Util
import time
import unittest
import tAnimator
import selectBrowser
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

# Test the Animator tape deck functionality
class tAnimatorTapeDeck(tAnimator.tAnimator):

    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp(self, browser)

    # Test that the Channel Animator can animate in the forward direction
    # Under default settings, it takes roughly 2 seconds for the channel to change by 1
    def test_channelAnimatorForwardAnimation(self):
        driver = self.driver 
        time.sleep(5)

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "Default")

        # Ensure default settings 
        self._setDefaultSettings( driver )

        # Record the first channel value of the test image
        self._getFirstValue( driver )
        firstChannelValue = self._getChannelValue( driver )

        print "Testing Channel Animator Forward Animation..."
        print "First channel value:", firstChannelValue
        
        # Click the forward animate button
        # Allow the image to animate for 2 seconds
        self._animateForward( driver )
        time.sleep(2)

        # Check that the channel value is greater than the first channel value 
        currChannelValue = self._getChannelValue( driver )
        print "Current channel", currChannelValue
        self.assertGreater( int(currChannelValue), int(firstChannelValue), "Channel value did not increase for forward animation.")

    # Test that the Channel Animator can animate in the reverse direction
    # Under default settings, it takes roughly 2 seconds for the channel to change by 1
    def test_channelAnimatorReverseAnimation(self):
        driver = self.driver 
        time.sleep(5)

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "Default")

        # Ensure default settings 
        self._setDefaultSettings( driver )

        # Record the last channel value of the test image
        self._getLastValue( driver )
        lastChannelValue = self._getChannelValue( driver )

        print "Testing Channel Animator Reverse Animation..."
        print "Last channel value:", lastChannelValue

        # Click the reverse animate button. Scroll into view if not visible
        # Allow image to animate for 2 seconds
        reverseAnimateButton = driver.find_element_by_xpath( "//div[@class='qx-toolbar']/div[@class='qx-button'][1]")
        self.assertIsNotNone( reverseAnimateButton, "Could not find reverse animation button")
        driver.execute_script( "arguments[0].scrollIntoView(true);", reverseAnimateButton)
        ActionChains(driver).click( reverseAnimateButton ).perform()
        time.sleep(2)

        # Check that the channel value is at a value less than the last channel value 
        currChannelValue = self._getChannelValue( driver )
        print "Current channel", currChannelValue
        self.assertEqual( int(currChannelValue), int(lastChannelValue)-1, "Channel value did not decrease for reverse animation.")

    # Test that the Channel Animator can stop the animation
    def test_channelAnimatorStopAnimation(self):
        driver = self.driver 
        time.sleep(5)

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "Default")

        # Allow the image to animate for 2 seconds
        self._animateForward( driver )
        time.sleep(2)

        # Click on the Stop button. Scroll into view if not visible
        stopButton = driver.find_element_by_xpath( "//div[@qxclass='qx.ui.toolbar.Button'][3]")
        self.assertIsNotNone( stopButton, "Could not find stop button")
        driver.execute_script( "arguments[0].scrollIntoView(true);", stopButton)
        ActionChains(driver).click( stopButton ).perform()
        channelValue = self._getChannelValue( driver )

        # Wait for another 2 seconds. Ensure the channel value did not change 
        time.sleep(2)
        currChannelValue = self._getChannelValue( driver )
        self.assertEqual( int(currChannelValue), int(channelValue), "Channel animation did not stop" )

    # Test that the Channel Animator can go to the first frame value of the test image 
    def test_channelAnimatorFirstValue(self):
        driver = self.driver 
        time.sleep(5)

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "Default")

        # Record the first channel value of the test image
        self._getFirstValue( driver )
        firstChannelValue = self._getChannelValue( driver )

        # Allow the image to animate for 2 seconds
        # Click the first valid value button 
        # Check that the channel value is the same as the first channel value
        self._animateForward( driver )
        time.sleep(2)
        self._getFirstValue( driver )
        currChannelValue = self._getChannelValue( driver )
        self.assertEqual( int(currChannelValue), int(firstChannelValue), "Channel Animator did not return to first channel value")

    # Test that the Channel Animator can go to the last frame value of the test image 
    def test_channelAnimatorLastValue(self):
        driver = self.driver 
        time.sleep(5)

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "Default")

        # Record the last channel value of the test image
        self._getLastValue( driver )
        lastChannelValue = self._getChannelValue( driver )

        # Return to the first frame and allow the image to animate for 2 seconds
        self._getFirstValue( driver )
        self._animateForward( driver )
        time.sleep(2)

        # Click the last valid value button 
        # Check that the channel value is the same as the last channel value
        self._getLastValue( driver )
        currChannelValue = self._getChannelValue( driver )
        self.assertEqual( int(currChannelValue), int(lastChannelValue), "Channel Animator did not return to last channel value")

    # Test that the Channel Animator lower spin box cannot exceed boundary values
    def test_channelAnimatorLowerBound(self):
        driver = self.driver 
        time.sleep(5)

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "Default")

        # Find and record the first valid value of the animation
        self._getFirstValue( driver )
        firstChannelValue = self._getChannelValue( driver )

        # Find and record the last valid value of the animation
        self._getLastValue( driver )
        lastChannelValue = self._getChannelValue( driver )

        # Find and click the lower spin box
        lowerBoundText = driver.find_element_by_xpath( "//div[@qxclass='skel.boundWidgets.Animator']/div[2]/div[@class='qx-input'][1]/input")
        self.assertIsNotNone( lowerBoundText, "Could not find lower bound spin box")
        driver.execute_script( "arguments[0].scrollIntoView(true);", lowerBoundText)
        lowerBoundText.click()

        # Check that the input to the lower spin box cannot be negative
        lowerBoundValue = Util._changeElementText( self, driver, lowerBoundText, -10 )
        self.assertGreaterEqual( int(lowerBoundValue), 0, "Lower bound value is negative")

        # Check that the input to the lower spin box cannot be greater than the last channel value
        lowerBoundValue = Util._changeElementText( self, driver, lowerBoundText, int(lastChannelValue )+10 )
        self.assertLessEqual( int(lowerBoundValue), int(lastChannelValue), "Lower bound value is greater than the last channel value")

        # Check that the input to the lower spin box cannot be less than the first channel value
        lowerBoundValue = Util._changeElementText( self, driver, lowerBoundText, int(firstChannelValue)-1 )
        self.assertGreaterEqual( int(lowerBoundValue), int(firstChannelValue), "Lower bound value is less than the first channel value") 

    # Test that the Channel Animator upper spin box cannot exceed boundary values
    def test_channelAnimatorUpperBound(self):
        driver = self.driver 
        time.sleep(5)

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "Default")

        # Find and record the first valid value of the animation
        self._getFirstValue( driver )
        firstChannelValue = self._getChannelValue( driver )

        # Find and record the last valid value of the animation
        self._getLastValue( driver )
        lastChannelValue = self._getChannelValue( driver )

        # Find and click the upper spin box
        upperBoundText = driver.find_element_by_xpath( "//div[@id='ChannelUpperBoundSpin']/input")
        self.assertIsNotNone( upperBoundText, "Could not find upper bound spin box")
        driver.execute_script( "arguments[0].scrollIntoView(true);", upperBoundText)
        upperBoundText.click()

        # Check that the input to the upper spin box cannot be greater than the last channel value
        upperBoundValue = Util._changeElementText( self, driver, upperBoundText, int(lastChannelValue)+10 )
        self.assertLessEqual( int(upperBoundValue), int(lastChannelValue), "Upper bound value is greater than the last valid channel value")

        # Check that the input to the upper spin box cannot be less than the first channel value
        upperBoundValue = Util._changeElementText( self, driver, upperBoundText, int(firstChannelValue )-10)
        self.assertGreaterEqual( int(upperBoundValue), int(firstChannelValue), "Upper bound value is less than the first channel value")

    # Test that the Channel Animator upper and lower bound values do not change during animation
    def test_channelAnimatorBoundaryAnimation(self):
        driver = self.driver 
        time.sleep(5)

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "Default")

        # Find and record the first valid value of the animation
        self._getFirstValue( driver )
        firstChannelValue = self._getChannelValue( driver )

        # Find and record the last valid value of the animation
        self._getLastValue( driver )
        lastChannelValue = self._getChannelValue( driver )

        # Find and click the lower spin box
        lowerBoundText = driver.find_element_by_xpath( "//div[@qxclass='skel.boundWidgets.Animator']/div[2]/div[@class='qx-input'][1]/input")
        self.assertIsNotNone( lowerBoundText, "Could not find lower bound spin box")
        driver.execute_script( "arguments[0].scrollIntoView(true);", lowerBoundText)
        lowerBoundText.click()

        # Change the lower bound value
        lowerBoundValue = Util._changeElementText(self, driver, lowerBoundText, int(firstChannelValue)+1)

        # Find and click the upper spin box
        upperBoundText = driver.find_element_by_xpath( "//div[@id='ChannelUpperBoundSpin']/input")
        self.assertIsNotNone( upperBoundText, "Could not find upper bound spin box")
        driver.execute_script( "arguments[0].scrollIntoView(true);", upperBoundText)
        upperBoundText.click()

        # Change the upper bound value 
        upperBoundValue = Util._changeElementText( self, driver, upperBoundText, int(lastChannelValue)-1)

        # Allow test image to animate for 2 seconds
        self._animateForward( driver )
        time.sleep(2)

        # Check that the lower and upper bound values did not change during animation
        lowerBound = lowerBoundText.get_attribute("value")
        upperBound = upperBoundText.get_attribute("value")
        self.assertEqual( int(lowerBound), int(lowerBoundValue), "Lower bound channel value changed during animation")
        self.assertEqual( int(upperBound), int(upperBoundValue), "Upper bound channel value changed during animation")

    # Test that the Image Animator can animate in the forward direction
    # Under default settings, it takes roughly 2 seconds for the image value to change by 1
    def test_imageAnimatorForwardAnimation(self):
        driver = self.driver
        time.sleep(5)

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Open test images so we have something to animate
        Util.load_image( self, driver, "Default")
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "aJ.fits")

        # Ensure default settings for the Image Animator
        self._setDefaultSettings( driver )

        # Go to the first image and record the first image value
        self._getFirstValue( driver )
        firstImageValue = self._getImageValue( driver )

        print "Testing Image Animator Forward Animation..."
        print "First image value:", firstImageValue

        # Click the forward animate button
        # Allow animation for 2 seconds
        self._animateForward( driver )
        time.sleep(2)

        # Check that the image value is greater than the first image value
        currImageValue = self._getImageValue( driver )
        print "Current image", currImageValue
        self.assertGreater( int(currImageValue), int(firstImageValue), "Image value did not increase for forward animation")

    # Test that the Image Animator can animate in the reverse direction
    # Under default settings, it takes roughly 2 seconds for the image value to change by 1
    def test_imageAnimatorReverseAnimation(self):
        driver = self.driver
        time.sleep(5)

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Open test images so we have something to animate
        Util.load_image( self, driver, "Default")
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "aJ.fits")

        # Ensure default settings for the Image Animator
        self._setDefaultSettings( driver )

        # Go to the last image and record the last image value
        self._getLastValue( driver )
        lastImageValue = self._getImageValue( driver )

        print "Testing Image Animator Reverse Animation..."
        print "Last image value:", lastImageValue

        # Click the reverse animate button. Scroll into view if not visible
        # Allow image to animate for 2 seconds
        reverseAnimateButton = driver.find_element_by_xpath( "//div[@class='qx-toolbar']/div[@class='qx-button'][1]")
        self.assertIsNotNone( reverseAnimateButton, "Could not find reverse animation button")
        driver.execute_script( "arguments[0].scrollIntoView(true);", reverseAnimateButton)
        ActionChains(driver).click( reverseAnimateButton ).perform()
        time.sleep(2)

        # Check that the image value is at a value less than the last image value 
        currImageValue = self._getImageValue( driver )
        print "Current image", currImageValue
        self.assertLess( int(currImageValue), int(lastImageValue), "Image value did not decrease for reverse animation")

    # Test that the Image Animation can stop animation
    def test_imageAnimatorStopAnimation(self):
        driver = self.driver
        time.sleep(5)

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Open test images so we have something to animate
        Util.load_image( self, driver, "Default")
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "aJ.fits")

        # Ensure default settings for the Image Animator
        self._setDefaultSettings( driver )

        # Allow animation for 2 seconds
        self._animateForward( driver )
        time.sleep(2)

        # Click on the Stop button. Scroll into view if not visible
        stopButton = driver.find_element_by_xpath( "//div[@qxclass='qx.ui.toolbar.Button'][3]")
        self.assertIsNotNone( stopButton, "Could not find stop button")
        driver.execute_script( "arguments[0].scrollIntoView(true);", stopButton)
        ActionChains(driver).click( stopButton ).perform()
        imageValue = self._getImageValue( driver )

        # Wait for another 2 seconds. Ensure that the image value did not change
        time.sleep(2)
        currImageValue = self._getImageValue( driver )
        self.assertEqual( int(currImageValue), int(imageValue), "Image animation did not stop")

    # Test that the Image Animator can go to the first image 
    def test_imageAnimatorFirstValue(self):
        driver = self.driver
        time.sleep(5)

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Open test images so we have something to animate
        Util.load_image( self, driver, "Default")
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "aJ.fits")

        # Ensure default settings for the Image Animator
        self._setDefaultSettings( driver )

        # Go to the first image and record the first image value
        self._getFirstValue( driver )
        firstImageValue = self._getImageValue( driver )

        # Allow animation for 2 seconds
        self._animateForward( driver )
        time.sleep(2)

        # Click the first valid value button 
        # Check that the image value is the same as the first image value       
        self._getFirstValue( driver )
        currImageValue = self._getImageValue( driver )
        self.assertEqual( int(currImageValue), int(firstImageValue), "Image Animator did not return to first image")

    # Test that the Image Animator can go to the last image 
    def test_imageAnimatorLastValue(self):
        driver = self.driver
        time.sleep(5)

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Open test images so we have something to animate
        Util.load_image( self, driver, "Default")
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "aJ.fits")

        # Ensure default settings for the Image Animator
        self._setDefaultSettings( driver )

        # Go to the first image and record the first image value
        self._getLastValue( driver )
        lastImageValue = self._getImageValue( driver )

        # Allow animation for 2 seconds
        self._animateForward( driver )
        time.sleep(2)

        # Click the first valid value button 
        # Check that the image value is the same as the first image value       
        self._getLastValue( driver )
        currImageValue = self._getImageValue( driver )
        self.assertEqual( int(currImageValue), int(lastImageValue), "Image Animator did not return to last image")

    # Test that the Image Animator lower spin box cannot exceed boundary values
    def test_imageAnimatorLowerBound(self):
        driver = self.driver
        time.sleep(5)

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Open test images so we have something to animate
        Util.load_image( self, driver, "Default")
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "aJ.fits")

        # Ensure default settings for the Image Animator
        self._setDefaultSettings( driver )

        # Record the first image value
        self._getFirstValue( driver )
        firstImageValue = self._getImageValue( driver )

        # Record the last image value
        self._getLastValue( driver )
        lastImageValue = self._getImageValue( driver )

        # Find and click the lower spin box
        lowerBoundText = driver.find_element_by_xpath( "//div[@qxclass='skel.boundWidgets.Animator']/div[2]/div[@class='qx-input'][1]/input")
        self.assertIsNotNone( lowerBoundText, "Could not find lower bound spin box")
        driver.execute_script( "arguments[0].scrollIntoView(true);", lowerBoundText)
        lowerBoundText.click()

        # Check that the input to the lower spin box cannot be negative
        lowerBoundValue = Util._changeElementText( self, driver, lowerBoundText, -10 )
        self.assertGreaterEqual( int(lowerBoundValue), 0, "Lower bound value is negative")

        # Check that the input to the lower spin box cannot be greater than the last image value
        lowerBoundValue = Util._changeElementText( self, driver, lowerBoundText, int(lastImageValue )+10 )
        self.assertLessEqual( int(lowerBoundValue), int(lastImageValue), "Lower bound value is greater than the last image value")

        # Check that the input to the lower spin box cannot be less than the first image value
        lowerBoundValue = Util._changeElementText( self, driver, lowerBoundText, int(firstImageValue)-1 )
        self.assertGreaterEqual( int(lowerBoundValue), int(firstImageValue), "Lower bound value is less than the first image value") 

    # Test that the Image Animator upper spin box cannot exceed boundary values
    def test_imageAnimatorUpperBound(self):
        driver = self.driver
        time.sleep(5)

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Open test images so we have something to animate
        Util.load_image( self, driver, "Default")
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "aJ.fits")

        # Ensure default settings for the Image Animator
        self._setDefaultSettings( driver )

        # Record the first image value
        self._getFirstValue( driver )
        firstImageValue = self._getImageValue( driver )

        # Record the last image value
        self._getLastValue( driver )
        lastImageValue = self._getImageValue( driver )

        # Find and click the upper spin box
        imageUpperBoundText = driver.find_element_by_xpath( "//div[@id='ImageUpperBoundSpin']/input")
        self.assertIsNotNone( imageUpperBoundText, "Could not find upper bound for image animator")
        driver.execute_script( "arguments[0].scrollIntoView(true);", imageUpperBoundText)
        imageUpperBoundText.click()

        # Check that the input to the upper spin box cannot be greater than the last image value
        imageUpperBoundValue = Util._changeElementText( self, driver, imageUpperBoundText, int(lastImageValue)+10 )
        self.assertLessEqual( int(imageUpperBoundValue), int(lastImageValue), "Upper bound value is greater than the last image value")

        # Check that the input to the upper spin box cannot be less than the first image value
        imageUpperBoundValue = Util._changeElementText( self, driver, imageUpperBoundText, int(firstImageValue )-10)
        self.assertGreaterEqual( int(imageUpperBoundValue), 0, "Upper bound value is less than the first image value")

    # Test that the Image Animator upper and lower bound values do not change during animation
    def test_imageAnimatorBoundaryAnimation(self):
        driver = self.driver
        time.sleep(5)

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Open test images so we have something to animate
        Util.load_image( self, driver, "Default")
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "aJ.fits")

        # Ensure default settings for the Image Animator
        self._setDefaultSettings( driver )

        # Record the first image value
        self._getFirstValue( driver )
        firstImageValue = self._getImageValue( driver )

        # Record the last image value
        self._getLastValue( driver )
        lastImageValue = self._getImageValue( driver )

        # Find and click the lower spin box
        lowerBoundText = driver.find_element_by_xpath( "//div[@qxclass='skel.boundWidgets.Animator']/div[2]/div[@class='qx-input'][1]/input")
        self.assertIsNotNone( lowerBoundText, "Could not find lower bound spin box")
        driver.execute_script( "arguments[0].scrollIntoView(true);", lowerBoundText)
        lowerBoundText.click()

        # Change the lower bound value
        imageLowerBoundValue = Util._changeElementText(self, driver, lowerBoundText, int(firstImageValue)+1)

        # Find and click the upper spin box
        imageUpperBoundText = driver.find_element_by_xpath( "//div[@id='ImageUpperBoundSpin']/input")
        self.assertIsNotNone( imageUpperBoundText, "Could not find upper bound for image animator")
        driver.execute_script( "arguments[0].scrollIntoView(true);", imageUpperBoundText)
        imageUpperBoundText.click()

        # Change the upper bound value
        imageUpperBoundValue = Util._changeElementText( self, driver, imageUpperBoundText, int(lastImageValue)-1 )

        # Allow animation for 2 seconds
        self._animateForward( driver )
        time.sleep(2)

        # Check that the lower and upper bound values did not change during animation
        lowerBound = lowerBoundText.get_attribute("value")
        upperBound = imageUpperBoundText.get_attribute("value")
        self.assertEqual( int(lowerBound), int(imageLowerBoundValue), "Lower bound image value changed during animation")
        self.assertEqual( int(upperBound), int(imageUpperBoundValue), "Upper bound image value changed during animation")

    def tearDown(self):
        # Close the browser
        self.driver.close()
        # Allow browser to fully close before continuing
        time.sleep(5)
        # Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()    
        
