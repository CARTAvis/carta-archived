import unittest
import Util
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

#Tests of histogram functionality
class tHistogram(unittest.TestCase):
    def setUp(self):
        self.driver = webdriver.Firefox()
        self.driver.get("http://localhost:8080/pureweb/app?client=html5&name=CartaSkeleton3")
        self.driver.implicitly_wait(10)
        
    def _getTextValue(self, driver, id ):
        textField = driver.find_element_by_id( id)
        self.assertIsNotNone( textField, "Could not find text")
        textValue = textField.get_attribute( "value")
        return textValue
        
    def _openHistogramSettings(self, driver, histWindow):
        ActionChains( driver ).context_click( histWindow ).perform()
        ActionChains( driver).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ARROW_DOWN
                 ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
                 Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ARROW_UP
                 ).send_keys(Keys.ARROW_UP).send_keys(Keys.ARROW_RIGHT
                 ).send_keys(Keys.ARROW_UP).send_keys(Keys.ARROW_UP).send_keys(
                 Keys.ARROW_UP).send_keys(Keys.ARROW_UP).send_keys(Keys.ENTER).perform()
        
        
    #Find the histogram window either as an inline display if it is already present or as a popup
    def _getHistogramWindow(self, driver) :
        # First see if there is a histogram window already there.
        histWindow = driver.find_element_by_xpath( "//div[@qxclass='skel.widgets.Window.DisplayWindowHistogram']")
        if histWindow is None:
            print 'Making popup histogram'
            # Find a window capable of loading an image.
            imageWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
            if imageWindow is None:
                print 'No way to get a histogram window'
                return
            
            # Show the context menu
            ActionChains(driver).context_click(imageWindow).perform()
            
            # Click the popup button
            popupButton = driver.find_element_by_xpath("//div[text()='Popup']/..")
            self.assertIsNotNone( popupButton, "Could not click popup button in the context menu")
            ActionChains(driver).click(popupButton).perform()
            
            # Look for the histogram button and click it to open the histogram dialog.
            histogramButton = driver.find_element_by_xpath("//div/div[text()='Histogram']/..")
            self.assertIsNotNone(histogramButton, "Could not click histogram button on popup subcontext menu.")
            ActionChains(driver).click(histogramButton).perform()
    
            # We should now see a histogram popup window
            histWindow = driver.find_element_by_xpath( "//div[@qxclass='skel.widgets.Window.DisplayWindowHistogram']")
            self.assertIsNotNone( histWindow, "Could not popup a histogram")
        return histWindow
    
    #Test that if we set the value in the bin count text field, the slider updates
    #its value accordingly
    def stest_binCountChange(self):   
        driver = self.driver
        
        #Find and select the histogram window
        histWindow = self._getHistogramWindow( driver )
        ActionChains( driver).click( histWindow ).perform()
        
        # Click the settings button to expose the settings.
        self._openHistogramSettings( driver, histWindow )
        
        # Look for the binCountText field.
        binCountText = driver.find_element_by_xpath( "//input[starts-with(@id,'histogramBinCountTextField')]" )
        self.assertIsNotNone( binCountText, "Could not find bin count text field")
        # Scroll the histogram window so the bin count at the bottom is visible.
        driver.execute_script( "arguments[0].scrollIntoView(true);", binCountText)
        textValue = binCountText.get_attribute("value")
        print "value of text field=", textValue
        
        #Calculate percent difference from center.  Note this will fail if the upper
        #bound of the slider changes.
        textScrollPercent = (500 - int(float(textValue))) / 1000.0
        print "scrollPercent=",textScrollPercent
       
        
        # Look for the bin count slider.
        binCountSlider = driver.find_element_by_xpath( "//div[starts-with(@id, 'histogramBinCountSlider')]" )
        self.assertIsNotNone( binCountSlider, "Could not find bin count slider")
        #Width of the slider
        sliderSize = binCountSlider.size
        #Amount we need to move in order to get to the center
        sliderScrollAmount = sliderSize['width'] * textScrollPercent
        print 'Slider scroll=',sliderScrollAmount
        
        
        #Look for the scroll bar in the slider and get its size
        sliderScroll = binCountSlider.find_element_by_xpath( ".//div")
        self.assertIsNotNone( sliderScroll, "Could not find bin count slider scroll")
        scrollSize = sliderScroll.size
        print 'Scroll width=', scrollSize['width']
        
        #Subtract half the width of the slider scroll.
        sliderScrollAmount = sliderScrollAmount - scrollSize['width'] / 2
        print 'Slider scroll adjusted=',sliderScrollAmount
        ActionChains( driver ).drag_and_drop_by_offset( sliderScroll, sliderScrollAmount, 0 ).perform()
        
        # Check that the value goes to the server and gets set in the text field.
        newText = binCountText.get_attribute( "value")
        print 'Text=',newText
        self.assertAlmostEqual( int(float(newText)), 500 ,None,"Failed to scroll halfway",30)
        
    def test_zoom(self):
        driver = self.driver
        
        # Load an image
        #At some point this test will need to be rewritten to use a
        #test image available where the tests are running.
        origImage = "Orion.methanol.cbc.contsub.image.fits"
        Util.load_image(self, driver, origImage)
        
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
        minPercentText.clear()
        newMinZoomPercent = int(minZoomPercent) + incrementAmount
        minPercentText.send_keys( newMinZoomPercent )
        minPercentText.send_keys( Keys.ENTER )
        maxPercentText.clear()
        newMaxZoomPercent = int(maxZoomPercent) - incrementAmount
        maxPercentText.send_keys( newMaxZoomPercent )
        maxPercentText.send_keys( Keys.ENTER )
        
        # Get the new min and max zoom values.
        newMinZoomValue = self._getTextValue( driver, "histogramZoomMinValue")
        newMaxZoomValue = self._getTextValue( driver, "histogramZoomMaxValue")
        
        # Check that the new min is larger than the old min
        print "oldMin=", minZoomValue," newMin=", newMinZoomValue
        self.assertGreater( float(newMinZoomValue), float(minZoomValue), "Min did not increase")
        
        # Check that the new max is smaller than the old max
        print "oldMax=", maxZoomValue, " newMax=",newMaxZoomValue
        self.assertGreater( float(maxZoomValue), float(newMaxZoomValue), "Max did not decrease")
        
    def tearDown(self):
        self.driver.close()
       

if __name__ == "__main__":
    unittest.main()        
        
