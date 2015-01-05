import unittest
from parseTestIds import Parser
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
        
        idParser = Parser()
        idMap = idParser.parseFile( "../widgets/TestID.js");
        
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
    
            # Click on the show popup menu
            popupButton = driver.find_element_by_id( idMap["SHOW_POPUP_BUTTON"])
            self.assertIsNotNone( popupButton, "Could not find button to show popup")
            ActionChains( driver).click( popupButton).perform()
    
            # Click the histogram button
            histogramButton = driver.find_element_by_id( idMap["HISTOGRAM_BUTTON"])
            self.assertIsNotNone( histogramButton, "Could not find histogram button")
            ActionChains(driver).click(histogramButton).perform()
    
            # We should now see a histogram popup window
            histWindow = driver.find_element_by_xpath( "//div[@qxclass='skel.widgets.Window.DisplayWindowHistogram']")
            self.assertIsNotNone( histWindow, "Could not popup a histogram")
            
        # Make sure the bin count is visible.
        binCountCheck = driver.find_element_by_id( idMap["HISTOGRAM_BIN_COUNT_CHECK"] )
        self.assertIsNotNone( binCountCheck, "Could not find bin count check")
        binSelected = binCountCheck.is_selected()
        print 'Bin selected', binSelected
        if not binSelected:
            print 'clicking bin count'
            binCountCheck.click()
        
        # Look for the binCountText field.
        #binCountText = driver.find_element_by_xpath( "//div[@id=histogramBinCountTextField]//input" )
        binCountText = driver.find_element_by_id( idMap["HISTOGRAM_BIN_COUNT_INPUT"] )
        self.assertIsNotNone( binCountText, "Could not find bin count text field")
        # Scroll the histogram window so the bin count at the bottom is visible.
        driver.execute_script( "arguments[0].scrollIntoView(true);", binCountText)
        textValue = binCountText.get_attribute("value")
        print "value of text field=", textValue
        textScrollPercent = (50 - int(float(textValue))) / 100.0
        print "scrollPercent=",textScrollPercent
       
        
        # Look for the bin count slider.
        print 'key=', idMap["HISTOGRAM_BIN_COUNT_SLIDER"]
        binCountSlider = driver.find_element_by_id( idMap["HISTOGRAM_BIN_COUNT_SLIDER"] )
        self.assertIsNotNone( binCountSlider, "Could not find bin count slider");
        sliderScroll = binCountSlider.find_element_by_xpath( ".//div")
        self.assertIsNotNone( sliderScroll, "Could not find bin count slider scroll")
        
        #Scroll the slider to the middle
        
        scrollSize = sliderScroll.size
        print 'Scroll width=', scrollSize['width']
        sliderSize = binCountSlider.size
        sliderScrollAmount = sliderSize['width'] * textScrollPercent
        print 'Slider scroll=',sliderScrollAmount
        #Subtract half the width of the slider scroll.
        sliderScrollAmount = sliderScrollAmount - scrollSize['width'] / 2
        print 'Slider scroll adjusted=',sliderScrollAmount
        ActionChains( driver ).drag_and_drop_by_offset( sliderScroll, sliderScrollAmount, 0 ).perform()
        
        # Check that the value goes to the server and gets set in the text field.
        newText = binCountText.get_attribute( "value")
        print 'Text=',newText
        self.assertAlmostEqual( int(float(newText)), 50,None,"Failed to scroll halfway",3)
        
        driver.close()
        
        
    def tearDown(self):
        print 'End tBinCount'
        #try :
            #self.driver.close()
        #except e:
        #    print 'Error closing driver'

if __name__ == "__main__":
    unittest.main()        
        
