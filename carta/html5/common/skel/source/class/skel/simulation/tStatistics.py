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
    
    def _checkRegionSumValue(self, driver, correctValue):
        sumValue = Util._getTextValue( self, driver, "SumStat")
        print "Sum is ", sumValue
        self.assertEqual( float(sumValue), correctValue, "Region sum not correct")
            
        
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
    
    # Test that we can display the correct statistics for a rectangle region.  Then test that the
    # statistics will update correctly if we change the size of the rectangle.
    def test_rectangleRegion(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "Orion.methanol.cbc.contsub.image.fits")
        time.sleep( timeout )
        
        #Load a rectangular region
        Util.load_image(self, driver, "OrionMethanolRegion.crtf")
        time.sleep( timeout )

        # Replace the histogram window with the statistics window.
        self.show_statistics_window(driver)
        
        #Test that we see the region text field labelled "Sum" and that its value is correct.
        self._checkRegionSumValue( driver, -11.5961 )
        
        #Now open the regions tab of the image window
        #Open the image settings
        ActionChains(driver).double_click( imageWindow ).perform()
        Util.openSettings( self, driver, "Image", True )
        time.sleep(4)
        
         #Open the regions tab
        Util.clickTab( driver, "Regions" )
        time.sleep( timeout )
        
        #Change the width and the height of the region to 10.
        heightText = driver.find_element_by_xpath("//div[@id='RectangleRegionHeight']/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", heightText )
        Util._changeElementText(self, driver, heightText, 10)
        widthText = driver.find_element_by_xpath( "//div[@id='RectangleRegionWidth']/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", widthText )
        Util._changeElementText(self, driver, widthText, 10)
        time.sleep( timeout )
        
        #Check that the sum has updated to a new correct value
        self._checkRegionSumValue( driver, 14.3478)
        
    # Test that we can display the correct statistics for a DS9 rectangle region.  Then test that the
    # statistics will update correctly if we change the size of the rectangle.
    def test_rectangleRegionDS9(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "Orion.methanol.cbc.contsub.image.fits")
        time.sleep( timeout )
        
        #Load a rectangular region
        Util.load_image(self, driver, "OrionMethanolRegion.reg")
        time.sleep( timeout )

        # Replace the histogram window with the statistics window.
        self.show_statistics_window(driver)
        
        #Test that we see the region text field labelled "Sum" and that its value is correct.
        self._checkRegionSumValue( driver, -6.03696 )
        
        #Now open the regions tab of the image window
        #Open the image settings
        ActionChains(driver).double_click( imageWindow ).perform()
        Util.openSettings( self, driver, "Image", True )
        time.sleep(4)
        
         #Open the regions tab
        Util.clickTab( driver, "Regions" )
        time.sleep( timeout )
        
        #Change the width and the height of the region to 10.
        heightText = driver.find_element_by_xpath("//div[@id='RectangleRegionHeight']/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", heightText )
        Util._changeElementText(self, driver, heightText, 10)
        widthText = driver.find_element_by_xpath( "//div[@id='RectangleRegionWidth']/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", widthText )
        Util._changeElementText(self, driver, widthText, 10)
        time.sleep( timeout )
        
        #Check that the sum has updated to a new correct value
        self._checkRegionSumValue( driver, 28.7604)
        
    # Test that we can display the correct statistics for an elliptical region.  Then test that the
    # statistics will update correctly if we change the size of the ellipse.
    def test_ellipseRegion(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "Orion.methanol.cbc.contsub.image.fits")
        time.sleep( timeout )
        
        #Load an elliptical region
        Util.load_image(self, driver, "OrionMethanolRegionEllipse.crtf")
        time.sleep( timeout )

        # Replace the histogram window with the statistics window.
        self.show_statistics_window(driver)
        
        #Test that we see the region text field labelled "Sum" and that its value is correct.
        self._checkRegionSumValue( driver, -11.4258 )
        
        #Now open the regions tab of the image window
        #Open the image settings
        ActionChains(driver).double_click( imageWindow ).perform()
        Util.openSettings( self, driver, "Image", True )
        time.sleep(4)
        
         #Open the regions tab
        Util.clickTab( driver, "Regions" )
        time.sleep( timeout )
        
        #Change the width and the height of the region to 10.
        minorRadiusText = driver.find_element_by_xpath("//div[@id='EllipseRegionMinorRadius']/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", minorRadiusText )
        Util._changeElementText(self, driver, minorRadiusText, 10)
        majorRadiusText = driver.find_element_by_xpath( "//div[@id='EllipseRegionMajorRadius']/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", majorRadiusText )
        Util._changeElementText(self, driver, majorRadiusText, 10)
        time.sleep( timeout )
        
        #Check that the sum has updated to a new correct value
        self._checkRegionSumValue( driver, 28.2493)
    
    # Test that we can display the correct statistics for an elliptical region in DS9 format.  Then test that the
    # statistics will update correctly if we change the size of the ellipse.
    def test_ellipseRegionDS9(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "Orion.methanol.cbc.contsub.image.fits")
        time.sleep( timeout )
        
        #Load an elliptical region
        Util.load_image(self, driver, "OrionMethanolRegionEllipse.reg")
        time.sleep( timeout )

        # Replace the histogram window with the statistics window.
        self.show_statistics_window(driver)
        
        #Test that we see the region text field labelled "Sum" and that its value is correct.
        self._checkRegionSumValue( driver, 25.481 )
        
        #Now open the regions tab of the image window
        #Open the image settings
        ActionChains(driver).double_click( imageWindow ).perform()
        Util.openSettings( self, driver, "Image", True )
        time.sleep(4)
        
         #Open the regions tab
        Util.clickTab( driver, "Regions" )
        time.sleep( timeout )
        
        #Change the width and the height of the region to 10.
        minorRadiusText = driver.find_element_by_xpath("//div[@id='EllipseRegionMinorRadius']/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", minorRadiusText )
        Util._changeElementText(self, driver, minorRadiusText, 10)
        majorRadiusText = driver.find_element_by_xpath( "//div[@id='EllipseRegionMajorRadius']/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", majorRadiusText )
        Util._changeElementText(self, driver, majorRadiusText, 10)
        time.sleep( timeout )
        
        #Check that the sum has updated to a new correct value
        self._checkRegionSumValue( driver, 32.6659)
     
    # Test that we can display the correct statistics for a polygonal region.  Then test that the
    # statistics will update correctly if we change the size of the polygon.
    def test_polygonRegion(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "Orion.methanol.cbc.contsub.image.fits")
        time.sleep( timeout )
        
        #Load a polygonal region
        Util.load_image(self, driver, "OrionMethanolRegionPolygon.crtf")
        time.sleep( timeout )

        # Replace the histogram window with the statistics window.
        self.show_statistics_window(driver)
        
        #Test that we see the region text field labelled "Sum" and that its value is correct.
        self._checkRegionSumValue( driver, 13.8497 )
        
        #Now open the regions tab of the image window
        #Open the image settings
        ActionChains(driver).double_click( imageWindow ).perform()
        Util.openSettings( self, driver, "Image", True )
        time.sleep(4)
        
         #Open the regions tab
        Util.clickTab( driver, "Regions" )
        time.sleep( timeout )
        
        #Change the width and the height of the region to 10.
        heightText = driver.find_element_by_xpath("//div[@id='RectangleRegionHeight']/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", heightText )
        Util._changeElementText(self, driver, heightText, 10)
        widthText = driver.find_element_by_xpath( "//div[@id='RectangleRegionWidth']/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", widthText )
        Util._changeElementText(self, driver, widthText, 10)
        time.sleep( timeout )
        
        #Check that the sum has updated to a new correct value
        self._checkRegionSumValue( driver, 2.19114)
        
    # Test that we can display the correct statistics for a polygonal region in DS9 format.  Then test that the
    # statistics will update correctly if we change the size of the polygon.
    def test_polygonRegion(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Load a specific image.
        imageWindow = Util.load_image(self, driver, "Orion.methanol.cbc.contsub.image.fits")
        time.sleep( timeout )
        
        #Load a polygonal region
        Util.load_image(self, driver, "OrionMethanolRegionPolygon.reg")
        time.sleep( timeout )

        # Replace the histogram window with the statistics window.
        self.show_statistics_window(driver)
        
        #Test that we see the region text field labelled "Sum" and that its value is correct.
        self._checkRegionSumValue( driver, -28.4015 )
        
        #Now open the regions tab of the image window
        #Open the image settings
        ActionChains(driver).double_click( imageWindow ).perform()
        Util.openSettings( self, driver, "Image", True )
        time.sleep(4)
        
         #Open the regions tab
        Util.clickTab( driver, "Regions" )
        time.sleep( timeout )
        
        #Change the width and the height of the region to 10.
        heightText = driver.find_element_by_xpath("//div[@id='RectangleRegionHeight']/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", heightText )
        Util._changeElementText(self, driver, heightText, 10)
        widthText = driver.find_element_by_xpath( "//div[@id='RectangleRegionWidth']/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", widthText )
        Util._changeElementText(self, driver, widthText, 10)
        time.sleep( timeout )
        
        #Check that the sum has updated to a new correct value
        self._checkRegionSumValue( driver, -.837169)
    
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

