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

#Test color map functionality.
class tColorMap(unittest.TestCase):
    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp(self, browser)
        
    # Go to the next valid value
    def _nextImage(self, driver):
        timeout = selectBrowser._getSleep()
        incrementButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ImageTapeDeckIncrement")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", incrementButton)
        ActionChains(driver).click( incrementButton ).perform()
        time.sleep( timeout )
        
    def _getColorMapName( self, driver ):
        colorMapCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "colorMapName")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", colorMapCombo )
        colorMapText = colorMapCombo.find_element_by_xpath( ".//div/div")
        mapName = colorMapText.text
        return mapName
    
    def _chooseNewColorMap(self, driver ):
        timeout = selectBrowser._getSleep()
        colorMapCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "colorMapName")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", colorMapCombo )
        ActionChains(driver).click(colorMapCombo).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ARROW_DOWN
                ).send_keys( Keys.ENTER).perform()
        time.sleep( timeout )
        
        
    # Load 3 images and check that if we change the color map of one of them,
    # all three of them are changed.
    def test_globalIsGlobal(self):    
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
        #Load a default image
        Util.load_image( self, driver, "Default")
        Util.load_image( self, driver, "aJ.fits")
        Util.load_image( self, driver, "aH.fits")
        
        #Store the old colormap
        oldMapName = self._getColorMapName( driver )
        print "Old map name=",oldMapName
        
        #Choose a new color map
        self._chooseNewColorMap( driver )
        newMapName = self._getColorMapName( driver )
        print "New map name=", newMapName
        self.assertTrue( oldMapName != newMapName, "Color map name did not change")
        
        #Animate through the images and make sure they are all using the
        #new color map.
        for i in range(0,2):
            self._nextImage( driver )
            imageMapName = self._getColorMapName( driver )
            print "Image name=",imageMapName
            self.assertTrue( imageMapName == newMapName, "Image name was not changed")
        
            
    # Load 3 images
    # Select the bottom two in the stack
    # Turn of global in the color map
    # Change the color map
    # Check that the first two have the changed color map and the third does not
    def test_notGlobalIsNotGlobal(self):    
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
        #Load a default image
        image1 = "Default"
        image2 = "aJ.fits"
        image3 = "aH.fits"
        Util.load_image( self, driver, image1 )
        Util.load_image( self, driver, image2 )
        Util.load_image( self, driver, image3 )
        
        #Store the old colormap
        oldMapName = self._getColorMapName( driver )
        print "Old map name=",oldMapName
        
        #Open the stack tab
        Util.openSettings( self, driver, "Image" )
        Util.clickTab( driver, "Stack" )
        
        #Change to manual selection
        autoSelectCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "autoSelectImages")))
        ActionChains(driver).click( autoSelectCheck ).perform()
        
        #Select the first two images in the stack
        imageFirst = "//div[text()='{0}']/..".format( image2 )
        print "imageFirst=",imageFirst
        firstItem = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, imageFirst )))
        imageSecond = "//div[text()='{0}']/..".format( image3 )
        print "imageSecond=",imageSecond
        secondItem = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, imageSecond )))
        ActionChains( driver ).send_keys( Keys.SHIFT ).click( firstItem ).click( secondItem ).perform()
    
        #Uncheck using a global color map
        globalCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "colorMapGlobal")))
        ActionChains(driver).click( globalCheck ).perform()
        
        #Choose a new color map
        self._chooseNewColorMap( driver )
        newMapName = self._getColorMapName( driver )
        print "New map name=", newMapName
        self.assertTrue( oldMapName != newMapName, "Color map name did not change")
        
        #Change back to auto select so that the color map changes as we animate
        ActionChains(driver).click( autoSelectCheck ).perform()
        
        #Animate through images.  Make sure the first and second ones are using the new map 
        #and the third is not
        self._nextImage( driver )
        imageMapName = self._getColorMapName( driver )
        print "Image 1 name=",imageMapName
        self.assertTrue( imageMapName == oldMapName, "Color map name 1 incorrect")
        self._nextImage( driver )
        imageMapName = self._getColorMapName( driver )
        print "Image 2 name=",imageMapName
        self.assertTrue( imageMapName == newMapName, "Color map name 2 incorrect")
        self._nextImage( driver )
        imageMapName = self._getColorMapName( driver )
        print "Image 3 name=",imageMapName
        self.assertTrue( imageMapName == newMapName, "Color map name 3 incorrect")
            
        
    def tearDown(self):
        #Close the browser
        self.driver.close()
        #Allow browser to fully close before continuing
        time.sleep(2)
        #Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()        