import unittest
import selectBrowser
import Util
import time
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.by import By

# Tests of Animator link functionality
class tAnimatorLinks(unittest.TestCase):

    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp( self, browser )

    # Test that the Animator will revert to default values after an image is removed
    def test_animatorRemoveImage(self):
        driver = self.driver 
        timeout = selectBrowser._getSleep()

        # Load an image
        Util.load_image( self, driver, "Default")

        # Click on the Data->Close->Image button to close the image.
        imageWindow = driver.find_element_by_xpath( "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        ActionChains(driver).double_click( imageWindow ).perform()
        dataButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Data']/..")))
        ActionChains(driver).click( dataButton ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()

        # Get the channel upper spin value 
        upperBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ChannelUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", upperBoundText)
        upperBound = upperBoundText.get_attribute("value")

        # Make sure the Animation window is enabled by clicking an element within the window
        channelText = driver.find_element_by_id( "ChannelIndexText")
        ActionChains(driver).click( channelText ).perform()

        # Show the Image Animator
        animateToolBar = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Animate']")))
        ActionChains(driver).click( animateToolBar ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ENTER).perform()
        time.sleep(timeout)

        # Find and click the upper spin box 
        imageUpperBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ImageUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", imageUpperBoundText)

        # Get the image upper spin value 
        imageCount = imageUpperBoundText.get_attribute("value")
        print "Image count=", imageCount

        # Check that the Animator is reset to default settings
        self.assertEqual( int(imageCount), 0, "Image Animator did not reset after the image was removed")
        self.assertEqual( int(upperBound), 0, "Channel Animator did not reset after the image was removed")


    # Test that the Animator will update after an image is loaded in the image window
    # Removal of the image should restore Animator to its default values
    def test_animatorAddImage(self):
        driver = self.driver

        # Load an image
        Util.load_image( self, driver, "Default")

        # Record the upper bound spin box value of the first image
        upperBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ChannelUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", upperBoundText)
        upperBound = upperBoundText.get_attribute("value")
        print "Upper bound", upperBound

        # In the same window, load a different image
        # The image should have a different number of channels
        Util.load_image( self, driver, "aH.fits")
        #Util.load_image( self, driver, "Orion.methanol.cbc.contsub.image.fits")

        # Check that the Animator upper bound spin box value is updated
        newUpperBound = upperBoundText.get_attribute("value")
        print "newUpperBound", newUpperBound
        self.assertNotEqual( int(newUpperBound), int(upperBound), "Animator did not update after an image was added")
  
    # Test that the Animator will update when linked to an image in a separate window
    def test_animatorChangeLink(self):
        driver = self.driver
        browser = selectBrowser._getBrowser()
        timeout = selectBrowser._getSleep()

        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Enable the animation window by clicking an element within the window
        channelText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ChannelIndexText")))
        ActionChains(driver).click( channelText ).perform()

        # Remove Animator link to the image window
        linkMenuButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Links...']")))
        ActionChains(driver).click( linkMenuButton ).perform()
        Util.remove_main_link( self, driver, imageWindow)

        # Load an image in a different window
        imageWindow2 = Util.load_image_different_window( self, driver, "N15693D.fits")

        # Change link to second image
        ActionChains(driver).click( channelText ).perform()
        linkMenuButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Links...']")))
        ActionChains(driver).click( linkMenuButton ).perform()
        Util.link_second_image( self, driver, imageWindow2)

        # Find and click the upper spin box
        upperBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ChannelUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", upperBoundText)

        # Check that the animator updates to the second image
        upperBound = upperBoundText.get_attribute("value")
        self.assertNotEqual( int(upperBound), 0, "Channel animator did not update to second image values")

        # Show the Image Animator
        animateToolBar = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Animate']")))
        ActionChains(driver).click( animateToolBar ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ENTER).perform()
        time.sleep(timeout)

        # Find and click the upper spin box 
        imageUpperBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ImageUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", imageUpperBoundText)

        # Get the image upper spin value 
        imageCount = imageUpperBoundText.get_attribute("value")

        # Check that the Image Animator updates
        self.assertEqual( int(imageCount), 0, "Image Animator did not update after image was linked")

    # Test that we can remove a linked image from the Animator
    def test_animatorRemoveLink(self):
        driver = self.driver
        browser = selectBrowser._getBrowser()
        timeout = selectBrowser._getSleep()

        # Locate the image window on the page; ensures browser is fully loaded
        imageWindow = WebDriverWait(driver, 30).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Make sure the animation window is enabled by clicking an element within the window
        channelText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ChannelIndexText")))
        ActionChains(driver).click( channelText ).perform()

        # Remove link from main image window to Animator 
        linkMenuButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Links...']")))
        ActionChains(driver).click( linkMenuButton ).perform()
        Util.remove_main_link( self, driver, imageWindow)

        # Load an image
        Util.load_image(self, driver, "Default")

        # Find and click the upper spin box
        upperBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ChannelUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", upperBoundText)

        # Check that the Channel Animator values remained at default values
        upperBound = upperBoundText.get_attribute("value")
        self.assertEqual( int(upperBound), 0, "Channel Animator is still linked to image after link was removed")

       # Show the Image Animator
        animateToolBar = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Animate']")))
        ActionChains(driver).click( animateToolBar ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ENTER).perform()
        time.sleep(timeout)

        # Find and click the upper spin box 
        imageUpperBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ImageUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", imageUpperBoundText)

        # Get the image upper spin value 
        imageCount = imageUpperBoundText.get_attribute("value")

        # Check that the Image Animator is at default values
        self.assertEqual( int(imageCount), 0, "Image Animator is still linked to image after the link was removed")

    # Test that we can add an Animator link to an image
    def test_animatorAddLink(self):
        driver = self.driver 
        timeout = selectBrowser._getSleep()
        browser = selectBrowser._getBrowser()

        # Load image in a separate window
        imageWindow2 = Util.load_image_different_window( self, driver, "N15693D.fits")

        # Make sure the animation window is enabled by clicking an element within the window
        channelText = driver.find_element_by_id("ChannelIndexText")
        ActionChains(driver).click( channelText ).perform()

        # Navigate to view links from the Animator settings        
        linkMenuButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Links...']")))
        ActionChains(driver).click( linkMenuButton ).perform()

        # Add link from the Animator to the image
        Util.link_second_image( self, driver, imageWindow2)

        # Find and click the upper spin box
        upperBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ChannelUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", upperBoundText)

        # Check that the animator updates 
        upperBound = upperBoundText.get_attribute("value")
        self.assertNotEqual( int(upperBound), 0, "Channel animator did not update to linked image")

        # Show the Image Animator
        animateToolBar = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Animate']")))
        ActionChains(driver).click( animateToolBar ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ENTER).perform()
        time.sleep(timeout)

        # Find and click the upper spin box 
        imageUpperBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ImageUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", imageUpperBoundText)

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