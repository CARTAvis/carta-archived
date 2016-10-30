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
    
    def _getRestFrequency(self, driver):
        restFrequencyText = WebDriverWait(driver, 10).until(EC.presence_of_element_located( ( By.ID, 'profileRestFrequency' ) ) )
        restFrequency = restFrequencyText.get_attribute("value")
        print "Rest frequency=",restFrequency
        return restFrequency
      
    # Show the profile window, which is at the moment hidden.
    def _showProfile(self, driver ):
        histWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowHistogram']")))
        ActionChains(driver).click( histWindow ).perform()
        time.sleep(2)
        viewMenuButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='View']/..")))
        ActionChains(driver).click( viewMenuButton ).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
            
    def _animateForward(self, animator, driver):
        forwardAnimateButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, animator+"TapeDeckIncrement")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", forwardAnimateButton)
        ActionChains(driver).click( forwardAnimateButton ).perform()
    
    def _hitNew(self, driver):
        newButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID,"profileNewButton")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", newButton )
        ActionChains(driver).click( newButton ).perform()
        
    def _hitDelete(self, driver, time ):
        deleteButton = WebDriverWait(driver,10).until(EC.presence_of_element_located((By.ID,"profileRemoveButton")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", deleteButton )
        ActionChains(driver).move_to_element( deleteButton ).perform()
        time.sleep(2)
        ActionChains(driver).click( deleteButton ).perform()
        
    def _setAutoGenerate(self, driver, auto ):
        autoCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='profileAutoGenerate']/div")))
        Util.setChecked(self, driver, autoCheck, auto)

        
    def _checkRegionProfiled(self, expected, driver ):
        comboPath = "//div[starts-with(@id,'ProfileSelectedRegion')]/div/div"
        regionCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, comboPath)))
        driver.execute_script( "arguments[0].scrollIntoView(true);", regionCombo )
        regionComboVal = regionCombo.text
        print "Region profiled, ", regionComboVal
        if ( expected not in regionComboVal ):
            self.assertTrue( False, "Not profiling correct region")
        profileCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[starts-with(@id,'profileNameSelect')]/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", profileCombo )
        profileComboVal = profileCombo.get_attribute( "value")
        print "Profile name, ",profileComboVal
        if ( expected not in profileComboVal ):
            self.assertTrue( False, "Not profiling correct profile")
        
    def _checkImageProfiled(self, expected, driver ):
        comboPath = "//div[starts-with(@id,'ProfileSelectedImage')]/div/div"
        imageCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, comboPath)))
        driver.execute_script( "arguments[0].scrollIntoView(true);", imageCombo )
        imageComboVal = imageCombo.text
        print "Image profiled, ", imageComboVal
        self.assertEqual( expected, imageComboVal, "Second image was not selected")
        profileCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[starts-with(@id,'profileNameSelect')]/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", profileCombo )
        profileComboVal = profileCombo.get_attribute( "value")
        print "Profile name, ",profileComboVal
        if ( expected not in profileComboVal ):
            self.assertTrue( False, "Not profiling correct image")

    # Test that we can change from frequency to wavelength units, alter the rest
    # frequency, and then reset it back to its original values.
    def test_restFrequencyReset(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        #Load a default image
        Util.load_image( self, driver, "Orion.methanol.cbc.contsub.image.fits")
        time.sleep(1)
        self._showProfile( driver )
        time.sleep(2)
        
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
        
        restFrequency = self._getRestFrequency(driver)
        self.assertEqual( float(restFrequency), 229759000000, "Rest frequency was not correct")
        time.sleep(10)
    
        #Change the units to wavelength
        waveUnitTypeRadio = WebDriverWait(driver, 10).until(EC.presence_of_element_located( ( By.ID, 'profileWaveUnitType' ) ) )
        driver.execute_script( "arguments[0].scrollIntoView(true);", waveUnitTypeRadio)
        ActionChains(driver).move_to_element( waveUnitTypeRadio ).perform()
        ActionChains(driver).click( waveUnitTypeRadio ).perform()
        time.sleep(10)
        
        
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
        time.sleep(4)
        
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
        time.sleep(4)
        
        # Show the profile view
        self._showProfile( driver )
        time.sleep(4)
        
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
        time.sleep(4)
        
         # Show the profile view
        self._showProfile( driver )
        time.sleep(4)
        
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
        time.sleep(6)
        self._checkProfileCount( driver, 2 )
    
    
    #Load one image and two regions
    #Test that if the generate mode is current, only one profile will be seen.
    #Test that if the generate mode is all, we see two profiles.
    #Test that if the generate mode is all but single plane, we also see two profiles.
    def test_generateModeRegion(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
         # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Load one image and two regions
        Util.load_image(self, driver, "Orion.methanol.cbc.contsub.image.fits")
        time.sleep(1)
        Util.load_image(self, driver, "OrionMethanolRegion.crtf")
        time.sleep(1)
        Util.load_image(self, driver, "OrionMethanolRegionEllipse.crtf")
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
    
        #There should now be two profiles loaded.
        time.sleep(2)
        self._checkProfileCount( driver, 2 )
    
        
        #Change the generate mode to all except single plane.
        Util.clickTab( driver, "Profiles")
        ActionChains(driver).click( genSelect ).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
            
        #There should now be two profiles loaded.
        time.sleep(2)
        self._checkProfileCount( driver, 2 )
    
    
    #Test that if we are on Auto generate current mode, then we can animate through images and
    #see the profile updated.
    def test_autoGenerateCurrent(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
          #Load two profiles
        firstImage = "Orion.methanol.cbc.contsub.image.fits"
        Util.load_image(self, driver, firstImage )
        time.sleep(1)
        secondImage = "TWHydra_CO2_1line.image.fits"
        Util.load_image(self, driver, secondImage )
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
        
        #Check that the profile is on the second image.
        self._checkImageProfiled( secondImage, driver )
        
        #Animate to the first image
        self._animateForward( "Image", driver )
        time.sleep( timeout )
        
        #Check that the profile is on the first image
        self._checkImageProfiled( firstImage, driver )
        
        #Animate to the first image
        self._animateForward( "Image", driver)
        time.sleep( timeout )
        
        #Check that the profile is on the second image
        self._checkImageProfiled( secondImage, driver )
        
        
    #Test that if we are on Auto generate current mode, then we can animate through regions and
    #see the profile updated.
    def test_autoGenerateCurrentRegion(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
        #Load one image and two regions
        Util.load_image(self, driver, "Orion.methanol.cbc.contsub.image.fits" )
        time.sleep(1)
        Util.load_image(self, driver, "OrionMethanolRegion.crtf" )
        time.sleep( 1)
        Util.load_image(self, driver, "OrionMethanolRegionEllipse.crtf" )
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
             
        #Check that the profile is on the second Region.
        self._checkRegionProfiled( "ellipse", driver )
        
        #Animate to the first region
        self._animateForward( "Region", driver )
        time.sleep( timeout )
        
        #Check that the profile is on the first region
        self._checkRegionProfiled( "rectangle", driver )
        
        #Animate to the second region
        self._animateForward( "Region", driver)
        time.sleep( timeout )
        
        #Check that the profile is on the second region
        self._checkRegionProfiled( "ellipse", driver ) 
        
    #Load two images; initially the mode should be auto generate and current so we
    #should see only one profile.  Turn off auto generate, change the image, and
    #hit the new button.  We should now see profiles for both images.
    #Turn auto generate back on, and we should only see one profile.
    def test_newProfileExisting(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()
        print "Timeout=",timeout
        
        #Load two images
        Util.load_image(self, driver, "Orion.methanol.cbc.contsub.image.fits" )
        time.sleep(1)
        Util.load_image(self, driver, "TWHydra_CO2_1line.image.fits" )
        time.sleep( 1)
        
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
        
        #Get the profiles combo and count how many are listed.  Since
        #the mode is auto generate and current there should be just one.
        self._checkProfileCount( driver, 1)
        
        #Turn off auto generate.
        Util.clickTab( driver, "Profiles" )
        self._setAutoGenerate( driver, False )
        time.sleep( timeout )
        
        #Store the image rest frequency.
        restFrequency = self._getRestFrequency( driver )
        
        #Select the other image and test the rest frequency changes to match
        #the selected image
        comboPath = "//div[starts-with(@id,'ProfileSelectedImage')]"
        imageCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, comboPath)))
        driver.execute_script( "arguments[0].scrollIntoView(true);", imageCombo )
        ActionChains(driver).click( imageCombo ).perform()
        ActionChains(driver).click( imageCombo).send_keys(Keys.UP).send_keys( Keys.ENTER).perform()
        time.sleep( timeout )
        newRestFrequency = self._getRestFrequency( driver )
        self.assertNotEqual( restFrequency, newRestFrequency, "Image rest frequency did not change")
        
        #Hit the new profile button.
        self._hitNew( driver )
        time.sleep( 4 )
    
        #Verify that we now have two profiles.
        self._checkProfileCount( driver, 2 )
        
        #Turn on auto generate
        Util.clickTab( driver, "Profiles" )
        self._setAutoGenerate( driver, True )
        time.sleep( timeout )
        
        #Verify that we are back to one profile.
        self._checkProfileCount( driver, 1 )
    
    #Test that we can load a non existent profile    
    def test_newProfile(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
        #Load two images
        Util.load_image(self, driver, "Orion.methanol.cbc.contsub.image.fits" )
        time.sleep(1)
        Util.load_image(self, driver, "Orion.cont.image.fits" )
        time.sleep( 1)
        
        # Show the profile view
        self._showProfile( driver )
        time.sleep(2)
        
        # Open the profiles tab
         #Open the profile settings to the Profiles tab.
        profileWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowProfile']")))
        ActionChains(driver).click( profileWindow ).perform()
        time.sleep(2)
        Util.openSettings( self, driver, "Profile", True )
        time.sleep(2)
        Util.clickTab( driver, "Profiles" )
        
        # Change current to all and verify that we see two profiles.
        genSelect = driver.find_element_by_id("profileGenerateMode")
        driver.execute_script( "arguments[0].scrollIntoView(true);", genSelect )
        ActionChains(driver).click( genSelect ).send_keys(
            Keys.ARROW_DOWN).send_keys( Keys.ENTER).perform()
            
        # Turn off auto-generate
        self._setAutoGenerate( driver, False )
        time.sleep( timeout )
        
        # Delete one of the profiles and verify that there is just one profile.
        self._hitDelete( driver, time )
        time.sleep( 4 )
        self._checkProfileCount(driver, 1)
        
        # Add the image to that of the profile that was deleted.
        Util.clickTab( driver, "Profiles" )
        time.sleep( timeout)
        self._hitNew( driver )
        time.sleep( timeout)
        
        # Verify that there are two profiles.
        self._checkProfileCount( driver, 2)
        
    #Test that we can load a new profile with everything the same except for a different
    #frequency from an existing profile.
    def test_newProfileFrequency(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()  
        
        #Load one image
        Util.load_image(self, driver, "Orion.methanol.cbc.contsub.image.fits" )
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
        
        #Get the profiles combo and count how many are listed.  Since
        #the mode is auto generate and current there should be just one.
        self._checkProfileCount( driver, 1)
        
        #Turn off auto generate.
        Util.clickTab( driver, "Profiles" )
        self._setAutoGenerate( driver, False )
        time.sleep( timeout )
        
        #Change the frequency to a different value.
        restFrequency = self._getRestFrequency( driver )
        self.assertEqual( float(restFrequency), 229759000000, "Rest frequency was not correct")
        newRestFrequency = 229000000000
        freqText = WebDriverWait(driver, 10).until(EC.presence_of_element_located( ( By.ID, 'profileRestFrequency' ) ) )
        Util._changeElementText( self, driver, freqText, newRestFrequency )
        
        #Hit the new profile button.
        self._hitNew( driver )
        time.sleep( 4 )
    
        #Verify that we now have two profiles.
        self._checkProfileCount( driver, 2 )
        
    #Test that we can load a new profile with everything the same except for a different
    #statistic from an existing profile.
    def test_newProfileStatistic(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()  
        
        #Load one image
        Util.load_image(self, driver, "Orion.methanol.cbc.contsub.image.fits" )
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
        
        #Get the profiles combo and count how many are listed.  Since
        #the mode is auto generate and current there should be just one.
        self._checkProfileCount( driver, 1)
        
        #Turn off auto generate.
        Util.clickTab( driver, "Profiles" )
        self._setAutoGenerate( driver, False )
        time.sleep( timeout )
        
        #Change the statistics to a different value.
        comboPath = "//div[starts-with(@id,'profileAggregateStatistics')]"
        statsCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, comboPath)))
        driver.execute_script( "arguments[0].scrollIntoView(true);", statsCombo )
        ActionChains(driver).click( statsCombo ).perform()
        ActionChains(driver).click( statsCombo).send_keys(Keys.DOWN).send_keys(Keys.DOWN).send_keys( Keys.ENTER).perform()
        time.sleep( timeout )
        
        #Hit the new profile button.
        self._hitNew( driver )
        time.sleep( 4 )
    
        #Verify that we now have two profiles.
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
        #autoCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='profileAutoGenerate']/div")))
        #Util.setChecked(self, driver, autoCheck, False)
        #time.sleep(timeout)
        self._setAutoGenerate( False )
        
        #Hit the delete button
        self._hitDelete( driver, time )
        
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
