import unittest
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

#Test that a pre-existing, hard-coded image can be loaded in a display window.
class tLoadImage(unittest.TestCase):
    def setUp(self):
        self.driver = webdriver.Firefox()
           
    def test_load_image(self):    
        driver = self.driver
        driver.get("http://localhost:8080/pureweb/app?client=html5&name=CartaSkeleton3&username=dan12&password=Cameron21")
        driver.implicitly_wait(10)
        
        # Find a window capable of loading an image.
        imageWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
               
        # Select the window
        ActionChains(driver).click(imageWindow).perform()
        
        # Click the data button
        dataButton = driver.find_element_by_xpath("//div[text()='Data']/..")
        self.assertIsNotNone( dataButton, "Could not click data button in the context menu")
        ActionChains(driver).click(dataButton).perform()
        
        # Look for the open button and click it to open the file dialog.
        openDataButton = driver.find_element_by_xpath("//div/div[text()='Open...']/..")
        self.assertIsNotNone(openDataButton, "Could not click open button on data subcontext menu.")
        ActionChains(driver).click(openDataButton).perform()
        
        # Use Orion methanol as the test image.  Eventually this will have to evolve into
        # using an image available on a test server somewhere.
        fileDiv = driver.find_element_by_xpath( "//div[text()='Orion.methanol.cbc.contsub.image.fits']")
        fileDiv.click()
        
        #Click the load button
        loadButton = driver.find_element_by_id( "loadFileButton")
        self.assertIsNotNone(loadButton, "Could not find load button to click")
        loadButton.click()
        
        #Now close the file dialog
        closeButton = driver.find_element_by_id( "closeFileLoadButton")
        self.assertIsNotNone(loadButton, "Could not find button to close the file browser")
        closeButton.click()
        
        # Check that the window is displaying an image.
        viewElement = driver.find_element_by_xpath("//div[@qxclass='skel.boundWidgets.View.View']")
        self.assertIsNotNone(viewElement, "Could not find view element on page.")
        imageElement = driver.find_element_by_id("pwUID0")
        self.assertIsNotNone(imageElement, "Could not find image on the page")
        
        # Click on the Data->Close->Image button to close the image.
        ActionChains(driver).context_click(imageWindow).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()
        
        
    def tearDown(self):
        self.driver.close()

if __name__ == "__main__":
    unittest.main()        
        
