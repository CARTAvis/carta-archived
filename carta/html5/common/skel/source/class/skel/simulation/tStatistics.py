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
class tStatistics(unittest.TestCase):
    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp(self, browser)
        
        
    def show_statistics_window(self, driver):
        histWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowHistogram']")))
       
        ActionChains(driver).click( histWindow ).perform()
        time.sleep( 2 );
        viewMenuButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='View']/..")))
        ActionChains(driver).click( viewMenuButton ).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
        time.sleep( 2 )

    # Test that we can show the statistics display, and it will automatically
    # link to the single controller and update the stats when shown.
    def test_show_stats(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "Orion.methanol.cbc.contsub.image.fits")

        # Replace the histogram window with the statistics window.
        self.show_statistics_window(driver)
        
        #Test that we see the combo box for image statistices and that it has
        #the default image loaded.
        imageStatsNameCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID,"ImageStatsCombo")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", imageStatsNameCombo )
        imageStatsNameText = imageStatsNameCombo.find_element_by_xpath( ".//div/div")
        mapName = imageStatsNameText.text
        self.assertEqual( "Orion.methanol.cbc.contsub.image.fits", mapName, "Stat image name incorrect")
    
    #Test that we can show and hide an individual statistic
    def test_show_hide_stat(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "Orion.methanol.cbc.contsub.image.fits")
        
        # Replace the histogram window with the statistics window.
        self.show_statistics_window(driver)
        time.sleep( timeout )
        
        # Wait for the statisticsWindow to appear on the page and select something
        # in it so we see the settings chec.
        statWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowStatistics']")))
        imageStatsNameCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID,"ImageStatsCombo")))
        ActionChains(driver).click( imageStatsNameCombo ).perform()
    
        # Verify that we see the Frequency statistic.
        frequencyStat = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID,"FrequencyStat")))

        # Open the statistics settings.
        settingsCheck = Util.openSettings(self, driver, "Statistics", True )
        
        # Find the Frequency stat check box and uncheck it.
        freqCheckBox = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH,"//div[@id[starts-with(.,'FrequencyStatVisible')]]/div[@qxclass='qx.ui.basic.Image']")))
        Util.setChecked(self, driver, freqCheckBox, False)
        
        #Hide the statistics settings.
        Util.setChecked(self, driver, settingsCheck, False)
        
        #Verify we do not see the Frequency statistic.
        try :
            WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID,"FrequencyStat")))
            self.assertTrue( False, "Frequency statistic did not disapper")
        except Exception:
            print "Good, frequency statistic was hidden!"
        
        #Show the statistics settings
        Util.setChecked( self, driver, settingsCheck, True )
        
        # Now check the frequency check box to make the stat visible.
        freqCheckBox = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH,"//div[@id[starts-with(.,'FrequencyStatVisible')]]/div[@qxclass='qx.ui.basic.Image']")))
        Util.setChecked( self, driver, freqCheckBox, True )
        
        #Close the statistics settings
        Util.setChecked( self, driver, settingsCheck, False )
        
        #Verify we do see the Frequency statistic
        WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID,'FrequencyStat2')))
        
    #Tests that we can hide and show both image and region statistics as a group.
    def test_show_hide_stats(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "Orion.methanol.cbc.contsub.image.fits")
        
         # Load a region file.
        Util.load_image( self, driver, "region.crtf")
        
        # Replace the histogram window with the statistics window.
        self.show_statistics_window(driver)
        time.sleep( timeout )
        
        #Check that we see image stats.
        imageStatsNameCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID,"ImageStatsCombo")))
        
        #Check that we see region stats.
        regionStatsNameCombo = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID,"RegionStatsCombo")))
        ActionChains(driver).click( regionStatsNameCombo ).perform()
        
        #Uncheck the show image stats
        showImageStats = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID,"statsExpandCollapse")))
        ActionChains( driver ).click( showImageStats ).perform()
        
        #Verify we do not see the image stats.
        try:
            WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[contains(text(),'Shape:')]")))
        except Exception:
            print "Good - we did not see the image statistics"
            
        #Open the settings
        settingsCheck = Util.openSettings(self, driver, "Statistics", True )
        
        #Click the regions tab
        regionTab = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.tabview.TabButton']/div[contains(text(),'Region')]/..")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", regionTab)
        ActionChains(driver).click( regionTab ).perform()
        
        #Uncheck the show region stats
        showRegionStats = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH,"//div[@id[starts-with(.,'RegionShowStats')]]/div[@qxclass='qx.ui.basic.Image']")))
        Util.setChecked( self, driver, showRegionStats, False )
        
        #Close the settings
        Util.setChecked( self, driver, settingsCheck, False )
        
        #Verify the region stats are also gone.
        try:
            WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID,"RegionStatsCombo")))
        except Exception:
            print "Good - we did not see the region statistics"
            
        #Open the settings
        Util.setChecked( self, driver, settingsCheck, True )
        
        #Set the region stats visible
        showRegionStats = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH,"//div[@id[starts-with(.,'RegionShowStats')]]/div[@qxclass='qx.ui.basic.Image']")))
        Util.setChecked( self, driver, showRegionStats, True )
        
        #Close the settings
        Util.setChecked( self, driver, settingsCheck, False)
        
        #Set the image stats visible 
        showImageStats = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID,"statsExpandCollapse")))
        ActionChains( driver ).click( showImageStats ).perform()
        
        #Verify that we can see both image and region stats.
        WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID,"RegionStatsCombo")))
        WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[contains(text(),'Shape:')]")))

    def tearDown(self):
        # Close the browser
        self.driver.close()
        # Allow browser to fully close before continuing
        time.sleep(2)
        # Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()

