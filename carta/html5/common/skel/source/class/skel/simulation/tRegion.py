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
class tRegion(unittest.TestCase):
    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp(self, browser)
        
    def _checkRegionCount(self, driver, expected):
        tableElement = driver.find_element_by_id('RegionsTable')
        cells = tableElement.find_elements_by_xpath(".//div[@class='qooxdoo-table-cell']")
        selectCount = len( cells )
        print "Region count=", selectCount
        self.assertEqual( float(selectCount), expected, "Incorrect number of regions loaded.")
    
    # Test that if we load an image and region, then delete the underlying region there will be
    # no problems
    def test_deleteImageFromRegion(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "Default")
        time.sleep( timeout )
        
        # Load a specific region in the image.
        Util.load_image( self, driver, "OrionMethanolRegion.crtf")
        time.sleep( timeout )
        
        #Open the image settings
        #Open the stack tab
        Util.openSettings( self, driver, "Image", True )
        time.sleep(4)
        Util.clickTab( driver, "Regions" )
        
        #Verify that there is one region in the region list.
        self._checkRegionCount( driver, 1 );

        # Click on the Data->Close->Image button to close the image.
        ActionChains(driver).double_click( imageWindow ).perform()
        dataButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Data']/..")))
        ActionChains(driver).click( dataButton ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()
        time.sleep( timeout )
           

    # Test that a region file in CASA format can be loaded and then closed.
    def test_load_regionCASA(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "Default")
        time.sleep( timeout )
        
        # Load a specific region in the image.
        Util.load_image( self, driver, "OrionMethanolRegion.crtf")
        time.sleep( timeout )
        
         #Open the image settings
        #Open the stack tab
        Util.openSettings( self, driver, "Image", True )
        time.sleep(4)
        Util.clickTab( driver, "Regions" )
        
        #Verify that there is one region in the region list.
        self._checkRegionCount( driver, 1 );

        # Click on the Data->Close->Region button to close the region.
        ActionChains(driver).double_click( imageWindow ).perform()
        dataButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Data']/..")))
        ActionChains(driver).click( dataButton ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
        time.sleep( timeout )
        
    # Test that a region file in CASA format can be loaded and then closed.
    def test_load_regionDS9(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "Default")
        time.sleep( timeout )
        
        # Load a specific region in the image.
        Util.load_image( self, driver, "OrionMethanolRegion.reg")
        time.sleep( timeout )
        
         #Open the image settings
        #Open the stack tab
        Util.openSettings( self, driver, "Image", True )
        time.sleep(4)
        Util.clickTab( driver, "Regions" )
        
        #Verify that there is one region in the region list.
        self._checkRegionCount( driver, 1 );

        # Click on the Data->Close->Region button to close the region.
        ActionChains(driver).double_click( imageWindow ).perform()
        dataButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Data']/..")))
        ActionChains(driver).click( dataButton ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
        time.sleep( timeout )    
        

    # Test that we can load several regions
    def test_load_images(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "Default")
        time.sleep( timeout )
        Util.load_image( self, driver, "OrionMethanolRegion.crtf")
        time.sleep( timeout )
        Util.load_image( self, driver, "OrionMethanolRegionEllipse.crtf")
        time.sleep( timeout )
        Util.load_image( self, driver, "OrionMethanolRegionPolygon.crtf")
        time.sleep( timeout )
       

        #Find the image animator and verify that there are 3 regions loaded
        upperBoundText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='RegionUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", upperBoundText)
        regionCount = upperBoundText.get_attribute("value")
        print "Region Count: ", regionCount
        self.assertEqual( regionCount, str(2), "Wrong number of regions were loaded")


    def tearDown(self):
        # Close the browser
        self.driver.close()
        # Allow browser to fully close before continuing
        time.sleep(2)
        # Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()

