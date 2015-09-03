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

# Test the Animator tape deck functionality
class tAnimatorTapeDeck(tAnimator.tAnimator):

    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp(self, browser)
    
    # Test that the Animator can animate in the forward direction
    # Under default settings, it takes roughly 2 seconds for the channel to change by 1
    def test_channelAnimatorForwardAnimation(self):
        driver = self.driver 

        # Open a test images
        # Note:  The test will fail unless the last image loaded has more than one channel
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "aJ.fits")
        Util.load_image( self, driver, "Default")

        # Record the first channel value of the test image
        self._getFirstValue( driver, "Channel" )

        print "Testing Channel Animator Forward Animation..."
        print "First channel value: 0"
        
        # Click the forward animate button
        # Allow the image to animate for 2 seconds
        self._animateForward( driver, "Channel" )
        time.sleep(2)

        # Check that the channel value is greater than the first channel value 
        currChannelValue = self._getCurrentValue( driver, "Channel" )
        print "Current channel", currChannelValue
        self.assertGreater( int(currChannelValue), 0, "Channel value did not increase for forward animation.")
        self._stopAnimation( driver, "Channel")

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Go to the first image and record the first image value
        self._getFirstValue( driver, "Image" )

        print "Testing Image Animator Forward Animation..."
        print "First image value: 0"

        # Click the forward animate button
        # Allow animation for 2 seconds
        self._animateForward( driver, "Image" )
        time.sleep(2)

        # Check that the image value is greater than the first image value
        currImageValue = self._getCurrentValue( driver, "Image" )
        print "Current image", currImageValue
        self.assertGreater( int(currImageValue), 0, "Image value did not increase for forward animation")

    # Test that the Animator can animate in the reverse direction
    # Under default settings, it takes roughly 2 seconds for the channel to change by 1
    def test_channelAnimatorReverseAnimation(self):
        driver = self.driver 

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "aJ.fits")
        Util.load_image( self, driver, "Default")

        # Record the last channel value of the test image
        self._getLastValue( driver, "Channel" )
        lastChannelValue = self._getCurrentValue( driver, "Channel" )

        print "Testing Channel Animator Reverse Animation..."
        print "Last channel value:", lastChannelValue

        # Click the reverse animate button. Scroll into view if not visible
        # Allow image to animate for 2 seconds
        reverseAnimateButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ChannelTapeDeckReversePlay")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", reverseAnimateButton)
        ActionChains(driver).click( reverseAnimateButton ).perform()
        time.sleep(2)

        # Check that the channel value is at a value less than the last channel value 
        currChannelValue = self._getCurrentValue( driver, "Channel" )
        print "Current channel", currChannelValue
        self.assertLess( int(currChannelValue), int(lastChannelValue), "Channel value did not decrease for reverse animation.")
        self._stopAnimation( driver, "Channel")

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Go to the last image and record the last image value
        self._getLastValue( driver, "Image" )
        lastImageValue = self._getCurrentValue( driver, "Image" )

        print "Testing Image Animator Reverse Animation..."
        print "Last image value:", lastImageValue

        # Click the reverse animate button. Scroll into view if not visible
        # Allow image to animate for 2 seconds
        reverseAnimateButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ImageTapeDeckReversePlay")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", reverseAnimateButton)
        ActionChains(driver).click( reverseAnimateButton ).perform()
        time.sleep(2)

        # Check that the image value is at a value less than the last image value 
        currImageValue = self._getCurrentValue( driver, "Image" )
        print "Current image", currImageValue
        self.assertLess( int(currImageValue), int(lastImageValue), "Image value did not decrease for reverse animation")

    # Test that the Channel Animator can stop the animation
    def test_channelAnimatorStopAnimation(self):
        driver = self.driver 

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "aJ.fits")
        Util.load_image( self, driver, "Default")

        # Allow the image to animate for 2 seconds
        self._animateForward( driver, "Channel" )
        time.sleep(2)

        # Click on the Stop button. Scroll into view if not visible
        stopButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ChannelTapeDeckStopAnimation")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", stopButton)
        ActionChains(driver).click( stopButton ).perform()
        channelValue = self._getCurrentValue( driver, "Channel" )

        # Wait for another 2 seconds. Ensure the channel value did not change 
        time.sleep(2)
        currChannelValue = self._getCurrentValue( driver, "Channel" )
        self.assertEqual( int(currChannelValue), int(channelValue), "Channel animation did not stop" )

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Allow animation for 2 seconds
        self._animateForward( driver, "Image" )
        time.sleep(2)

        # Click on the Stop button. Scroll into view if not visible
        stopButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ImageTapeDeckStopAnimation")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", stopButton)
        ActionChains(driver).click( stopButton ).perform()
        imageValue = self._getCurrentValue( driver, "Image" )

        # Wait for another 2 seconds. Ensure that the image value did not change
        time.sleep(2)
        currImageValue = self._getCurrentValue( driver, "Image" )
        self.assertEqual( int(currImageValue), int(imageValue), "Image animation did not stop")

    # Test that the Channel Animator can go to the first frame value of the test image 
    def test_channelAnimatorFirstValue(self):
        driver = self.driver 

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "aJ.fits")
        Util.load_image( self, driver, "Default")

        # Record the first channel value of the test image
        self._getFirstValue( driver, "Channel" )
        firstChannelValue = self._getCurrentValue( driver, "Channel" )

        # Allow the image to animate for 2 seconds
        # Click the first valid value button 
        # Check that the channel value is the same as the first channel value
        self._animateForward( driver, "Channel" )
        time.sleep(2)
        self._getFirstValue( driver, "Channel" )
        currChannelValue = self._getCurrentValue( driver, "Channel" )
        self.assertEqual( int(currChannelValue), int(firstChannelValue), "Channel Animator did not return to first channel value")

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

         # Go to the first image and record the first image value
        self._getFirstValue( driver, "Image" )
        firstImageValue = self._getCurrentValue( driver, "Image" )

        # Allow animation for 2 seconds
        self._animateForward( driver, "Image" )
        time.sleep(2)

        # Click the first valid value button 
        # Check that the image value is the same as the first image value       
        self._getFirstValue( driver, "Image" )
        currImageValue = self._getCurrentValue( driver, "Image" )
        self.assertEqual( int(currImageValue), int(firstImageValue), "Image Animator did not return to first image")

    # Test that the Channel Animator can go to the last frame value of the test image 
    def test_channelAnimatorLastValue(self):
        driver = self.driver 

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "Default")

        # Record the last channel value of the test image
        self._getLastValue( driver, "Channel" )
        lastChannelValue = self._getCurrentValue( driver, "Channel" )

        # Return to the first frame and allow the image to animate for 2 seconds
        self._getFirstValue( driver, "Channel" )
        self._animateForward( driver, "Channel" )
        time.sleep(2)

        # Click the last valid value button 
        # Check that the channel value is the same as the last channel value
        self._getLastValue( driver, "Channel" )
        currChannelValue = self._getCurrentValue( driver, "Channel" )
        self.assertEqual( int(currChannelValue), int(lastChannelValue), "Channel Animator did not return to last channel value")

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Go to the first image and record the first image value
        self._getLastValue( driver, "Image" )
        lastImageValue = self._getCurrentValue( driver, "Image" )

        # Allow animation for 2 seconds
        self._animateForward( driver, "Image" )
        time.sleep(2)

        # Click the first valid value button 
        # Check that the image value is the same as the first image value       
        self._getLastValue( driver, "Image" )
        currImageValue = self._getCurrentValue( driver, "Image" )
        self.assertEqual( int(currImageValue), int(lastImageValue), "Image Animator did not return to last image")

    # Test that the Channel Animator lower spin box cannot exceed boundary values
    def test_animatorBoundary(self):
        driver = self.driver 

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "aJ.fits")
        Util.load_image( self, driver, "Default")

        # Find and record the last valid value of the animation
        self._getLastValue( driver, "Channel" )
        lastChannelValue = self._getCurrentValue( driver, "Channel" )

        # Find and click the lower spin box
        lowerBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ChannelLowerBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", lowerBoundText)
        lowerBoundText.click()

        # Check that the input to the lower spin box cannot be negative
        lowerBoundValue = Util._changeElementText( self, driver, lowerBoundText, -10 )
        self.assertGreaterEqual( int(lowerBoundValue), 0, "Channel lower bound value is negative")

        # Check that the input to the lower spin box cannot be greater than the last channel value
        lowerBoundValue = Util._changeElementText( self, driver, lowerBoundText, int(lastChannelValue )+10 )
        self.assertLessEqual( int(lowerBoundValue), int(lastChannelValue), "Channel lower bound value is greater than the last channel value")

        # Find and click the upper spin box
        upperBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ChannelUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", upperBoundText)
        upperBoundText.click()

        # Check that the input to the upper spin box cannot be greater than the last channel value
        upperBoundValue = Util._changeElementText( self, driver, upperBoundText, int(lastChannelValue)+10 )
        self.assertLessEqual( int(upperBoundValue), int(lastChannelValue), "Channel upper bound value is greater than the last valid channel value")

        # Check that the input to the upper spin box cannot be less than the first channel value
        upperBoundValue = Util._changeElementText( self, driver, upperBoundText, -10)
        self.assertGreaterEqual( int(upperBoundValue), 0, "Channel Upper bound value is negative")

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Record the last image value
        self._getLastValue( driver )
        lastImageValue = self._getImageValue( driver )

        # Find and click the lower spin box
        lowerBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ImageLowerBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", lowerBoundText)
        lowerBoundText.click()

        # Check that the input to the lower spin box cannot be negative
        lowerBoundValue = Util._changeElementText( self, driver, lowerBoundText, -10 )
        self.assertGreaterEqual( int(lowerBoundValue), 0, "Image lower bound value is negative")

        # Check that the input to the lower spin box cannot be greater than the last image value
        lowerBoundValue = Util._changeElementText( self, driver, lowerBoundText, int(lastImageValue )+10 )
        self.assertLessEqual( int(lowerBoundValue), int(lastImageValue), "Image lower bound value is greater than the last image value")

        # Find and click the upper spin box
        imageUpperBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ImageUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", imageUpperBoundText)
        imageUpperBoundText.click()

        # Check that the input to the upper spin box cannot be greater than the last image value
        imageUpperBoundValue = Util._changeElementText( self, driver, imageUpperBoundText, int(lastImageValue)+10 )
        self.assertLessEqual( int(imageUpperBoundValue), int(lastImageValue), "Image upper bound value is greater than the last image value")

        # Check that the input to the upper spin box cannot be less than the first image value
        imageUpperBoundValue = Util._changeElementText( self, driver, imageUpperBoundText, -10)
        self.assertGreaterEqual( int(imageUpperBoundValue), 0, "Image upper bound value is negative")

    # Test that the Channel Animator upper and lower bound values do not change during animation
    def test_channelAnimatorBoundaryAnimation(self):
        driver = self.driver 

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "Default")

        # Find and record the last valid value of the animation
        self._getLastValue( driver, "Channel" )
        lastChannelValue = self._getCurrentValue( driver, "Channel" )

        # Find and click the lower spin box
        lowerBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ChannelLowerBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", lowerBoundText)
        lowerBoundText.click()

        # Change the lower bound value
        lowerBoundValue = Util._changeElementText(self, driver, lowerBoundText, 1)

        # Find and click the upper spin box
        upperBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ChannelUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", upperBoundText)
        upperBoundText.click()

        # Change the upper bound value 
        upperBoundValue = Util._changeElementText( self, driver, upperBoundText, int(lastChannelValue)-1)

        # Allow test image to animate for 2 seconds
        self._animateForward( driver, "Channel" )
        time.sleep(2)

        # Check that the lower and upper bound values did not change during animation
        lowerBound = lowerBoundText.get_attribute("value")
        upperBound = upperBoundText.get_attribute("value")
        self.assertEqual( int(lowerBound), int(lowerBoundValue), "Lower bound channel value changed during animation")
        self.assertEqual( int(upperBound), int(upperBoundValue), "Upper bound channel value changed during animation")
    
    def test_imageAnimatorBoundaryAnimation(self):
        driver = self.driver 

        # Open a test image so we have something to animate
        Util.load_image( self, driver, "aH.fits")
        Util.load_image( self, driver, "aJ.fits")
        Util.load_image( self, driver, "Default")

        # Change the Channel Animator to an Image Animator
        self.channel_to_image_animator( driver )

        # Record the first image value
        self._getFirstValue( driver, "Image" )

        # Record the last image value
        self._getLastValue( driver, "Image" )
        lastImageValue = self._getCurrentValue( driver, "Image" )

        # Find and click the lower spin box
        lowerBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ImageLowerBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", lowerBoundText)
        lowerBoundText.click()

        # Change the lower bound value
        imageLowerBoundValue = Util._changeElementText(self, driver, lowerBoundText, 1)

        # Find and click the upper spin box
        imageUpperBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ImageUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", imageUpperBoundText)
        imageUpperBoundText.click()

        # Change the upper bound value
        imageUpperBoundValue = Util._changeElementText( self, driver, imageUpperBoundText, int(lastImageValue)-1 )

        # Allow animation for 2 seconds
        self._animateForward( driver, "Image" )
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
        time.sleep(2)
        # Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()    
        
