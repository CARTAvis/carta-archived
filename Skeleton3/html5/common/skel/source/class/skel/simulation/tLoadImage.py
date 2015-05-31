import Util
import unittest
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

#Test that a pre-existing, hard-coded image can be loaded in a display window.
class tLoadImage(unittest.TestCase):
    def setUp(self):
        self.driver = webdriver.Firefox()
           
    # Test that an image can be loaded and then closed.
    def test_load_image(self):    
        driver = self.driver
        driver.get("http://localhost:8080/pureweb/app?client=html5&name=CartaSkeleton3&username=dan12&password=Cameron21")
        driver.implicitly_wait(10)
        
        #Load a specific image.
        Util.load_image(self, driver, "Orion.methanol.cbc.contsub.image.fits")
        
        # Click on the Data->Close->Image button to close the image.
        imageWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        ActionChains(driver).context_click(imageWindow).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()
        
        
    def tearDown(self):
        self.driver.close()

if __name__ == "__main__":
    unittest.main()        
        
