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
class tProfile(unittest.TestCase):
    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp(self, browser)
        
    def _checkProfileCount(self, driver, expected):
        Util.clickTab( driver, "Curves" )
        tableElement = driver.find_element_by_id('profileTable')
        cells = tableElement.find_elements_by_xpath(".//div[@class='qooxdoo-table-cell']")
        selectCount = len( cells )
        print "Profile curve count=", selectCount
        self.assertEqual( float(selectCount), expected, "Incorrect number of profiles loaded.")
        
    # Show the profile window, which is at the moment hidden.
    def _showProfile(self, driver ):
        histWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowHistogram']")))
        ActionChains(driver).click( histWindow ).perform()
        time.sleep(2)
        viewMenuButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='View']/..")))
        ActionChains(driver).click( viewMenuButton ).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

    # Test that we can change from frequency to wavelength units, alter the rest
    # frequency, and then reset it back to its original values.
    def test_restFrequencyReset(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        #Load a default image
        Util.load_image( self, driver, "Orion.methanol.cbc.contsub.image.fits")
        
        self._showProfile( driver )
        
        #Open the profile settings to the Profiles tab.
        profileWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowProfile']")))
        ActionChains(driver).click( profileWindow ).perform()
        Util.openSettings( self, driver, "Profile", True )
        Util.clickTab( driver, "Profiles" )
        
        #Set initial rest units to frequency in Hz and verify the
        #rest frequency of the image is 229759000000.
        frequencyUnitTypeRadio = WebDriverWait(driver, 10).until(EC.presence_of_element_located( ( By.ID, 'profileFrequencyUnitType' ) ) )
        ActionChains(driver).click( frequencyUnitTypeRadio ).perform()
        unitsCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located( ( By.ID, 'profileFrequencyUnits' ) ) )
        driver.execute_script( "arguments[0].scrollIntoView(true);", unitsCombo)
        ActionChains(driver).click( unitsCombo).perform()
        ActionChains(driver).send_keys(Keys.ENTER).perform()
        restFrequencyText = WebDriverWait(driver, 10).until(EC.presence_of_element_located( ( By.ID, 'profileRestFrequency' ) ) )
        restFrequency = restFrequencyText.get_attribute("value")
        print "Rest frequency=",restFrequency
        self.assertEqual( float(restFrequency), 229759000000, "Rest frequency was not correct")
        
    
        #Change the units to wavelength
        waveUnitTypeRadio = WebDriverWait(driver, 10).until(EC.presence_of_element_located( ( By.ID, 'profileWaveUnitType' ) ) )
        ActionChains(driver).click( waveUnitTypeRadio ).perform()
        
        #Verify the rest frequency has been converted to wavelength
        restFrequencyText = WebDriverWait(driver, 10).until(EC.presence_of_element_located( ( By.ID, 'profileRestFrequency' ) ) )
        restFrequencyWave = str(restFrequencyText.get_attribute("value"))
        print "Rest frequency wave=",restFrequencyWave
        self.assertEqual( float(restFrequencyWave), 1.30481, "Rest frequency wave was not correct")
        
        #Change the rest frequency
        Util._changeElementText(self, driver, restFrequencyText, '1.30482' )
        restFrequencyWave = str(restFrequencyText.get_attribute("value"))
        print "Rest frequency wave2=", restFrequencyWave
        self.assertEqual( float(restFrequencyWave), 1.30482, "Rest frequency wave was not correct")
        
        #Reset the rest frequency and verify it matches the original value.
        resetButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located( ( By.ID, 'profileRestReset' ) ) )
        ActionChains(driver).click( resetButton).perform()
        time.sleep( 1 )
        restFrequencyReset = str(restFrequencyText.get_attribute("value"))
        print "Rest frequency reset=",restFrequencyReset
        self.assertEqual( restFrequency, restFrequencyReset, "Rest frequency was not reset correctly")
        
    #Test that we can change the zoom percentage of the plot then the
    #corresponding values will update.
    def test_zoomValue(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Load an image
        Util.load_image(self, driver, "Orion.methanol.cbc.contsub.image.fits")
        
        self._showProfile( driver )
        
        #Open the profile settings to the Range tab.
        profileWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowProfile']")))
        ActionChains(driver).click( profileWindow ).perform()
        time.sleep(2)
        Util.openSettings( self, driver, "Profile", True )
        time.sleep(2)
        Util.clickTab( driver, "Range" )

        # Look for the min and max zoom values and check their values.
        minZoomValue = Util._getTextValue( self, driver, 'profileZoomMinValue')
        print "Min zoom=", minZoomValue
        maxZoomValue = Util._getTextValue( self, driver, 'profileZoomMaxValue')
        print "Max zoom=", maxZoomValue
        self.assertEqual( float(minZoomValue), 0, "Min zoom value was not correct")
        self.assertEqual( float(maxZoomValue), 40, "Max zoom value was not correct")

        # Find the min and max zoom percentages.  Decrease their values.
        minPercentText = driver.find_element_by_id("profileZoomMinPercent")
        driver.execute_script( "arguments[0].scrollIntoView(true);", minPercentText)
        minZoomPercent = minPercentText.get_attribute( "value")
        maxPercentText = driver.find_element_by_id("profileZoomMaxPercent")
        maxZoomPercent = maxPercentText.get_attribute( "value")
        driver.execute_script( "arguments[0].scrollIntoView(true);", maxPercentText)
        incrementAmount = 10;
        newMinZoomPercent = Util._changeElementText(self, driver, minPercentText, str(float(minZoomPercent) + incrementAmount))
        newMaxZoomPercent = Util._changeElementText(self, driver, maxPercentText, str(float(maxZoomPercent) - incrementAmount))
        time.sleep( timeout )

        # Get the new min and max zoom values.
        newMinZoomValue = Util._getTextValue( self, driver, "profileZoomMinValue")
        newMaxZoomValue = Util._getTextValue( self, driver, "profileZoomMaxValue")
        print "New min zoom=",newMinZoomValue
        print "New max zoom=",newMaxZoomValue
        self.assertEqual( float(newMinZoomValue), 4, "Min zoom value was not correct")
        self.assertEqual( float(newMaxZoomValue), 36, "Max zoom value was not correct")

       
    #Test that we can change the zoom value of a plot and the
    #corresponding zoom percentages will update.
    def test_zoomPercentage(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Load an image
        Util.load_image(self, driver, "Orion.methanol.cbc.contsub.image.fits")
        
        # Show the profile view
        self._showProfile( driver )
        
        #Open the profile settings to the Range tab.
        profileWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowProfile']")))
        ActionChains(driver).click( profileWindow ).perform()
        Util.openSettings( self, driver, "Profile", True )
        time.sleep(2)
        Util.clickTab( driver, "Range" )

        # Look for the min and max zoom percentages and check their values.
        minZoomPercent = Util._getTextValue( self, driver, 'profileZoomMinPercent')
        print "Min zoom percent=", minZoomPercent
        maxZoomPercent = Util._getTextValue( self, driver, 'profileZoomMaxPercent')
        print "Max zoom percent=", maxZoomPercent
        self.assertEqual( float(minZoomPercent), 0, "Min zoom percent was not correct")
        self.assertEqual( float(maxZoomPercent), 100, "Max zoom percent was not correct")

        # Find the min and max zoom values.  Change their values.
        minText = driver.find_element_by_id("profileZoomMinValue")
        driver.execute_script( "arguments[0].scrollIntoView(true);", minText)
        maxText = driver.find_element_by_id("profileZoomMaxValue")
        driver.execute_script( "arguments[0].scrollIntoView(true);", maxText)
        Util._changeElementText(self, driver, minText, '4')
        Util._changeElementText(self, driver, maxText, '36')
        time.sleep( timeout )

        # Get the new min and max zoom values.
        newMinZoomPercent = Util._getTextValue( self, driver, "profileZoomMinPercent")
        newMaxZoomPercent= Util._getTextValue( self, driver, "profileZoomMaxPercent")
        print "New min zoom=",newMinZoomPercent
        print "New max zoom=",newMaxZoomPercent
        self.assertEqual( float(newMinZoomPercent), 10, "Min zoom percent was not correct")
        self.assertEqual( float(newMaxZoomPercent), 90, "Max zoom percent was not correct")
        
    #Load three images, one of which is a single plane image.
    #Test that if the generate mode is current, only one profile will be seen.
    #Test that if the generate mode is all, we see three images.
    #Test that if the generate mode is all but single plane, we see two images.
    def test_generateMode(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
         # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Load three images
        Util.load_image(self, driver, "Orion.methanol.cbc.contsub.image.fits")
        time.sleep(1)
        Util.load_image(self, driver, "TWHydra_CO2_1line.image.fits")
        time.sleep(1)
        Util.load_image(self, driver, "Orion.cont.image.fits")
        time.sleep(1)
        
         # Show the profile view
        self._showProfile( driver )
        
        #Open the profile settings to the Profiles tab.
        profileWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowProfile']")))
        ActionChains(driver).click( profileWindow ).perform()
        time.sleep(2)
        Util.openSettings( self, driver, "Profile", True )
        time.sleep(2)
        Util.clickTab( driver, "Profiles" )
        
        #Get the profiles combo and count how many are listed.  Since
        #the mode is current there should be just one.
        self._checkProfileCount( driver, 1)
        
        #Change the generate mode to all
        Util.clickTab( driver, "Profiles")
        time.sleep(1)
        genSelect = driver.find_element_by_id("profileGenerateMode")
        driver.execute_script( "arguments[0].scrollIntoView(true);", genSelect )
        ActionChains(driver).click( genSelect ).send_keys(
            Keys.ARROW_DOWN).send_keys( Keys.ENTER).perform()
    
        #There should now be three profiles loaded.
        time.sleep(2)
        self._checkProfileCount( driver, 3 )
    
        
        #Change the generate mode to all except single plane.
        Util.clickTab( driver, "Profiles")
        ActionChains(driver).click( genSelect ).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
            
        #There should now be two profiles loaded.
        time.sleep(2)
        self._checkProfileCount( driver, 2 )
        
    

    #Test that we can remove a profile curve.
    def test_removeProfile(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
        #Load two profiles
        Util.load_image(self, driver, "Orion.methanol.cbc.contsub.image.fits")
        time.sleep(1)
        Util.load_image(self, driver, "TWHydra_CO2_1line.image.fits")
        time.sleep(1)
        
         # Show the profile view
        self._showProfile( driver )
        time.sleep(2)
        
        #Open the profile settings to the Profiles tab.
        profileWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowProfile']")))
        ActionChains(driver).click( profileWindow ).perform()
        time.sleep(2)
        Util.openSettings( self, driver, "Profile", True )
        time.sleep(2)
        Util.clickTab( driver, "Profiles" )
        
        #Change the generate mode to all
        Util.clickTab( driver, "Profiles")
        time.sleep(1)
        genSelect = driver.find_element_by_id("profileGenerateMode")
        driver.execute_script( "arguments[0].scrollIntoView(true);", genSelect )
        ActionChains(driver).click( genSelect ).send_keys(
            Keys.ARROW_DOWN).send_keys( Keys.ENTER).perform()
            
        #There should now be two profiles loaded.
        time.sleep(2)
        self._checkProfileCount( driver, 2 )
        
        #Uncheck auto-generate so we can customize the profiles.
        Util.clickTab( driver, "Profiles")
        autoCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='profileAutoGenerate']/div")))
        Util.setChecked(self, driver, autoCheck, False)
        time.sleep(timeout)
        
        #Hit the delete button
        deleteButton = driver.find_element_by_id("profileRemoveButton")
        driver.execute_script( "arguments[0].scrollIntoView(true);", deleteButton )
        ActionChains(driver).click( deleteButton ).perform()
        
        #There should now be just one profile loaded
        time.sleep(2)
        self._checkProfileCount( driver, 1 )
        
    # Test that we can profile a region in an image
    def test_profileRegion(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        #Load a default image
        Util.load_image( self, driver, "Orion.methanol.cbc.contsub.image.fits")
        time.sleep( timeout )
        
        #Load a region for the image
        Util.load_image( self, driver, "OrionMethanolRegion.crtf")
        time.sleep( timeout )
        
        self._showProfile( driver )
        time.sleep( timeout )
        
        #Open the profile settings to the Profiles tab.
        profileWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowProfile']")))
        ActionChains(driver).click( profileWindow ).perform()
        time.sleep( timeout )
        
        Util.openSettings( self, driver, "Profile", True )
        time.sleep( timeout )
        
        Util.clickTab( driver, "Profiles" )
        time.sleep( timeout )
        
        #Check that there is a region we are profiling
        regionCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ProfileSelectedRegion")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", regionCombo )
        nameDiv = regionCombo.find_element_by_xpath( ".//div/div")
        regionName = nameDiv.get_attribute( "innerHTML").strip()
        print "Region is ",regionName
        self.assertTrue( regionName != "None", "Expected a region to be profiled")
        
        
    def tearDown(self):
        #Close the browser
        self.driver.close()
        #Allow browser to fully close before continuing
        time.sleep(2)
        #Close the session and delete temporary files
        self.driver.quit()
    

if __name__ == "__main__":
    unittest.main()
