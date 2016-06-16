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
        
    def _verifyRGB(self, driver, imageName, rgbStr ):
        xPath = "//div[@qxclass='skel.widgets.Image.Stack.TreeItem']/div[text()='" + imageName + "']/../div[@qxclass='skel.widgets.Image.Stack.CustomIcon']"
        item = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, xPath)))
        styleStr = item.get_attribute("style")
        print "Style=",styleStr
        rgb = item.value_of_css_property( 'background-color')
        print "RGB color=",rgb
        print "RGBSTR=", rgbStr
        self.assertTrue( rgb==rgbStr, "Red Icon not correct color")
            
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
        secondItem = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Image.Stack.TreeItem']/div[text()='aJ.fits']/..")))
        ActionChains(driver).context_click( secondItem ).perform()
        ActionChains(driver).send_keys( Keys.ARROW_DOWN ).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ENTER ).perform()
        
        #Verify the animator sees two images
        time.sleep( 2 )
        Util.verifyAnimatorUpperBound(self, driver, 1, "Image" )
        
        #Show the second image
        secondItem = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Image.Stack.TreeItem']/div[text()='aJ.fits']/..")))
        ActionChains(driver).context_click( secondItem ).perform()
        ActionChains(driver).send_keys( Keys.ARROW_DOWN ).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ENTER ).perform()
        
        #Verify the animator sees three images
        time.sleep( 2 )
        Util.verifyAnimatorUpperBound( self, driver, 2, "Image")
        
    # Load 3 images
    # Test that we can group the images into an RGB layer.
    # Test that we can ungroup the images.
    def test_groupUngroup(self):    
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
        #Load images
        Util.load_image( self, driver, "Orion.methanol.cbc.contsub.image.fits")
        Util.load_image( self, driver, "Orion.cont.image.fits")
        Util.load_image( self, driver, "TWHydra_CO2_1line.image.fits")
        time.sleep( 2 )
    
        
        #Open the image settings
        #Open the stack tab
        Util.openSettings( self, driver, "Image", True )
        Util.clickTab( driver, "Stack" )
        
         #Turn off auto select
        autoSelectCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "autoSelectImages")))
        ActionChains(driver).click( autoSelectCheck ).perform()
        
    
        #Select all images (The third should already be selected so selecting
        #the first with a shift should do it).
        firstItem = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Image.Stack.TreeItem']/div[text()='Orion.methanol.cbc.contsub.image.fits']/..")))
        actions = ActionChains( driver).key_down(Keys.SHIFT).click( firstItem )
        actions.key_up( Keys.SHIFT ).perform()
        

        #Click the group check box.
        groupCheck = WebDriverWait( driver, 10).until( EC.presence_of_element_located((By.ID, "stackGroupImages")))
        ActionChains(driver).click( groupCheck ).perform()
        time.sleep(2)
        
        #Verify that the images now have RGB boxes.
        self._verifyRGB( driver, "Orion.methanol.cbc.contsub.image.fits", "rgba(255, 0, 0, 1)")
        self._verifyRGB( driver, "Orion.cont.image.fits", "rgba(0, 255, 0, 1)")
        self._verifyRGB( driver, "TWHydra_CO2_1line.image.fits", "rgba(0, 0, 255, 1)")
        
        #Ungroup the images.
        groupCheck = WebDriverWait( driver, 10).until( EC.presence_of_element_located((By.ID, "stackGroupImages")))
        ActionChains(driver).click( groupCheck ).perform()
        time.sleep(2)
        
        #Verify the images have transparent RGB boxes.
        self._verifyRGB( driver, "Orion.methanol.cbc.contsub.image.fits", "rgba(0, 0, 0, 0)")
        self._verifyRGB( driver, "Orion.cont.image.fits", "rgba(0, 0, 0, 0)")
        self._verifyRGB( driver, "TWHydra_CO2_1line.image.fits", "rgba(0, 0, 0, 0)")
    
    #Test that we can rename a group.
    def test_groupRename(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
        #Load images
        Util.load_image( self, driver, "Orion.methanol.cbc.contsub.image.fits")
        Util.load_image( self, driver, "Orion.cont.image.fits")
        Util.load_image( self, driver, "TWHydra_CO2_1line.image.fits")
        time.sleep( 2 )
    
        
        #Open the image settings
        #Open the stack tab
        Util.openSettings( self, driver, "Image", True )
        time.sleep(4)
        Util.clickTab( driver, "Stack" )
        
         #Turn off auto select
        autoSelectCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "autoSelectImages")))
        ActionChains(driver).click( autoSelectCheck ).perform()
        
        #Group the bottom two images.
        secondItem = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Image.Stack.TreeItem']/div[text()='Orion.cont.image.fits']/..")))
        actions = ActionChains( driver).key_down(Keys.SHIFT).click( secondItem )
        actions.key_up( Keys.SHIFT ).perform()
        

        #Click the group check box.
        groupCheck = WebDriverWait( driver, 10).until( EC.presence_of_element_located((By.ID, "stackGroupImages")))
        ActionChains(driver).click( groupCheck ).perform()
        time.sleep(2)
        
        #Change the name of the group to twoImageRGB & verify that there is a tree node with that name..
        nameText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//input[starts-with(@id, 'stackLayerName')]"))) 
        Util._changeElementText(self, driver, nameText, "twoImageRGB")
        WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[contains(text(),'twoImageRGB')]")))

    def tearDown(self):
        #Close the browser
        self.driver.close()
        #Allow browser to fully close before continuing
        time.sleep(2)
        #Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()        