import unittest
import time
import selectBrowser
import Util
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

# Tests of histogram functionality
class tHistogram( unittest.TestCase ):

    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp( self, browser )

    def _getTextValue(self, driver, id):
        textField = driver.find_element_by_id(id)
        self.assertIsNotNone( textField, "Could not find text")
        textValue = textField.get_attribute("value")
        return textValue

    def _openHistogramSettings(self, driver, histWindow):
        ActionChains( driver ).context_click( histWindow ).perform()
        ActionChains( driver).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ARROW_DOWN
                 ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
                 Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
        

    # Find the histogram window either as an inline display if it is already present or as a popup
    def _getHistogramWindow(self, driver):
        # First see if there is a histogram window already there
        histWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowHistogram']")

        if histWindow is None:
            print "Making popup histogram"
            #Find a window capable of loading an image 
            imageWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
            if imageWindow is None:
                print "No way to get a histogram window"
                return

            # Show the context menu 
            ActionChains(driver).context_click( imageWindow ).perform()

            # Click the popup button 
            popupButton = driver.find_element_by_xpath("//div[text()='Popup']/..")
            self.assertIsNotNone( popupButton, "Could not click popup button in the context menu")
            ActionChains(driver).click( popupButton ).perform()

            # Look for the histogram button and click it to open the histogram dialog
            histogramButton = driver.find_element_by_xpath("//div/div[text()='Histogram']/..")
            self.assertIsNotNone( histogramButton, "Could not click histogram button on popup subcontext menu")
            ActionChains(driver).click( histogramButton ).perform()

            # We should now see a histogram popup window 
            histWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowHistogram']")
            self.assertIsNotNone( histWindow, "Could not popup a histogram")

        return histWindow

    # Test that if we set the value in the bin count text field, the slider updates 
    # its value accordingly
    def test_binCountChange(self):
        driver = self.driver
        time.sleep(5)

        # Find and select the histogram window 
        histWindow = self._getHistogramWindow( driver )
        ActionChains(driver).click( histWindow )

        # Click the settings button to expose the settings
        self._openHistogramSettings( driver, histWindow)
        
        # Select the display tab
        displayTab = driver.find_element_by_xpath( "//div[@qxclass='qx.ui.tabview.TabButton']/div[contains(text(),'Display')]/..");
        self.assertIsNotNone( displayTab, "Could not find histogram display tab" );
        driver.execute_script( "arguments[0].scrollIntoView(true);", displayTab)
        ActionChains( driver ).click( displayTab ).perform()

        # Look for the binCountText field.
        binCountText = driver.find_element_by_xpath( "//input[starts-with(@id,'histogramBinCountTextField')]" )
        self.assertIsNotNone( binCountText, "Could not find bin count text field")
        # Scroll the histogram window so the bin count at the bottom is visible.
        driver.execute_script( "arguments[0].scrollIntoView(true);", binCountText)
        textValue = binCountText.get_attribute("value")
        print "value of text field=", textValue
        
        # Calculate percent difference from center.  Note this will fail if the upper
        # bound of the slider changes.
        textScrollPercent = (5000 - int(float(textValue))) / 10000.0
        print "scrollPercent=",textScrollPercent
       
        # Look for the bin count slider.
        binCountSlider = driver.find_element_by_xpath( "//div[starts-with(@id, 'histogramBinCountSlider')]" )
        self.assertIsNotNone( binCountSlider, "Could not find bin count slider")
        # Width of the slider
        sliderSize = binCountSlider.size
        # Amount we need to move in order to get to the center
        sliderScrollAmount = sliderSize['width'] * textScrollPercent
        print 'Slider scroll=',sliderScrollAmount
        
        # Look for the scroll bar in the slider and get its size
        sliderScroll = binCountSlider.find_element_by_xpath( ".//div")
        self.assertIsNotNone( sliderScroll, "Could not find bin count slider scroll")
        scrollSize = sliderScroll.size
        print 'Scroll width=', scrollSize['width']
        
        # Subtract half the width of the slider scroll.
        sliderScrollAmount = sliderScrollAmount - scrollSize['width'] / 2
        print 'Slider scroll adjusted=',sliderScrollAmount
        ActionChains( driver ).drag_and_drop_by_offset( sliderScroll, sliderScrollAmount, 0 ).perform()
        time.sleep(1)

        # Check that the value goes to the server and gets set in the text field.
        newText = binCountText.get_attribute( "value")
        print 'Text=',newText
        self.assertAlmostEqual( int(float(newText)), 5000 ,None,"Failed to scroll halfway",250)

    # Test that the Histogram min and max zoom value 
    def test_zoom(self):
        driver = self.driver
        time.sleep(5)
        
        # Load an image
        Util.load_image(self, driver, "Default")
        
        #Find and select the histogram window
        histWindow = self._getHistogramWindow( driver )
        ActionChains( driver).click( histWindow ).perform()
        
        # Click the settings button to expose the settings.
        self._openHistogramSettings( driver, histWindow )
        
        # Look for the min and max zoom values and store their values.
        minZoomValue = self._getTextValue( driver, "histogramZoomMinValue")
        print "Min zoom=", minZoomValue
        maxZoomValue = self._getTextValue( driver, "histogramZoomMaxValue")
        print "Max zoom=", maxZoomValue
        
        # Find the min and max zoom percentages.  Decrease their values.
        minPercentText = driver.find_element_by_id( "histogramZoomMinPercent")
        self.assertIsNotNone( minPercentText, "Could not find zoom min percent text field")
        minZoomPercent = minPercentText.get_attribute( "value")
        maxPercentText = driver.find_element_by_id( "histogramZoomMaxPercent")
        self.assertIsNotNone( maxPercentText, "Could not find zoom max percent text field")
        maxZoomPercent = maxPercentText.get_attribute( "value")
        driver.execute_script( "arguments[0].scrollIntoView(true);", maxPercentText)
        incrementAmount = 40;
        newMinZoomPercent = Util._changeElementText(self, driver, minPercentText, int(minZoomPercent) + incrementAmount)
        newMaxZoomPercent = Util._changeElementText(self, driver, maxPercentText, int(maxZoomPercent) - incrementAmount)
        time.sleep(4)

        # Get the new min and max zoom values.
        newMinZoomValue = self._getTextValue( driver, "histogramZoomMinValue")
        newMaxZoomValue = self._getTextValue( driver, "histogramZoomMaxValue")
        
        # Check that the new min is larger than the old min
        print "oldMin=", minZoomValue," newMin=", newMinZoomValue
        self.assertGreater( float(newMinZoomValue), float(minZoomValue), "Min did not increase")
        
        # Check that the new max is smaller than the old max
        print "oldMax=", maxZoomValue, " newMax=",newMaxZoomValue
        self.assertGreater( float(maxZoomValue), float(newMaxZoomValue), "Max did not decrease")

    # Test that histogram values will update when an additional image is loaded 
    # in the image window. We then remove the image and check that the initial
    # data in the histogram is restored
    def test_histogramAddImage(self):
        driver = self.driver 
        time.sleep(5)

        # Load an image
        Util.load_image(self, driver, "Default")

        # Find and select the histogram
        histWindow = self._getHistogramWindow(driver)
        ActionChains(driver).click( histWindow ).perform()

        # Click the settings button to expose the settings
        self._openHistogramSettings(driver, histWindow )
        time.sleep(2)

        # Get the max zoom value of the first image
        maxZoomValue = self._getTextValue( driver, "histogramZoomMaxValue")
        print "First image maxZoomValue:", maxZoomValue

        # Load a different image in the same window 
        Util.load_image(self, driver, "aH.fits")
        time.sleep(2)

        # Check that the new max zoom value updates 
        newMaxZoomValue = self._getTextValue( driver, "histogramZoomMaxValue")
        self.assertNotEqual(float(newMaxZoomValue), float(maxZoomValue), "The histogram did not update when a new image was loaded.")
        print "Second image maxZoomValue:", newMaxZoomValue

        # Remove the second image
        imageWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        ActionChains(driver).context_click(imageWindow).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        # Allow the second image to be removed
        time.sleep(2)

        # Get the new max zoom value 
        # Check that the max zoom value is restored to the first image data values
        newZoomValue = self._getTextValue( driver, "histogramZoomMaxValue")
        self.assertEqual( float(maxZoomValue), float(newZoomValue), "Histogram data values should be restored.")
        print "After the second image is removed maxZoomValue:", newZoomValue 

    # Test that the removal of an image will restore the Histogram to default values
    def test_histogramRemoveImage(self):
        driver = self.driver
        time.sleep(5)

        # Load an image
        Util.load_image( self, driver, "Default")

        # Click on the Data->Close->Image button to close the image
        imageWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        ActionChains(driver).context_click( imageWindow ).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()

        # Allow image to fully close
        time.sleep(2)

        # Find and select the Histogram window
        histWindow = self._getHistogramWindow( driver )
        ActionChains(driver).click( histWindow )

        # Click the settings button to expose the settings
        self._openHistogramSettings( driver, histWindow )
        time.sleep(2)

        # Check that the histogram values are restored to default values
        newMaxZoomValue = self._getTextValue( driver, "histogramZoomMaxValue")
        self.assertEqual( float(newMaxZoomValue), 1, "Default values were not restored after image removal")

    # Test that the histogram updates its values when the image is changed in the image window. 
    def test_histogramChangeImage(self):
        driver = self.driver 
        time.sleep(5)

        # Load two images in the same image window
        Util.load_image( self, driver, "Default")
        Util.load_image( self, driver, "aH.fits")

        # Find and select the histogram
        histWindow = self._getHistogramWindow( driver )
        ActionChains(driver).click( histWindow ).perform()

        # Click the settings button to expose the settings
        self._openHistogramSettings( driver, histWindow )
        time.sleep(2)

        # Record the Histogram max zoom value of the second image 
        secondMaxZoomValue = self._getTextValue( driver, "histogramZoomMaxValue" )
        print "Second image maxZoomValue:", secondMaxZoomValue

        # Find and click on the animation window 
        animWindow = driver.find_element_by_xpath( "//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")
        self.assertIsNotNone( animWindow, "Could not find animation window")
        ActionChains(driver).click( animWindow ).perform()   

        # Make sure the animation window is enabled by clicking an element within the window
        # From the context menu, uncheck the Channel Animator and check the Image Animator
        channelText = driver.find_element_by_id( "ChannelIndexText")
        ActionChains(driver).click( channelText ).perform()
        ActionChains(driver).context_click( channelText ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys( 
            Keys.ARROW_DOWN ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT
                ).send_keys(Keys.SPACE).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
        time.sleep(2)

        # Find the first value button and click the button
        firstValueButton = driver.find_element_by_xpath( "//div[@class='qx-toolbar']/div[@qxclass='qx.ui.toolbar.Button'][1]")
        self.assertIsNotNone( firstValueButton, "Could not find button to go to the first valid value")
        driver.execute_script( "arguments[0].scrollIntoView(true);", firstValueButton)
        ActionChains(driver).click( firstValueButton ).perform()
        time.sleep(2)

        # Record the Histogram max zoom value of the first image
        firstMaxZoomValue = self._getTextValue( driver, "histogramZoomMaxValue" )
        print "First image maxZoomValue:", firstMaxZoomValue

        # Check that the Histogram updates its values
        self.assertNotEqual( float(firstMaxZoomValue), float(secondMaxZoomValue), "Histogram did not update when the image was switched in the image window.")

    # Test that the histogram is only able to link to one image. We first load
    # an image in the image window. This image should be linked to the histogram window.
    # We then open a different image in a separate window and try to link the 
    # histogram to the second image. This should fail, and the histogram values should not change. 
    def test_histogramLinking(self):
        driver = self.driver 
        time.sleep(5)

        # Load an image
        Util.load_image( self, driver, "Default")

        # Load the second image in a separate window
        imageWindow2 = Util.load_image_different_window( self, driver, "aH.fits")

        # Find and select the histogram
        histWindow = self._getHistogramWindow( driver )
        ActionChains(driver).click( histWindow ).perform()

        # Click the settings button to expose the settings
        self._openHistogramSettings( driver, histWindow )
        time.sleep(2)

        # Record the max zoom value of the first image
        maxZoomValue = self._getTextValue( driver, "histogramZoomMaxValue" )
        print "First image maxZoomValue:", maxZoomValue

        # Open link settings for the histogram
        ActionChains(driver).context_click( histWindow ).send_keys( Keys.ARROW_DOWN ).send_keys(
            Keys.ARROW_DOWN).send_keys( Keys.ENTER ).perform()

        # Change the link location of the animator to the second image
        ActionChains(driver).move_to_element( histWindow ).click( histWindow ).drag_and_drop(
            histWindow, imageWindow2).perform()

        # Wait for the actions to be completely performed before continuing
        time.sleep(2)

        # Get the new max zoom value
        # Check that it did not change
        newMaxZoomValue = self._getTextValue( driver, "histogramZoomMaxValue")
        print "New maxZoomValue:", newMaxZoomValue
        self.assertEqual( float( maxZoomValue ), float( newMaxZoomValue), "Histogram should not link to second image.")

    # Test removal of a link from the Histogram.
    #  Note:  This test is disabled because drag_and_drop does not remove the link.
    def stest_histogramLinkRemoval(self):
        driver = self.driver
        time.sleep(5)

        # Find the Histogram window
        histWindow = self._getHistogramWindow( driver )

        # Open Link settings for the Histogram window
        ActionChains(driver).context_click( histWindow ).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        # Remove link from the main image window from the Histogram
        imageWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        ActionChains(driver).move_to_element( imageWindow ).context_click( imageWindow ).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        # Exit Links before continuing
        ActionChains(driver).send_keys( Keys.ESCAPE).perform();

        # Load an image
        Util.load_image( self, driver, "Default")

        # Find and select the histogram window
        histWindow = self._getHistogramWindow( driver )
        ActionChains(driver).click( histWindow ).perform()

        # Click the settings button to expose the settings
        self._openHistogramSettings( driver, histWindow )
        time.sleep(2)

        # Check that the histogram values are default values
        newMaxZoomValue = self._getTextValue( driver, "histogramZoomMaxValue")
        self.assertEqual( float(newMaxZoomValue), 1, "Histogram is linked to image after link was removed")

    # Test that we can change the linked image to the Histogram
    # Note:  This test is disabled because drag_and_drop does not remove the link
    def stest_histogramChangeLinks(self):
        driver = self.driver 
        time.sleep(5)

        # Load an image in a separate window
        imageWindow2 = Util.load_image_different_window( self, driver, "aH.fits")

        # Find and select the Histogram window
        histWindow = self._getHistogramWindow( driver )
        ActionChains(driver).click( histWindow ).perform()

        # Click the settings button to expose the settings
        self._openHistogramSettings( driver )
        time.sleep(2)

        # Open Link settings for the Histogram window
        ActionChains(driver).context_click( histWindow ).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        # Remove the link from the Histogram to the main image window
        imageWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        ActionChains(driver).move_to_element( imageWindow ).context_click( imageWindow ).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        # Exit links before continuing
        ActionChains(driver).move_to_element( imageWindow ).context_click( imageWindow ).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        # Open link settings for the Histogram 
        ActionChains(driver).context_click( histWindow ).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        # Link the Histogram to the second image
        ActionChains(driver).move_to_element( histWindow ).click( histWindow ).drag_and_drop(
            histWindow, imageWindow2).perform()
        time.sleep(2)

        # Exit links before continuing
        ActionChains(driver).move_to_element( imageWindow ).context_click( imageWindow ).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        # Check that the histogram values are not default values
        newMaxZoomValue = self._getTextValue( driver, "histogramZoomMaxValue")
        self.assertNotEqual( float(newMaxZoomValue), 1, "Histogram did not update to newly linked image")

    def tearDown(self):
        # Close the browser
        self.driver.close()
        # Allow browser to fully close before continuing
        time.sleep(2)
        # Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()      
        
