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
        time.sleep( 4 )
        
        #Open the image settings
        #Open the regions tab
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
        
    #Test that if we load two images and then turn auto select off we can
    #select the other image and see a region information update in text fields
    def test_noAutoSelect(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "Default")
        time.sleep( timeout )
        
        # Load a rectangular region in the image.
        Util.load_image( self, driver, "OrionMethanolRegion.crtf")
        time.sleep( timeout )
        
        # Load an elliptical region in the image.
        Util.load_image( self, driver, "OrionMethanolRegionEllipse.crtf")
        time.sleep( 10 )
        
        # Open the regions tab in the image window
        Util.openSettings( self, driver, "Image", True )
        time.sleep(timeout)
        Util.clickTab( driver, "Regions" )
        
        # Turn off auto select
        autoSelectBox = WebDriverWait(driver, 10).until(EC.presence_of_element_located( ( By.XPATH, "//div[@id[starts-with(.,'RegionAutoSelect')]]/div[@qxclass='qx.ui.basic.Image']") ) )
        Util.setChecked(self, driver, autoSelectBox, False)
        time.sleep( timeout )
        
        # The second region loaded (the ellipse) should be selected.
        # Store the center of the ellipse.
        centerXText = driver.find_element_by_xpath("//div[@id[starts-with(.,'RegionCenterX')]]/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", centerXText )
        centerX = centerXText.get_attribute( "value" )
        centerYText = driver.find_element_by_xpath("//div[@id[starts-with(.,'RegionCenterY')]]/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", centerYText )
        centerY = centerYText.get_attribute( "value" )
        print "Ellipse center at (", centerX, " and ", centerY, ")"
        
        # Select the other region
        firstItem = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='regionsTable']/div/div/div/div/div/div/div[contains(text(),'rectangle')]")))
        ActionChains( driver).click( firstItem ).perform()
        time.sleep(timeout)
        
        # Verify that the region information is updated
        centerXText = driver.find_element_by_xpath("//div[@id[starts-with(.,'RegionCenterX')]]/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", centerXText )
        newCenterX = centerXText.get_attribute( "value" )
        centerYText = driver.find_element_by_xpath("//div[@id[starts-with(.,'RegionCenterY')]]/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", centerYText )
        newCenterY = centerYText.get_attribute( "value" )
        print "Center of rectangle (",newCenterX,",",newCenterY,")"
        self.assertTrue( float( centerX) != float( newCenterX), "Center X did not change!")
        self.assertTrue( float( centerY) != float( newCenterY), "Center Y did not change!")
    
    

    # Test that a region file in CASA format can be loaded and then closed.
    def test_load_regionCASA(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "Default")
        time.sleep( timeout )
        
        # Load a specific region in the image.
        Util.load_image( self, driver, "OrionMethanolRegion.crtf")
        time.sleep( 8 )
        
         #Open the image settings
        #Open the stack tab
        Util.openSettings( self, driver, "Image", True )
        time.sleep( timeout )
        Util.clickTab( driver, "Regions" )
        
        #Verify that there is one region in the region list.
        self._checkRegionCount( driver, 1 );

        # Click on the Data->Close->Region button to close the region.
        ActionChains(driver).double_click( imageWindow ).perform()
        dataButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Data']/..")))
        ActionChains(driver).click( dataButton ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
        time.sleep( timeout )
        
    # Test that if a negative number is entered for the width of a region, an error message is
    # reported.  If the value is corrected, the error message goes away.
    def test_negativeRegionWidth(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "Default")
        time.sleep( timeout )
        
        # Load a specific region in the image.
        Util.load_image( self, driver, "OrionMethanolRegion.crtf")
        time.sleep( 4 )
        
         #Open the image settings
        #Open the stack tab
        Util.openSettings( self, driver, "Image", True )
        time.sleep(4)
        Util.clickTab( driver, "Regions" )
        
         #Change the width to -40 and check for an error.
        widthText = driver.find_element_by_xpath("//div[@id='RectangleRegionWidth']/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", widthText )
        Util._changeElementText(self, driver, widthText, -10)
        widthError = driver.find_element_by_xpath("//div[@id='RectangleRegionWidth']/div[contains(text(),'Range error:')]")
        self.assertIsNotNone( widthError, "No error was posted")
        time.sleep( timeout )
        
        #Change the width to 40 and check that the error is not there
        Util._changeElementText(self, driver, widthText, 40 );
        try:
            widthError = driver.find_element_by_xpath("//div[@id='RectangleRegionWidth']/div[contains(text(),'Range error:')]")
            self.assertTrue( False, "Should not be able to find a range error.")
        except Exception:
            print "Good, the error disappeared!"
            
    # Test that the major radius of an elliptical region cannot be made smaller than the
    # minor radius of an ellipse        
    def test_ellipseMinorMajorSwitch(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "Default")
        time.sleep( timeout )
        
        # Load a specific region in the image.
        Util.load_image( self, driver, "OrionMethanolRegionEllipse.reg")
        time.sleep( 6 )
        
        #Open the image settings
        #Open the stack tab
        Util.openSettings( self, driver, "Image", True )
        time.sleep(4)
        Util.clickTab( driver, "Regions" )
        
        #Store the value of the minor radius.
        minorRadiusText = driver.find_element_by_xpath("//div[@id='EllipseRegionMinorRadius']/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", minorRadiusText )
        minorRadius = minorRadiusText.get_attribute( "value")
        
        #Try setting the minor radius to 20, which is larger than the major radius.
        #There should be an error message from the server.
        Util._changeElementText(self, driver, minorRadiusText, 20)
        time.sleep( timeout )
        
        #Check that an error is posted
        radiusError = driver.find_element_by_xpath("//div[@id='EllipseRegionMinorRadius']/div[contains(text(),'Required: MajorRadius >= MinorRadius')]")
        self.assertIsNotNone( radiusError, "No error was posted")
        time.sleep( timeout )
        
        #Set the minor radius back to 5.
        Util._changeElementText(self, driver, minorRadiusText, 5 )
        time.sleep( timeout )
        
        #Check there is no longer and error
        try:
            radiusError = driver.find_element_by_xpath("//div[@id='EllipseRegionMinorRadius']/div[contains(text(),'Required: MajorRadius >= MinorRadius')]")
            self.assertTrue( False, "Should not be able to find a radius error.")
        except Exception:
            print "Good, the error disappeared!"
        
        
    # Test that a region file in CASA format can be loaded and then closed.
    def test_load_regionDS9(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "Default")
        time.sleep( timeout )
        
        # Load a specific region in the image.
        Util.load_image( self, driver, "OrionMethanolRegion.reg")
        time.sleep( 4 )
        
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
    def test_load_regions(self):
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

