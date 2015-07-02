import Util
import time
import unittest
import selectBrowser
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

# Tests of Animator link functionality
class tAnimatorLinks(unittest.TestCase):

    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp( self, browser )

    # Test that the Animator will revert to default values after an image is removed
    def test_animatorRemoveImage(self):
        driver = self.driver 
        time.sleep(5)

        # Load an image
        Util.load_image( self, driver, "Default")

        # Click on the Data->Close->Image button to  close the image.
        imageWindow = driver.find_element_by_xpath( "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        ActionChains(driver).context_click( imageWindow ).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()
        time.sleep(2)

        # Find and click the upper spin box
        upperBoundText = driver.find_element_by_xpath( "//div[@id='ChannelUpperBoundSpin']/input")
        self.assertIsNotNone( upperBoundText, "Could not find upper bound spin box")
        driver.execute_script( "arguments[0].scrollIntoView(true);", upperBoundText)
        upperBoundText.click()

        # Get the channel upper spin value 
        upperBound = upperBoundText.get_attribute("value")

        # Click the Animation window so that its actions will be enabled
        animWindow = driver.find_element_by_xpath( "//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")
        self.assertIsNotNone( animWindow, "Could not find Animation window")
        # Make sure the Animation window is enabled by clicking an element within the window
        channelText = driver.find_element_by_id( "ChannelIndexText")
        ActionChains(driver).click( channelText ).perform()

        # Show the Image Animator
        ActionChains(driver).context_click( channelText ).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        # Find and click the upper spin box 
        imageUpperBoundText = driver.find_element_by_xpath( "//div[@id='ImageUpperBoundSpin']/input")
        self.assertIsNotNone( imageUpperBoundText, "Could not find upper bound for image animator")
        driver.execute_script( "arguments[0].scrollIntoView(true);", imageUpperBoundText)
        imageUpperBoundText.click()

        # Get the image upper spin value 
        imageCount = imageUpperBoundText.get_attribute("value")

        # Check that the Animator is reset to default settings
        self.assertEqual( int(imageCount), -1, "Image Animator did not reset after the image was removed")
        self.assertEqual( int(upperBound), 0, "Channel Animator did not reset after the image was removed")
        
    # Test that the Animator will update after an an image is loaded in the image window
    # Removal of the image should restore Animator to its default values
    def test_animatorAddImage(self):
        driver = self.driver
        time.sleep(5)

        # Load an image
        Util.load_image( self, driver, "Default")

        # Find and click the upper spin box
        upperBoundText = driver.find_element_by_xpath( "//div[@id='ChannelUpperBoundSpin']/input")
        self.assertIsNotNone( upperBoundText, "Could not find upper bound spin box" )
        driver.execute_script( "arguments[0].scrollIntoView(true);", upperBoundText)
        upperBoundText.click()

        # Record the upper bound spin box value of the first image
        upperBound = upperBoundText.get_attribute("value")

        # In the same window, load a different image
        # The image should have a different number of channels
        Util.load_image( self, driver, "aH.fits")
        time.sleep(2)

        # Check that the Animator upper bound spin box value is updated
        upperBoundText.click()
        newUpperBound = upperBoundText.get_attribute("value")
        self.assertNotEqual( int(newUpperBound), int(upperBound), "Animator did not update after an image was added")

        # Remove the image
        imageWindow = driver.find_element_by_xpath( "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        ActionChains(driver).context_click(imageWindow).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
        time.sleep(2)

        # Check that Animator is reset to default values after the image is removed
        upperBoundText.click()
        newUpperBound = upperBoundText.get_attribute("value")
        self.assertNotEqual( int(newUpperBound), int(upperBound), "Animator did not update after image was removed")

    # Test that the Animator will update when linked to an image in a separate window
    def test_animatorChangeLink(self):
        driver = self.driver
        time.sleep(5)

        # Load image in a separate window
        imageWindow2 = Util.load_image_different_window( self, driver, "N15693D.fits")

        # Click on the Animator to enable it
        animWindow = driver.find_element_by_xpath( "//div[@qxclass='skel.boundWidgets.Animator']")
        self.assertIsNotNone( animWindow, "Could not find Animator window")
        ActionChains(driver).click( animWindow ).perform()
        # Make sure the animation window is enabled by clicking an element within the window
        channelText = driver.find_element_by_id("ChannelIndexText")
        ActionChains(driver).click( channelText ).perform()
        
        # Remove Animator link to the main image window
        ActionChains(driver).context_click( channelText ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        # Change the link location of the animator to the second image
        ActionChains(driver).move_to_element( animWindow ).click( animWindow ).drag_and_drop(animWindow, imageWindow2).perform()
        time.sleep(2)

        # Exit Links
        ActionChains(driver).move_to_element( animWindow ).context_click( animWindow ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        # Find and click the upper spin box
        upperBoundText = driver.find_element_by_xpath( "//div[@id='ChannelUpperBoundSpin']/input")
        self.assertIsNotNone( upperBoundText, "Could not find upper bound spin box" )
        driver.execute_script( "arguments[0].scrollIntoView(true);", upperBoundText)

        # Check that the animator updates to the second image
        upperBound = upperBoundText.get_attribute("value")
        self.assertNotEqual( int(upperBound), 0, "Channel animator did not update to second image values")

        # Show the Image Animator
        ActionChains(driver).context_click( channelText ).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
        time.sleep(2)

        # Find and click the upper spin box 
        imageUpperBoundText = driver.find_element_by_xpath( "//div[@id='ImageUpperBoundSpin']/input")
        self.assertIsNotNone( imageUpperBoundText, "Could not find upper bound for image animator")
        driver.execute_script( "arguments[0].scrollIntoView(true);", imageUpperBoundText)
        imageUpperBoundText.click()

        # Get the image upper spin value 
        imageCount = imageUpperBoundText.get_attribute("value")

        # Check that the Image Animator updates
        self.assertEqual( int(imageCount), 0, "Image Animator did not update after image was linked")
    
    # Test that we can remove a linked image from the Animator
    def test_animatorRemoveLink(self):
        driver = self.driver
        time.sleep(5)

        # Click on the Animator to enable it
        animWindow = driver.find_element_by_xpath( "//div[@qxclass='skel.boundWidgets.Animator']")
        self.assertIsNotNone( animWindow, "Could not find Animator window")
        ActionChains(driver).click( animWindow ).perform()
        # Make sure the animation window is enabled by clicking an element within the window
        channelText = driver.find_element_by_id("ChannelIndexText")
        ActionChains(driver).click( channelText ).perform()

        # Navigate to view links from the Animator settings        
        ActionChains(driver).context_click( channelText ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        # Remove Animator link to the main image window
        ActionChains(driver).move_to_element( animWindow ).context_click( animWindow ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        # Exit Links
        ActionChains(driver).move_to_element( animWindow ).context_click( animWindow ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        # Load an image
        Util.load_image(self, driver, "Default")

        # Find and click the upper spin box
        upperBoundText = driver.find_element_by_xpath( "//div[@id='ChannelUpperBoundSpin']/input")
        self.assertIsNotNone( upperBoundText, "Could not find upper bound spin box" )
        driver.execute_script( "arguments[0].scrollIntoView(true);", upperBoundText)

        # Check that the Channel Animator values remained at default values
        upperBound = upperBoundText.get_attribute("value")
        self.assertEqual( int(upperBound), -1, "Channel Animator is still linked to image after link was removed")

       # Show the Image Animator
        ActionChains(driver).context_click( channelText ).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
        time.sleep(2)

        # Find and click the upper spin box 
        imageUpperBoundText = driver.find_element_by_xpath( "//div[@id='ImageUpperBoundSpin']/input")
        self.assertIsNotNone( imageUpperBoundText, "Could not find upper bound for image animator")
        driver.execute_script( "arguments[0].scrollIntoView(true);", imageUpperBoundText)
        imageUpperBoundText.click()

        # Get the image upper spin value 
        imageCount = imageUpperBoundText.get_attribute("value")

        # Check that the Image Animator is at default values
        self.assertEqual( int(imageCount), -1, "Image Animator is still linked to image after the link was removed")

    # Test that we can add an Animator link to an image
    def test_animatorAddLink(self):
        driver = self.driver 
        time.sleep(5)

        # Load image in a separate window
        imageWindow2 = Util.load_image_different_window( self, driver, "N15693D.fits")

        # Click on the Animator to enable it
        animWindow = driver.find_element_by_xpath( "//div[@qxclass='skel.boundWidgets.Animator']")
        self.assertIsNotNone( animWindow, "Could not find Animator window")
        ActionChains(driver).click( animWindow ).perform()
        # Make sure the animation window is enabled by clicking an element within the window
        channelText = driver.find_element_by_id("ChannelIndexText")
        ActionChains(driver).click( channelText ).perform()

        # Navigate to view links from the Animator settings        
        ActionChains(driver).context_click( channelText ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        # Add link from the Animator to the image
        ActionChains(driver).move_to_element( animWindow ).click( animWindow ).drag_and_drop( animWindow, imageWindow2).perform()
        time.sleep(2)

        # Exit Links
        ActionChains(driver).move_to_element( animWindow ).context_click( animWindow ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        # Find and click the upper spin box
        upperBoundText = driver.find_element_by_xpath( "//div[@id='ChannelUpperBoundSpin']/input")
        self.assertIsNotNone( upperBoundText, "Could not find upper bound spin box" )
        driver.execute_script( "arguments[0].scrollIntoView(true);", upperBoundText)

        # Check that the animator updates 
        upperBound = upperBoundText.get_attribute("value")
        self.assertNotEqual( int(upperBound), 0, "Channel animator did not update to linked image")

        # Show the Image Animator
        ActionChains(driver).context_click( channelText ).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
        time.sleep(2)

        # Find and click the upper spin box 
        imageUpperBoundText = driver.find_element_by_xpath( "//div[@id='ImageUpperBoundSpin']/input")
        self.assertIsNotNone( imageUpperBoundText, "Could not find upper bound for image animator")
        driver.execute_script( "arguments[0].scrollIntoView(true);", imageUpperBoundText)
        imageUpperBoundText.click()

        # Get the image upper spin value 
        imageCount = imageUpperBoundText.get_attribute("value")

        # Check that the Image Animator updates
        self.assertEqual( int(imageCount), 0, "Image Animator did not update after image was linked")

    def tearDown(self):
        # Close the browser
        self.driver.close()
        # Allow browser to fully close before continuing
        time.sleep(2)
        # Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()     