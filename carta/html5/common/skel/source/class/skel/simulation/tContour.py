import unittest
import Util
import time
import selectBrowser
from selenium import webdriver
from flaky import flaky
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait

#Test drawing contours.
@flaky(max_runs=3)
class tContour(unittest.TestCase):
    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp(self, browser)



    # Click the tab containing contour functionality
    def _clickContourTab(self, driver ):
        contourTab = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.tabview.TabButton']/div[contains(text(),'Contour')]/..")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", contourTab)
        ActionChains(driver).click( contourTab ).perform()

    # Click the default contour set tab.
    def _clickDefaultContourSetTab(self, driver ):
        defaultSetTab = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.tabview.TabButton']/div[contains(text(),'Default')]/..")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", defaultSetTab)
        ActionChains(driver).click( defaultSetTab ).perform()

    # Click the generate contour set tab.
    def _clickGenerateContourSetTab(self, driver ):
        defaultSetTab = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.tabview.TabButton']/div[contains(text(),'Contour Sets')]/..")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", defaultSetTab)
        ActionChains(driver).click( defaultSetTab ).perform()

    # Click the add a contour set button from the generate tab.
    def _clickAddContourSetButton(self, driver):
        addButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ImageContourGenerateAddButton")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", addButton)
        ActionChains(driver).click( addButton ).perform()

    # Click the delete a contour set button from the generate tab.
    def _clickDeleteContourSetButton(self, driver):
        deleteButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ImageContourGenerateDeleteButton")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", deleteButton)
        ActionChains(driver).click( deleteButton ).perform()

    # Return the number of contour levels in the default set.
    def _getLevels(self, driver, tableId ):
        tableDiv  = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, tableId )))
        levelElements = tableDiv.find_elements_by_xpath( ".//div/div/div/div/div/div[@class='qooxdoo-table-cell']")
        levelCount = len(levelElements)
        levelList = []
        for i in range(levelCount):
            print 'Text=', levelElements[i].text
            levelList.append( levelElements[i].text)
        return levelList

    # Set the number of contour levels to generate.
    def _setLevelCount(self, driver, value):
        levelSpinParent = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "imageContourLevelCount")))
        #Upper spin button
        levelSpinInput = levelSpinParent.find_element_by_xpath( ".//input" )
        currentValue = levelSpinInput.get_attribute("value")
        print "Current level ", currentValue
        diff = value - int(currentValue)
        print "Diff=",diff
        levelButs = levelSpinParent.find_elements_by_class_name( "qx-button")
        buttonCount = len( levelButs )
        print "Button count=",buttonCount
        buttonIndex = 0

        if diff < 0 :
            #Use the decrease button if both buttons are enabled
            if buttonCount > 1 :
                buttonIndex = 1
            #levelSpinButton = levelSpinParent.find_element_by_xpath( ".//div[1]")
            diff = diff * -1

        ActionChains( driver).move_to_element( levelButs[buttonIndex] )
        for i in range( diff ):
            ActionChains(driver).click( levelButs[buttonIndex] ).perform()
        newValue = levelSpinInput.get_attribute("value")
        self.assertEqual( newValue, str(value), "Level spin not correct")

    # Type a numeric value into a text box with the given id.
    def _typeText(self,driver,textId,value):
        textBoxParent = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, textId)))
        textBox = textBoxParent.find_element_by_xpath( ".//input" )
        driver.execute_script( "arguments[0].scrollIntoView(true);", textBox)
        elementValue = Util._changeElementText( self, driver, textBox, str(value))
        self.assertEqual( str(value), elementValue, "Numeric value not correct")
        
    # Test that we can add a default Contour set using the percentile method,
    # linear spacing, 3 levels, and a minimum level of 20 and a maximum level of 90.
    def test_percentileMethod(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        #Load a default image
        Util.load_image( self, driver, "aH.fits")
        time.sleep(2)

        #Show the image settings
        Util.openSettings( self, driver, "Image", True )

        # Navigate to Contour tab of the Image Settings
        self._clickContourTab( driver )

        # Set a minimum contour level of 10 and a maximum contour level of 90
        self._typeText( driver, "imageContourLevelMin", 20 )
        self._typeText( driver, "imageContourLevelMax", 90 )

        # Change the level count to 3.
        self._setLevelCount( driver, 3 )

        # Hit the Add/Update ContourSet button
        self._clickAddContourSetButton( driver )

        # Check that there is now a Tab called Default under Contour Sets
        self._clickDefaultContourSetTab( driver )

       

    # Test that we can add a default Contour set using the range method,
    # linear spacing, 3 levels, and a minimum level of 0.1 and a maximum level of 0.9.
    def test_rangeMethod(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        #Load a default image
        Util.load_image( self, driver, "aH.fits")

        #Show the image settings
        Util.openSettings( self, driver, "Image", True )

        # Navigate to Contour tab of the Histogram Settings
        self._clickContourTab( driver )

        # Set a minimum contour level of 0.1 and a maximum contour level of 0.9
        self._typeText( driver, "imageContourLevelMin", 0.1 )
        self._typeText( driver, "imageContourLevelMax", 0.9 )

        # Change the level count to 3.
        self._setLevelCount( driver, 3 )

        # Hit the Add/Update ContourSet button
        self._clickAddContourSetButton( driver )

        # Check that there is now a Tab called Default under Contour Sets
        self._clickDefaultContourSetTab( driver )

        # Check that we have levels 0.1, 0.5, 0.9
        levels = self._getLevels( driver, "contourLevelListDefault" )

        self.assertEqual( len(levels), 3, "Three contour levels were not generated")
        self.assertEqual( levels[0], str(0.1), "First contour was not 0.1")
        self.assertEqual( levels[1], str(0.5), "Second contour was not 0.5")
        self.assertEqual( levels[2], str(0.9), "Third contour was not 0.9")

    # Test that we can add a contour set and then delete it.  In the process we test
    # that we can add a contour set using the minimum method.
    def test_deleteContourSet(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        #Load a default image
        Util.load_image( self, driver, "aH.fits")

        #Show the image settings
        Util.openSettings( self, driver, "Image", True )

        # Navigate to Contour tab of the Histogram Settings
        self._clickContourTab( driver )

        # Change the method used to generate the contour set to minimum.
        generateButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "contourGenerateMethod")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", generateButton)
        ActionChains(driver).click( generateButton ).perform()
        ActionChains(driver).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        # Set a minimum contour level of -0.1
        self._typeText( driver, "imageContourLevelMin", -0.1 )

        # Use 2 levels
        self._setLevelCount( driver, 2)

        # Set the spacing to 2
        self._typeText(driver, "contourSpacingInterval", 1)

        # Hit the Add/Update ContourSet button
        self._clickAddContourSetButton( driver )

        # Check that there is now a Tab called Default under Contour Sets
        self._clickDefaultContourSetTab( driver )

        # Check that we have levels -0.1 and 0.9
        levels = self._getLevels( driver, "contourLevelListDefault" )

        # Check that the levels are correct
        self.assertEqual( len(levels), 2, "Two contour levels were not generated")
        self.assertEqual( levels[0], str(-0.1), "First contour was not -0.1")
        self.assertEqual( levels[1], str(0.9), "Second contour was not 0.9")

        # Change back to the generator tab
        self._clickGenerateContourSetTab( driver )

        # Now delete the contour set.
        self._clickDeleteContourSetButton( driver )

        # Verify the default contour set is no longer there.
        try:
            defaultSetTab = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.tabview.TabButton']/div[contains(text(),'Default')]/..")))
            self.assertTrue( False, "Should not be able to find default contour set tab.")
        except Exception:
            print "Test succeeded because we should not be able to find a default contour set tab"

    def tearDown(self):
        #Close the browser
        self.driver.close()
        #Allow browser to fully close before continuing
        time.sleep(2)
        #Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()
