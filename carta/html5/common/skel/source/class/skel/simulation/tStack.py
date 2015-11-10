import unittest
import Util
import time
import selectBrowser
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait

#Stack functionality.
class tStack(unittest.TestCase):
    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp(self, browser)
        
        

            
    # Load 3 images
    # Hide the second image; check the count goes down to 2
    # Show the second image; check the count goes up to 3
    def test_hideShow(self):    
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
        #Load images
        Util.load_image( self, driver, "Default")
        Util.load_image( self, driver, "aJ.fits")
        Util.load_image( self, driver, "aH.fits")
        
        #Verify the image animator sees three images.
        Util.verifyAnimatorUpperBound( self, driver, 2, "Image" )
        
        #Open the image settings
        #Open the stack tab
        Util.openSettings( self, driver, "Image" )
        Util.clickTab( driver, "Stack" )
        
        #Turn off auto select
        autoSelectCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "autoSelectImages")))
        ActionChains(driver).click( autoSelectCheck ).perform()
        
        #Hide the second image
        secondItem = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.ListItem']/div[text()='aJ.fits']/..")))
        ActionChains(driver).context_click( secondItem ).perform()
        ActionChains(driver).send_keys( Keys.ARROW_DOWN ).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ENTER ).perform()
        
        #Verify the animator sees two images
        Util.verifyAnimatorUpperBound(self, driver, 1, "Image" )
        
        #Show the second image
        secondItem = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.ListItem']/div[text()='aJ.fits']/..")))
        ActionChains(driver).context_click( secondItem ).perform()
        ActionChains(driver).send_keys( Keys.ARROW_DOWN ).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ENTER ).perform()
        
        #Verify the animator sees three images
        Util.verifyAnimatorUpperBound( self, driver, 2, "Image")
  

    def tearDown(self):
        #Close the browser
        self.driver.close()
        #Allow browser to fully close before continuing
        time.sleep(2)
        #Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()        