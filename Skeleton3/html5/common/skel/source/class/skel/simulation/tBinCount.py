import unittest
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

#WHAT WE should be testing is if we set the value in the slider, then
#it sends a command to the server and sets the same value in the text field.
class tBinCount(unittest.TestCase):
    def setUp(self):
        print 'tBinCount'
        
    def test_binCountChange(self):   
        driver = webdriver.Firefox()
        driver.get("http://localhost:8080/pureweb/app?client=html5&name=CartaSkeleton3")
        driver.implicitly_wait(10)
        
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
        
        ActionChains( driver).click( histWindow ).perform()
        
        # Click the settings button to expose the settings.
        settingsButton = driver.find_element_by_xpath( "//div[starts-with(@id,'Histogram')]//div[starts-with(@id,'SettingsButton')]")
        self.assertIsNotNone( settingsButton, "Could not find histogram settings button")
        ActionChains( driver).click( settingsButton).perform()
        
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
        self.assertAlmostEqual( int(float(newText)), 500 ,None,"Failed to scroll halfway",25)
        
        driver.close()
        
        
    def tearDown(self):
        print 'End tBinCount'
        #try :
            #self.driver.close()
        #except e:
        #    print 'Error closing driver'

if __name__ == "__main__":
    unittest.main()        
        
