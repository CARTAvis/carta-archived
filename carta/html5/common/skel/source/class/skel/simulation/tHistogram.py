import unittest
import time
import math
import selectBrowser
import Util
from selenium import webdriver
from flaky import flaky
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.by import By

# Tests of histogram functionality
@flaky(max_runs=3)
class tHistogram( unittest.TestCase ):

    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp( self, browser )

    # Open histogram settings by clicking on the Histogram settings checkbox located on the menu bar
    def _openHistogramSettings(self, driver):
        histogramSettingsCheckbox = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.CheckBox']/div[text()='Histogram Settings']")))
        ActionChains(driver).click( histogramSettingsCheckbox ).perform()

    # Find the histogram window either as an inline display if it is already present or as a popup
    def _getHistogramWindow(self, driver):
        # First see if there is a histogram window already there
        histWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowHistogram']")))

        if histWindow is None:
            print "Making popup histogram"
            #Find a window capable of loading an image
            imageWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
            if imageWindow is None:
                print "No way to get a histogram window"
                return

            # Show the context menu
            ActionChains(driver).context_click( imageWindow ).perform()

            # Click the popup button
            popupButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Popup']/..")))
            self.assertIsNotNone( popupButton, "Could not click popup button in the context menu")
            ActionChains(driver).click( popupButton ).perform()

            # Look for the histogram button and click it to open the histogram dialog
            histogramButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div/div[text()='Histogram']/..")))
            self.assertIsNotNone( histogramButton, "Could not click histogram button on popup subcontext menu")
            ActionChains(driver).click( histogramButton ).perform()

            # We should now see a histogram popup window
            histWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowHistogram']")))
            self.assertIsNotNone( histWindow, "Could not popup a histogram")

        return histWindow

    # Test that if we set the value in the bin count text field, the slider updates
    # its value accordingly
    def test_binCountChange(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
        # Load an image
        Util.load_image(self, driver, "Default")

        # Find and select the histogram window
        histWindow = self._getHistogramWindow( driver )
        ActionChains(driver).click( histWindow ).perform()
        time.sleep( timeout )

        # Click the settings button to expose the settings
        self._openHistogramSettings( driver )

        # Navigate to Display tab of the Histogram Settings
        displayTab = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.tabview.TabButton']/div[contains(text(),'Display')]/..")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", displayTab)
        ActionChains(driver).click( displayTab ).perform()

        # Look for the binCountText field.
        binCountText = driver.find_element_by_xpath( "//input[starts-with(@id,'histogramBinCountTextField')]" )
        # Scroll the histogram window so the bin count at the bottom is visible.
        driver.execute_script( "arguments[0].scrollIntoView(true);", binCountText)
        textValue = binCountText.get_attribute("value")
        print "value of text field=", textValue

        #Slider is at the log of the textValue.
        logTextValue = math.log(float(textValue))
        print "log text value=",logTextValue

        # Calculate percent difference from center.  Note this will fail if the upper
        # bound of the slider changes
        #9.21034037198 = ln(10000)
        per = logTextValue / 100;
        print " percent=",per
        textScrollPercent = .5 - per
        print "scrollPercent=",textScrollPercent

        # Look for the bin count slider.
        binCountSlider = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[starts-with(@id, 'histogramBinCountSlider')]")))
        # Width of the slider
        sliderSize = binCountSlider.size
        print 'Slider size=', sliderSize
        # Amount we need to move in order to get to the center
        sliderScrollAmount = sliderSize['width'] * textScrollPercent
        print 'Slider scroll=',sliderScrollAmount

        # Look for the scroll bar in the slider and get its size
        sliderScroll = binCountSlider.find_element_by_xpath( ".//div")
        self.assertIsNotNone( sliderScroll, "Could not find bin count slider scroll")
        scrollSize = sliderScroll.size
        print 'Scroll width=', scrollSize['width']
        scrollSizeWidth = scrollSize['width']
        
        # Subtract half the width of the slider scroll.
        sliderScrollAmount = sliderScrollAmount - scrollSizeWidth / 2
        print 'Slider scroll adjusted=',sliderScrollAmount
        ActionChains( driver ).drag_and_drop_by_offset( sliderScroll, sliderScrollAmount, 0 ).perform()
        time.sleep( timeout )

        # Check that the value goes to the server and gets set in the text field.
        newText = binCountText.get_attribute( "value")
        print 'Text=',newText
        self.assertAlmostEqual( int(float(newText)), 100 ,None,"Failed to scroll halfway",20)


    # Test that the Histogram min and max zoom value
    def test_zoom(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Load an image
        Util.load_image(self, driver, "Default")

        #Find and select the histogram window
        histWindow = self._getHistogramWindow( driver )
        ActionChains( driver).click( histWindow ).perform()
        time.sleep(2)

        # Click the settings button to expose the settings.
        self._openHistogramSettings( driver )

        # Look for the min and max zoom values and store their values.
        minZoomValue = Util._getTextValue( self, driver, "histogramZoomMinValue")
        print "Min zoom=", minZoomValue
        maxZoomValue = Util._getTextValue( self, driver, "histogramZoomMaxValue")
        print "Max zoom=", maxZoomValue

        # Find the min and max zoom percentages.  Decrease their values.
        minPercentText = driver.find_element_by_id("histogramZoomMinPercent")
        driver.execute_script( "arguments[0].scrollIntoView(true);", minPercentText)
        minZoomPercent = minPercentText.get_attribute( "value")
        maxPercentText = driver.find_element_by_id("histogramZoomMaxPercent")
        maxZoomPercent = maxPercentText.get_attribute( "value")
        driver.execute_script( "arguments[0].scrollIntoView(true);", maxPercentText)
        incrementAmount = 40;
        newMinZoomPercent = Util._changeElementText(self, driver, minPercentText, str(float(minZoomPercent) + incrementAmount))
        newMaxZoomPercent = Util._changeElementText(self, driver, maxPercentText, str(float(maxZoomPercent) - incrementAmount))
        time.sleep( timeout )

        # Get the new min and max zoom values.
        newMinZoomValue = Util._getTextValue( self, driver, "histogramZoomMinValue")
        newMaxZoomValue = Util._getTextValue( self, driver, "histogramZoomMaxValue")

        # Check that the new min is larger than the old min
        print "oldMin=", minZoomValue," newMin=", newMinZoomValue
        self.assertGreater( float(newMinZoomValue), float(minZoomValue), "Min did not increase")

        # Check that the new max is smaller than the old max
        print "oldMax=", maxZoomValue, " newMax=",newMaxZoomValue
        self.assertGreater( float(maxZoomValue), float(newMaxZoomValue), "Max did not decrease")


    # Test that histogram values will update when an additional image is loaded
    # in the image window.
    def test_histogramAddImage(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Load an image
        Util.load_image(self, driver, "Default")

        # Find and select the histogram
        histWindow = self._getHistogramWindow(driver)
        ActionChains(driver).click( histWindow ).perform()
        time.sleep(2)

        # Click the settings button to expose the settings
        self._openHistogramSettings( driver )

        # Get the max zoom value of the first image
        maxZoomValue = Util._getTextValue( self, driver, "histogramZoomMaxValue")
        print "First image maxZoomValue:", maxZoomValue

        # Load a different image in the same window
        Util.load_image(self, driver, "aH.fits")
        time.sleep( timeout )

        # Check that the new max zoom value updates
        newMaxZoomValue = Util._getTextValue( self, driver, "histogramZoomMaxValue")
        self.assertNotEqual(float(newMaxZoomValue), float(maxZoomValue), "The histogram did not update when a new image was loaded.")
        print "Second image maxZoomValue:", newMaxZoomValue

    # Test that the removal of an image will restore the Histogram to default values
    def test_histogramRemoveImage(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Load an image
        Util.load_image( self, driver, "Default")
        time.sleep(timeout)

        # Click on the Data->Close->Image button to close the image
        imageWindow = driver.find_element_by_xpath( "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        ActionChains(driver).double_click( imageWindow ).perform()
        dataButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Data']/..")))
        ActionChains(driver).click( dataButton ).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()

        # Find and select the Histogram window
        histWindow = self._getHistogramWindow( driver )
        ActionChains(driver).click( histWindow ).perform()
        time.sleep( timeout )

        # Click the settings button to expose the settings
        self._openHistogramSettings( driver )

        # Check that the histogram values are restored to default values
        newMaxZoomValue = Util._getTextValue( self, driver, "histogramZoomMaxValue")
        print "Zoom max value=", newMaxZoomValue
        self.assertEqual( float(newMaxZoomValue), 1, "Default values were not restored after image removal")

    # Test that the histogram updates its values when the image is changed in the image window.
    def test_histogramChangeImage(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Load two images in the same image window
        Util.load_image( self, driver, "Default")
        Util.load_image( self, driver, "aH.fits")
        time.sleep( timeout )

        # Find and select the histogram
        histWindow = self._getHistogramWindow( driver )
        ActionChains(driver).click( histWindow ).perform()

        # Click the settings button to expose the settings
        self._openHistogramSettings( driver )
        time.sleep( timeout )

        # Record the Histogram max zoom value of the second image
        secondMaxZoomValue = Util._getTextValue( self, driver, "histogramZoomMaxValue" )
        print "Second image maxZoomValue:", secondMaxZoomValue

        # Find and click on the animation window
        animWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")))
        ActionChains(driver).click( animWindow ).perform()

        # Make sure the animation window is enabled by clicking an element within the window
        imageText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ImageIndexText")))
        ActionChains(driver).click( imageText ).perform()

        # Find the first value button and click the button
        firstValueButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ImageTapeDeckFirstValue")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", firstValueButton)
        ActionChains(driver).click( firstValueButton ).perform()
        time.sleep( timeout )

        # Record the Histogram max zoom value of the first image
        firstMaxZoomValue = Util._getTextValue( self, driver, "histogramZoomMaxValue" )
        print "First image maxZoomValue:", firstMaxZoomValue

        # Check that the Histogram updates its values
        self.assertNotEqual( float(firstMaxZoomValue), float(secondMaxZoomValue), "Histogram did not update when the image was switched in the image window.")

    # Test that the histogram is only able to link to one image. We first load
    # an image in the image window. This image should be linked to the histogram window.
    # We then open a different image in a separate window and try to link the
    # histogram to the second image. This should fail, and the histogram values should not change.
    def test_histogramLinking(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Load an image
        Util.load_image( self, driver, "Default")
        time.sleep(2)

        # Load the second image in a separate window
        imageWindow2 = Util.load_image_different_window( self, driver, "aH.fits")

        # Find and select the histogram
        histWindow = self._getHistogramWindow( driver )
        ActionChains(driver).click( histWindow ).perform()

        # Click the settings button to expose the settings
        self._openHistogramSettings( driver )

        # Record the max zoom value of the first image
        maxZoomValue = Util._getTextValue( self, driver, "histogramZoomMaxValue" )
        print "First image maxZoomValue:", maxZoomValue
        time.sleep( timeout )

        # Open link settings for the histogram
        linkMenuButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Links...']")))
        ActionChains(driver).click( linkMenuButton ).perform()
        time.sleep( timeout )

        # Try to add a link from the Histogram to the second image
        # This should fail: no link should be made from the Histogram to the second image
        Util.link_second_image( self, driver, imageWindow2 )

        # Check that the second image is not linked to the Histogram
        # Check that the max zoom value did not change from the linking attempt to the second image
        newMaxZoomValue = Util._getTextValue( self, driver, "histogramZoomMaxValue")
        print "New maxZoomValue:", newMaxZoomValue
        self.assertEqual( float( maxZoomValue ), float( newMaxZoomValue), "Histogram should not link to second image.")

    # Test removal of a link from the Histogram.
    def test_histogramLinkRemoval(self):
        driver = self.driver
        browser = selectBrowser._getBrowser()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Find the Histogram window
        histWindow = self._getHistogramWindow( driver )
        ActionChains(driver).click(histWindow).perform()

        # Open Link settings for the Histogram window
        linkMenuButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Links...']")))
        ActionChains(driver).click( linkMenuButton ).perform()

        # Remove link from the Histogram to the main image window
        Util.remove_main_link( self, driver, imageWindow )

        # Load an image
        Util.load_image( self, driver, "Default")
        time.sleep(2)

        # Find and select the histogram window
        histWindow = self._getHistogramWindow( driver )
        ActionChains(driver).click( histWindow ).perform()

        # Click the settings button to expose the settings
        self._openHistogramSettings( driver )

        # Check that the histogram values are default values
        newMaxZoomValue = Util._getTextValue( self, driver, "histogramZoomMaxValue")
        self.assertEqual( float(newMaxZoomValue), 1, "Histogram is linked to image after link was removed")

    # Test that we can change the linked image to the Histogram
    def test_histogramChangeLinks(self):
        driver = self.driver
        browser = selectBrowser._getBrowser()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Find and select the Histogram window
        histWindow = self._getHistogramWindow( driver )
        ActionChains(driver).click( histWindow ).perform()

        # Click the settings button to expose the settings
        self._openHistogramSettings( driver )

         # Open Link settings for the Histogram window
        linkMenuButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Links...']")))
        ActionChains(driver).click( linkMenuButton ).perform()

        # Remove the link from the Histogram to the main image window
        Util.remove_main_link( self, driver, imageWindow )
        time.sleep(2)

        # Load an image in a separate window
        imageWindow2 = Util.load_image_different_window( self, driver, "aH.fits")
        time.sleep(2)

        # Open link settings for the Histogram
        ActionChains(driver).click( histWindow ).perform()
        linkMenuButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Links...']")))
        ActionChains(driver).click( linkMenuButton ).perform()

        # Link the Histogram to the second image
        Util.link_second_image( self, driver, imageWindow2)

        # Check that the histogram values are not default values
        newMaxZoomValue = Util._getTextValue( self, driver, "histogramZoomMaxValue")
        self.assertNotEqual( float(newMaxZoomValue), 1, "Histogram did not update to newly linked image")
    
    # Test that if we have no region loaded, and switch between the 2D footprint options,
    # image, current region, and selected region there are no problems (same histogram appears).  
    def test_histogramNoRegion(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Load an image
        Util.load_image( self, driver, "Default")
        time.sleep(2)
        
        # Find and select the Histogram window
        histWindow = self._getHistogramWindow( driver )
        ActionChains(driver).click( histWindow ).perform()

        # Click the settings button to expose the settings
        self._openHistogramSettings( driver )

         # Open the Selection settings for the Histogram window
        selectionTab = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.tabview.TabButton']/div[contains(text(),'Selection')]/..")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", selectionTab)
        ActionChains(driver).click( selectionTab ).perform()
        
        #Choose selected region as the foot print.
        regionId = "Histogram2DFootPrintRegion"
        selectRegionRadio = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "Histogram2DFootPrintRegion")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", selectRegionRadio)
        ActionChains(driver).click( selectRegionRadio ).perform()
        time.sleep( timeout )
        selectRegion = Util.isCheckedRadio( self, selectRegionRadio )
        print "Select region=",selectRegion
        self.assertEqual( selectRegion, True, "Histogram not displaying current region")
        
         #Choose all regions as the foot print.
        selectRegionsRadio = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "Histogram2DFootPrintRegions")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", selectRegionsRadio)
        ActionChains(driver).click( selectRegionsRadio ).perform()
        time.sleep( timeout )
        selectRegions = Util.isCheckedRadio( self, selectRegionsRadio )
        print "Select region=",selectRegions
        self.assertEqual( selectRegions, True, "Histogram not displaying all region")
    
    # Test that if we load a region and the change the histogram to selected region, then delete
    # the region there will not be a problem.  
    def test_histogramSelectRegionDelete(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Load an image
        Util.load_image( self, driver, "Default")
        time.sleep(2)
        
        # Load a region
        Util.load_image( self, driver, "OrionMethanolRegion.crtf")
        time.sleep(2)
        
        # Find and select the Histogram window
        histWindow = self._getHistogramWindow( driver )
        ActionChains(driver).click( histWindow ).perform()

        # Click the settings button to expose the settings
        self._openHistogramSettings( driver )

         # Open the Selection settings for the Histogram window
        selectionTab = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.tabview.TabButton']/div[contains(text(),'Selection')]/..")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", selectionTab)
        ActionChains(driver).click( selectionTab ).perform()
        
        #Choose selected region as the foot print.
        regionId = "Histogram2DFootPrintRegion"
        selectRegionRadio = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "Histogram2DFootPrintRegion")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", selectRegionRadio)
        ActionChains(driver).click( selectRegionRadio ).perform()
        time.sleep( timeout )
        selectRegion = Util.isCheckedRadio( self, selectRegionRadio )
        print "Select region=",selectRegion
        self.assertEqual( selectRegion, True, "Histogram not displaying current region")
        
        #Delete the region
        ActionChains(driver).double_click( imageWindow ).perform()
        dataButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Data']/..")))
        ActionChains(driver).click( dataButton ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
        time.sleep( timeout ) 

    def tearDown(self):
        # Close the browser
        self.driver.close()
        # Allow browser to fully close before continuing
        time.sleep(2)
        # Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()

