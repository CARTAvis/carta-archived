import unittest
import selectBrowser
import Util
import time
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

#Test that we can use commands switch between different layouts.
class tLayout(unittest.TestCase):
    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp(self, browser)
    
    def _clickLayoutButton(self, driver):
        # Getting element not found in cache without this
        time.sleep(4)
        # Find the layout button on the menu bar and click it.
        layoutButton = driver.find_element_by_xpath("//div[text()='Layout']/..")
        self.assertIsNotNone( layoutButton, "Could not find layout button on the menu bar")
        ActionChains(driver).click(layoutButton).perform()
        
    # Test that we can switch to image layout using the 'Layout' menu button.
    def test_layout_image(self):    
        driver = self.driver
        time.sleep(5)

        # Getting element not found in cache without this.
        driver.implicitly_wait(20)
        
        # Find the layout button on the menu bar and click it.
        self._clickLayoutButton( driver )
        
        # Find the layout image button in the submenu and click it.
        imageLayoutButton = driver.find_element_by_xpath( "//div[text()='Image Layout']/..")
        self.assertIsNotNone( imageLayoutButton, "Could not find layout image button in submenu")
        ActionChains(driver).click( imageLayoutButton).perform()
        time.sleep(5)

        # Check that there is an Image Window
        imageWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        self.assertIsNotNone( imageWindow, "Could not find an image window")
        
        # Check that there are no other Windows
        windowCount = Util.get_window_count(self, driver)
        self.assertEqual( windowCount, 1, "Image Layout should have only one window")
        
    # Test that we can switch to image layout using the 'Layout' menu button.
    def test_layout_analysis(self):    
        driver = self.driver        
        time.sleep(5)

        # Getting element not found in cache without this.
        driver.implicitly_wait(20)

        # Find the layout button on the menu bar and click it.
        self._clickLayoutButton( driver )
        
        # Find the layout analysis button in the submenu and click it.
        analysisLayoutButton = driver.find_element_by_xpath( "//div[text()='Analysis Layout']")
        self.assertIsNotNone( analysisLayoutButton, "Could not find layout analysis button in submenu")
        ActionChains(driver).click( analysisLayoutButton).perform()
        
        # Check that there is an Image Window
        imageWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        self.assertIsNotNone( imageWindow, "Could not find an image window")
        
        # Check that there is a Histogram Window
        histogramWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowHistogram']")
        self.assertIsNotNone( histogramWindow, "Could not find aCha histogram window")

        # Check that there is a Colormap Window
        colormapWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowColormap']")
        self.assertIsNotNone( colormapWindow, "Could not find a colormap window")

        # Check that there is an AnimatorWindow
        animatorWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")
        self.assertIsNotNone( animatorWindow, "Could not find an animator window")
        
        # Check that there are the correct number of Windows
        windowCount = Util.get_window_count(self, driver)
        print windowCount
        self.assertEqual( windowCount, 4, "Image Layout does not have the correct number of window")
        
    
    # Test that we can set a custom layout with 5 rows and 3 columns
    def test_layout_custom(self):
        driver = self.driver
        time.sleep(5)

        # Getting element not found in cache without this.
        driver.implicitly_wait(20)
        
        Util.layout_custom(self, driver, 5, 3 )
        
       
        # Check that there are the correct number of Windows
        windowCount = Util.get_window_count(self, driver)
        print windowCount
        self.assertEqual( windowCount, 15, "Image Layout does not have the correct number of Windows")
    
    def tearDown(self):
        # Close the browser
        self.driver.close()
        # Allow browser to fully close before continuing
        time.sleep(2)
        # Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()        
        
