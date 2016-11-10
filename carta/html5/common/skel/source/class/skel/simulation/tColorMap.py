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
        ActionChains(driver).click( colorMapCombo ).perform()
        return mapName
    
    def _chooseNewColorMap(self, driver ):
        timeout = selectBrowser._getSleep()
        colorMapCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "colorMapName")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", colorMapCombo )
        ActionChains(driver).click(colorMapCombo).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ARROW_DOWN
                ).send_keys( Keys.ENTER).perform()
        time.sleep( timeout )
        
    
    # This was written in response to issue #180.  Change the units in the color bar with a 2D
    # image produced a crash.
    def test_unitChange(self):    
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
        #Load a 2D image
        Util.load_image( self, driver, "aH.fits")
        
        #Get the old units
        unitCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "colorImageUnits")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", unitCombo )
        unitText = unitCombo.find_element_by_xpath( ".//div/div")
        oldUnits = unitText.text
        print "Old units=", oldUnits
        
        #Change the units
        ActionChains(driver).click(unitCombo).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ARROW_DOWN
                ).send_keys( Keys.ENTER).perform()
        time.sleep( timeout )
        
        #Verify the units are changed.
        unitCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "colorImageUnits")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", unitCombo )
        unitText = unitCombo.find_element_by_xpath( ".//div/div")
        newUnits = unitText.text
        print "New units=",newUnits
        self.assertTrue( newUnits != oldUnits, "Color map units did not change")
        
        
    
        
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
    # Turn off 'apply all' in the color map
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
        time.sleep(4)
        
        #Open the stack tab
        Util.openSettings( self, driver, "Image", True )
        Util.clickTab( driver, "Stack" )
        
        #Change to manual selection
        autoSelectCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "autoSelectImages")))
        ActionChains(driver).click( autoSelectCheck ).perform()
        
        # Find the colormap window.
        colorWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowColormap']")))
        ActionChains(driver).click(colorWindow).perform()
        time.sleep( timeout )
        
        #Open the colormap settings
        Util.openSettings( self, driver, "Colormap", True )
        Util.clickTab( driver, "Color Map")
        time.sleep( timeout );
        
         #Store the old colormap
        oldMapName = self._getColorMapName( driver )
        print "Old map name=",oldMapName
        
        #Uncheck using a global color map
        globalCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "colorMapGlobal")))
        ActionChains(driver).move_to_element( globalCheck).click().perform()
        
        #Choose a new color map
        self._chooseNewColorMap( driver )
        newMapName = self._getColorMapName( driver )
        print "New map name=", newMapName
        self.assertTrue( oldMapName != newMapName, "Color map name did not change")
    
        #Change back to auto select so that the color map changes as we animate
        ActionChains(driver).click( autoSelectCheck ).perform()
        time.sleep( timeout )
        
        #Animate through images.  Make sure the first and second ones are using the old map 
        #and the third is not
        self._nextImage( driver )
        time.sleep( timeout )
        imageMapName = self._getColorMapName( driver )
        print "Image 1 name=",imageMapName
        self.assertTrue( imageMapName == oldMapName, "Color map name 1 incorrect")
        
        self._nextImage( driver )
        time.sleep( timeout )
        imageMapName = self._getColorMapName( driver )
        print "Image 2 name=",imageMapName
        self.assertTrue( imageMapName == oldMapName, "Color map name 2 incorrect")
        self._nextImage( driver )
        time.sleep( timeout )
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