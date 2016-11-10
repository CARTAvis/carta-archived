import Util
import time
import unittest
import selectBrowser
from selenium import webdriver
from flaky import flaky
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.by import By

# Test loading images.
from flaky import flaky
class tLoadImage(unittest.TestCase):
    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp(self, browser)

    # Test that an image can be loaded and then closed.
    def test_load_image(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "Default")
        time.sleep( timeout )

        # Click on the Data->Close->Image button to close the image.
        ActionChains(driver).double_click( imageWindow ).perform()
        dataButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Data']/..")))
        ActionChains(driver).click( dataButton ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()
        time.sleep( timeout )
        
    # Test was written in response to Issue 178.  Loading a particular image produced
    # a crash.
    def test_load_image178(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "SI1.fits")
        time.sleep( timeout )

        # Make sure we have not crashed by closing the image
        ActionChains(driver).double_click( imageWindow ).perform()
        dataButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Data']/..")))
        ActionChains(driver).click( dataButton ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()
        time.sleep( timeout )

    # Test was written in response to Issue 152.  Loading an image and then pressing the
    # 100% clip button produced and error rather than changing the clip.
    def test_clip100(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "Default")
        time.sleep( timeout )
        
        # Store the minimum clip value
        minClipText = driver.find_element_by_xpath("//div[@id='clipMinIntensity']/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", minClipText )
        minClip = minClipText.get_attribute( "value")
        print "Min intensity", minClip
        
        # Press the 100% clip button
        ActionChains(driver).double_click( imageWindow ).perform()
        clippingButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Clipping']/..")))
        ActionChains(driver).click( clippingButton ).send_keys(Keys.ARROW_RIGHT
            ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN
            ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN
            ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN
            ).send_keys(Keys.ENTER).perform()                
        time.sleep( timeout )
        
        # Make sure the minimum clip value goes down.
        newMinClip = minClipText.get_attribute( "value")
        print "New min intensity", newMinClip
        self.assertTrue( float(newMinClip) < float(minClip), "Minimum clip value did not go down")
    
    
    # Test that we can load a large number of images, one after another
    def test_load_images(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "aH.fits")
        time.sleep( timeout )
        Util.load_image( self, driver, "aJ.fits")
        time.sleep( timeout )
        Util.load_image( self, driver, "N15693D.fits")
        time.sleep( timeout )
        Util.load_image( self, driver, "Orion.cont.image.fits")
        time.sleep( timeout )
        Util.load_image( self, driver, "Orion.methanol.cbc.contsub.image.fits")
        time.sleep( timeout )
        Util.load_image( self, driver, "TWHydra_CO2_1line.image.fits")
        time.sleep( timeout )
        Util.load_image( self, driver, "br1202_wide.image")
        time.sleep( timeout )
        Util.load_image( self, driver, "TWHydra_CO3_2line.image")
        time.sleep( timeout )
        Util.load_image( self, driver, "TWHydra_cont1.3mm.image")
        time.sleep( timeout )
        Util.load_image( self, driver, "v2.0_ds2_l000_13pca_map20.fits")
        time.sleep( timeout )

        #Find the image animator and verify that there are 9 images loaded
        upperBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ImageUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", upperBoundText)
        imageCount = upperBoundText.get_attribute("value")
        print "Image Count: ", imageCount
        self.assertEqual( imageCount, str(9), "Wrong number of images were loaded")


    def tearDown(self):
        # Close the browser
        self.driver.close()
        # Allow browser to fully close before continuing
        time.sleep(2)
        # Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()

