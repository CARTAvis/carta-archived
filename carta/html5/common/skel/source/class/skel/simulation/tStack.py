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
        
    def verifyCompositionMode(self, driver, mode):
        print "verifying mode=", mode
        combineCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "layerCompositionMode")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", combineCombo )
        combineText = combineCombo.find_element_by_xpath( ".//div/div")
        combMode = combineText.text
        print "Combine mode=",combMode
        self.assertTrue( mode==combMode, "Combine modes not as expected")
        
    def _isColorChecked(self, colorBox ):
        colorBorder = colorBox.get_attribute( "class")
        checked = False
        if colorBorder == "qx-line-border":
            checked = True
        return checked
    
    def _testColor(self, colorBoxId, colorExpected, colorStr, driver ):
        filterBox = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, colorBoxId)))
        driver.execute_script( "arguments[0].scrollIntoView(true);", filterBox )
        colorChecked = self._isColorChecked( filterBox )
        print "Color checked=", colorChecked
        colorCheckExpected = True
        if colorExpected == 0:
            colorCheckExpected = False
        self.assertEqual( colorChecked, colorCheckExpected, colorStr + " amount is not correct")
        
    def verifyColor(self, driver, redExpected, greenExpected, blueExpected ):
        self._testColor( "filterRedBox", redExpected, "Red", driver )
        self._testColor( "filterGreenBox", greenExpected, "Green", driver)
        self._testColor( "filterBlueBox", blueExpected, "Blue", driver )
            
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
        Util.openSettings( self, driver, "Image", True )
        Util.clickTab( driver, "Stack" )
        
        #Turn off auto select
        autoSelectCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "autoSelectImages")))
        ActionChains(driver).click( autoSelectCheck ).perform()
        
        #Hide the second image
        secondItem = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Image.Stack.ListItemIcon']/div[text()='aJ.fits']/..")))
        ActionChains(driver).context_click( secondItem ).perform()
        ActionChains(driver).send_keys( Keys.ARROW_DOWN ).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ENTER ).perform()
        
        #Verify the animator sees two images
        time.sleep( 2 )
        Util.verifyAnimatorUpperBound(self, driver, 1, "Image" )
        
        #Show the second image
        secondItem = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Image.Stack.ListItemIcon']/div[text()='aJ.fits']/..")))
        ActionChains(driver).context_click( secondItem ).perform()
        ActionChains(driver).send_keys( Keys.ARROW_DOWN ).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ENTER ).perform()
        
        #Verify the animator sees three images
        time.sleep( 2 )
        Util.verifyAnimatorUpperBound( self, driver, 2, "Image")
        
    # Test that flipping between combine types resets non-supported settings
    # to default values.
    def test_layerDefaultSettings(self):    
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
        #Open image settings and stack.
        Util.load_image( self, driver, "Default")
        time.sleep( timeout )
        Util.openSettings( self, driver, "Image", True )
        Util.clickTab( driver, "Stack" )
        
        #Load the plus layer
        combineCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "layerCompositionMode")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", combineCombo )
        ActionChains(driver).click( combineCombo ).perform()
        ActionChains(driver).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
        
        #Change the transparency to 100
        time.sleep( timeout )
        transparencyText = driver.find_element_by_xpath( "//input[starts-with(@id,'maskAlphaTextField')]" )
        driver.execute_script( "arguments[0].scrollIntoView(true);", transparencyText)
        Util._changeElementText(self, driver, transparencyText, 100)
        
        #Click the red color box.
        filterBoxRed = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "filterRedBox")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", filterBoxRed )
        ActionChains(driver).click( filterBoxRed ).perform()
        
        #Verify the RGB spins are correct.
        self.verifyColor( driver, 255, 0, 0 )
    
        #Load the alpha layer.
        combineCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "layerCompositionMode")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", combineCombo )
        ActionChains(driver).click( combineCombo ).perform()
        ActionChains(driver).send_keys(Keys.ARROW_UP).send_keys(Keys.ENTER).perform()
        
        #Verify the color has been set back to white.
        self.verifyColor( driver, 0, 0, 0)
        
        #Load the none layer.
        combineCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "layerCompositionMode")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", combineCombo )
        ActionChains(driver).click( combineCombo ).perform()
        ActionChains(driver).send_keys(Keys.ARROW_UP).send_keys(Keys.ENTER).perform()
        
        #Verify the layer is now opaque.
        transparencyText = driver.find_element_by_xpath( "//input[starts-with(@id,'maskAlphaTextField')]" )
        driver.execute_script( "arguments[0].scrollIntoView(true);", transparencyText)
        alphaAmount = transparencyText.get_attribute( "value" )
        self.assertEqual( int(alphaAmount), 255, "Layer did not go opaque")
        
    #Test that layer settings work when multiple layers are selected.
    def test_selectMultipleLayers(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
        #Load images
        Util.load_image( self, driver, "Orion.methanol.cbc.contsub.image.fits")
        Util.load_image( self, driver, "aJ.fits")
        Util.load_image( self, driver, "aH.fits")
        
        #Open the image settings
        #Open the stack tab
        Util.openSettings( self, driver, "Image", True )
        Util.clickTab( driver, "Stack" )
        
        #Turn off auto select
        autoSelectCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "autoSelectImages")))
        ActionChains(driver).click( autoSelectCheck ).perform()
        
        #Select the last two images
        secondItem = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Image.Stack.ListItemIcon']/div[text()='aJ.fits']/..")))
        ActionChains(driver).key_down( Keys.CONTROL).click(secondItem).perform()
        
        #Change these images to plus layer combining
        combineCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "layerCompositionMode")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", combineCombo )
        ActionChains(driver).click( combineCombo ).perform()
        ActionChains(driver).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
        
        #Change the filter color to green
        time.sleep(2)
        filterBoxGreen = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "filterGreenBox")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", filterBoxGreen )
        ActionChains(driver).click( filterBoxGreen ).perform()
    
        #Turn on auto select
        autoSelectCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "autoSelectImages")))
        ActionChains(driver).click( autoSelectCheck ).perform()
        
        #Scroll through images and check the first two are plus with green and the
        #third one is not.
        # Click on Animator window so its actions will be enabled
        animWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")))
        ActionChains(driver).click( animWindow ).perform()
        # Find the next button
        forwardAnimateButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ImageTapeDeckIncrement"))) 
        driver.execute_script( "arguments[0].scrollIntoView(true);", forwardAnimateButton)
        
        #First layer
        ActionChains(driver).click( forwardAnimateButton ).perform()
        time.sleep(2)
        self.verifyCompositionMode( driver, "None");
        self.verifyColor( driver, 0, 0, 0 )
        
        #Second & third layer
        for i in range(0,2):
            print "Layer, ",i
            ActionChains(driver).click( forwardAnimateButton ).perform()
            time.sleep( timeout )
            self.verifyCompositionMode( driver,"Plus");
            self.verifyColor( driver, 0, 255, 0 )
        
        

    def tearDown(self):
        #Close the browser
        self.driver.close()
        #Allow browser to fully close before continuing
        time.sleep(2)
        #Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()        