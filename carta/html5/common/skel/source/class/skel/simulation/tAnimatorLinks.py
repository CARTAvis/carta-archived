import unittest
import selectBrowser
import Util
import time
import tAnimator
from selenium import webdriver
from flaky import flaky
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.by import By

# Tests of Animator link functionality
@flaky(max_runs=3)
class tAnimatorLinks(tAnimator.tAnimator):

    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp( self, browser )

    def animatorsAtDefault(self):
        # Make sure that there are no animators visible
        try :
            upperBoundTextChannel = driver.find_element_by_xpath( "//div[@id='ChannelUpperBoundSpin']/input")
            self.assertTrue( false, "Channel animator is still present")
        except Exception:
            print "Good - channel animator disappeared"
        try :
            upperBoundTextAnimator = driver.find_element_by_xpath( "//div[@id='ImageUpperBoundSpin']/input")
            self.assertTrue( false, "Image animator is still present")
        except Exception:
            print "Good - image animator disappeared"

    # Test that the Animator will revert to default values after an image is removed
    def test_animatorRemoveImage(self):
        driver = self.driver
        timeout = 2*selectBrowser._getSleep()

        # Load two image
        Util.load_image( self, driver, "Default")
        Util.load_image( self, driver, "aH.fits")

        # Click on the Data->Close->Image button to close the image.
        imageWindow = driver.find_element_by_xpath( "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        ActionChains(driver).click( imageWindow ).perform()
        dataButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Data']/..")))
        ActionChains(driver).click( dataButton ).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()
        time.sleep(timeout)
        dataButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Data']/..")))
        ActionChains(driver).click( dataButton ).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()


        self.animatorsAtDefault()

    # Test that the Animator will update after an image is loaded in the image window
    # Removal of the image should restore Animator to its default values
    def test_animatorAddImage(self):
        driver = self.driver

        # Load an image
        Util.load_image( self, driver, "TWHydra_CO2_1line.image.fits")

        # Record the upper bound spin box value of the first image
        upperBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ChannelUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", upperBoundText)
        upperBound = upperBoundText.get_attribute("value")
        print "Upper bound", upperBound

        # In the same window, load a different image
        # The image should have more channels than the first image in order to see an update
        Util.load_image( self, driver, "Default")

        # Check that the Animator upper bound spin box value is updated
        newUpperBound = upperBoundText.get_attribute("value")
        print "newUpperBound=", newUpperBound," old upperBound=",upperBound
        self.assertNotEqual( int(newUpperBound), int(upperBound), "Animator did not update after an image was added")

    # Test that the Animator will update when linked to an image in a separate window
    def test_animatorChangeLink(self):
        driver = self.driver
        browser = selectBrowser._getBrowser()
        timeout = 2*selectBrowser._getSleep()

        #Load an image with more than one channel so the channel
        #animator will appear
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
        Util.load_image( self, driver, "Default")
        time.sleep(2)

        # Enable the animation window by a context click
        # Note, clicking on the animation window is not working because the center of the
        # animation window is the stop button, and clicking that does not select the window.
        animWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")))
        ActionChains(driver).context_click( animWindow ).perform()

        # Remove Animator link to the image window
        linkMenuButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Links...']")))
        ActionChains(driver).click( linkMenuButton ).perform()
        Util.remove_main_link( self, driver, imageWindow)
    
        # Load an image in a different window
        imageWindow2 = Util.load_image_different_window( self, driver, "Orion.methanol.cbc.contsub.image.fits")

        # Change link to second image
        animWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ChannelAnimatorUpperBound")))
        ActionChains(driver).double_click( animWindow ).perform()
        
        linkMenuButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Links...']")))
        ActionChains(driver).click( linkMenuButton ).perform()
        
        Util.link_second_image( self, driver, imageWindow2)

        # Find and click the upper spin box
        upperBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ChannelUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", upperBoundText)

        # Check that the animator updates to the second image
        upperBound = upperBoundText.get_attribute("value")
        self.assertNotEqual( int(upperBound), 0, "Channel animator did not update to second image values")

        # Show the image animator by loading a second image in the second window
        Util.load_image_windowIndex( self, driver, "aH.fits", 2)
        time.sleep(2)

        # Find and click the upper spin box
        imageUpperBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ImageUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", imageUpperBoundText)

        # Get the image upper spin value
        imageCount = imageUpperBoundText.get_attribute("value")
        print "Animator image count ", imageCount

        # Check that the Image Animator updates
        self.assertEqual( int(imageCount), 1, "Image Animator did not update after image was linked")

    # Test that we can remove a linked image from the Animator
    def test_animatorRemoveLink(self):
        driver = self.driver
        browser = selectBrowser._getBrowser()
        timeout = 2*selectBrowser._getSleep()

        # Locate the image window on the page; ensures browser is fully loaded
        imageWindow = WebDriverWait(driver, 30).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Load an image with multiple channels so that the channel animator will show.
        Util.load_image( self, driver, "Default")

        # Make sure the animation window is enabled by clicking an element within the window
        channelText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ChannelIndexText")))
        ActionChains(driver).click( channelText ).perform()

        # Remove link from main image window to Animator
        linkMenuButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Links...']")))
        ActionChains(driver).click( linkMenuButton ).perform()
        Util.remove_main_link( self, driver, imageWindow)

        # Load another image
        Util.load_image(self, driver, "aH.fits")

        self.animatorsAtDefault()


    # Test that we can add an Animator link to an image
    def test_animatorAddLink(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()
        browser = selectBrowser._getBrowser()
        time.sleep(2)

        # Load image in a separate window, but make sure it has at least
        # one channel.
        imageWindow2 = Util.load_image_different_window( self, driver, "TWHydra_CO2_1line.image.fits")
        time.sleep(2)
        
        # Make sure the animation window is enabled by clicking an element within the window
        animWindow = driver.find_element_by_xpath( "//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']" )
        ActionChains(driver).click( animWindow ).perform()

        # Navigate to view links from the Animator settings
        linkMenuButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Links...']")))
        ActionChains(driver).click( linkMenuButton ).perform()

        # Add link from the Animator to the image
        Util.link_second_image( self, driver, imageWindow2)
        ActionChains(driver).send_keys( Keys.ESCAPE).perform()
        # Find and click the upper spin box
        upperBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ChannelUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", upperBoundText)

        # Check that the animator updates
        upperBound = upperBoundText.get_attribute("value")
        print "Upper bound ", upperBound
        self.assertNotEqual( int(upperBound), 0, "Channel animator did not update to linked image")

        # Show the Image Animator by loading a second image.
        Util.load_image_windowIndex( self, driver, "aH.fits", 2)

        # Find and click the upper spin box
        imageUpperBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ImageUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", imageUpperBoundText)

        # Get the image upper spin value
        imageCount = imageUpperBoundText.get_attribute("value")

        # Check that the Image Animator updates
        self.assertEqual( int(imageCount), 1, "Image Animator did not update after image was linked")

    def tearDown(self):
        # Close the browser
        self.driver.close()
        # Allow browser to fully close before continuing
        timeout = 2*selectBrowser._getSleep()
        time.sleep(timeout)
        # Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()
