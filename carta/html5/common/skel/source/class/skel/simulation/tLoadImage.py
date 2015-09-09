import Util
import time
import unittest
import selectBrowser
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.by import By

# Test that a pre-existing, hard-coded image can be loaded in a display window.
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
        
        # Click on the Data->Close->Image button to close the image.
        ActionChains(driver).double_click( imageWindow ).perform()
        dataButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Data']/..")))
        ActionChains(driver).click( dataButton ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()
        time.sleep( timeout )
        
    def tearDown(self):
        # Close the browser
        self.driver.close()
        # Allow browser to fully close before continuing
        time.sleep(2)
        # Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()        
        
