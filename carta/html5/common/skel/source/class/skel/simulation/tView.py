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

#Test that we can view different plugins.
class tView(unittest.TestCase):
    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp(self, browser)
        
    # Test that we can change an animator to a CasaImageLoader
    def test_animator_to_casaimageloader(self):    
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 30).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Get the animation window count and make sure it is non-zero
        animWindowList = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")
        animWindowCount = len( animWindowList )
        self.assertGreater( animWindowCount, 0, "There are not any animators")
        
        # Get the image window count
        imageWindowList = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        imageWindowCount = len( imageWindowList )
        
        # Locate an animator window and bring up the right-context menu,
        # changing to a CasaImageLoader.
        Util.animation_to_image_window( self, driver )
        time.sleep( timeout )
        
        # Verify that the animation count has gone down by one and the image count
        # has increased by one.
        animWindowList = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")
        newAnimWindowCount = len( animWindowList )
        self.assertEqual( animWindowCount - 1, newAnimWindowCount, "Animation count did not decrease")
        imageWindowList = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        newImageWindowCount = len( imageWindowList )
        self.assertEqual( newImageWindowCount - 1, imageWindowCount, "Image window count did not increase")

    def tearDown(self):
        #Close the browser
        self.driver.close()
        #Allow browser to fully close before continuing
        time.sleep(2)
        #Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()        