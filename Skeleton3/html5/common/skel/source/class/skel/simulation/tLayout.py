import unittest
import Util
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

#Test that we can use commands switch between different layouts.
class tLayout(unittest.TestCase):
    def setUp(self):
        self.driver = webdriver.Firefox()
        self.driver.get("http://localhost:8080/pureweb/app?client=html5&name=CartaSkeleton3&username=dan12&password=Cameron21")
        self.driver.implicitly_wait(10)
    
    
    def _clickLayoutButton(self, driver):
        # Find the layout button on the menu bar and click it.
        layoutButton = driver.find_element_by_xpath("//div[text()='Layout']/..")
        self.assertIsNotNone( layoutButton, "Could not find div with text layout")
        ActionChains(driver).click(layoutButton).perform()
        
        
    #Test that we can switch to image layout using the 'Layout' menu button.
    def test_layout_image(self):    
        driver = self.driver
        #Getting element not found in cache without this.
        driver.implicitly_wait(10)
        
        # Find the layout button on the menu bar and click it.
        self._clickLayoutButton( driver )
        
        # Find the layout image button in the submenu and click it.
        imageLayoutButton = driver.find_element_by_xpath( "//div[text()='Image Layout']/..")
        self.assertIsNotNone( imageLayoutButton, "Could not find layout image button in submenu")
        ActionChains(driver).click( imageLayoutButton).perform()
        
        #Check that there is an Image Window
        imageWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        self.assertIsNotNone( imageWindow, "Could not find an image window")
        
        #Check that there are no other Windows
        windowCount = Util.get_window_count( self, driver )
        self.assertEqual( windowCount, 1, "Image Layout should have only one window")
        
        
    #Test that we can switch to image layout using the 'Layout' menu button.
    def test_layout_analysis(self):    
        driver = self.driver
        #Getting element not found in cache without this.
        driver.implicitly_wait(10)
        
        # Find the layout button on the menu bar and click it.
        self._clickLayoutButton( driver )
        
        # Find the layout image button in the submenu and click it.
        analysisLayoutButton = driver.find_element_by_xpath( "//div[text()='Analysis Layout']/..")
        self.assertIsNotNone( analysisLayoutButton, "Could not find layout analysis button in submenu")
        ActionChains(driver).click( analysisLayoutButton).perform()
        
        #Check that there is an Image Window
        imageWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        self.assertIsNotNone( imageWindow, "Could not find an image window")
        
        #Check that there is a Statistics Window
        statisticsWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowStatistics']")
        self.assertIsNotNone( statisticsWindow, "Could not find a statistics window")
        
        #Check that there is a Histogram Window
        histogramWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowHistogram']")
        self.assertIsNotNone( histogramWindow, "Could not find a histogram window")
        
        #Check that there is a Colormap Window
        colormapWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowColormap']")
        self.assertIsNotNone( colormapWindow, "Could not find a colormap window")
        
        #Check that there is an AnimatorWindow
        animatorWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")
        self.assertIsNotNone( animatorWindow, "Could not find an animator window")
        
        #Check that there are the correct number of Windows
        windowCount = Util.get_window_count( self, driver )
        print windowCount
        self.assertEqual( windowCount, 5, "Image Layout does not have the correct number of window")
        
    #Test set a custom layout with the given number of rows and columns
    def layout_custom(self, driver, rows, cols ):
        
        # Find the layout button on the menu bar and click it.
        self._clickLayoutButton( driver )
        
        # Find the layout custom button in the submenu and click it.
        customLayoutButton = driver.find_element_by_xpath( "//div[text()='Custom Layout']/..")
        self.assertIsNotNone( customLayoutButton, "Could not find custom layout button in submenu")
        ActionChains(driver).click( customLayoutButton).perform()
        
        #Get the row count spin and set its value.
        rowSpin = driver.find_element_by_xpath( "//div[starts-with(@id,'customLayoutRows')]/input")
        self.assertIsNotNone( rowSpin, "Could not find custom layout row indicator")
        rowSpin.send_keys( Keys.BACK_SPACE )
        rowSpin.send_keys(str(rows))
        
        #Get the column count spin and set its value.
        colSpin = driver.find_element_by_xpath( "//div[starts-with(@id,'customLayoutCols')]/input")
        self.assertIsNotNone( colSpin, "Could not find custom layout column indicator")
        colSpin.send_keys(str(cols))
        colSpin.send_keys(Keys.ARROW_LEFT)
        colSpin.send_keys( Keys.BACK_SPACE )
        
        #Hit the ok button
        okButton = driver.find_element_by_xpath( "//div[starts-with(@id,'customLayoutOK')]")
        self.assertIsNotNone( okButton, "Could not find custom layout ok button")
        ActionChains(driver).click(okButton).perform()
            
    
    #Test that we can set a custom layout with 5 rows and 3 columns
    def test_layout_custom35(self):
        driver = self.driver
        #Getting element not found in cache without this.
        driver.implicitly_wait(10)
        
        self.layout_custom( driver, 3, 5 )
        
        #Check that there are the correct number of Windows
        windowCount = Util.get_window_count( self, driver )
        print windowCount
        self.assertEqual( windowCount, 15, "Custom Layout does not have 3 rows and 5 columns")
    
    #Test that we can set a custom layout with 1 rows and 1 columns
    def test_layout_custom11(self):
        driver = self.driver
        #Getting element not found in cache without this.
        driver.implicitly_wait(10)
        
        self.layout_custom( driver, 1, 1 )
        
        #Check that there are the correct number of Windows
        windowCount = Util.get_window_count( self, driver )
        print windowCount
        self.assertEqual( windowCount, 1, "Custom Layout does not have 1 row and 1 column")
        
    #Test that we can set a custom layout with 1 rows and 2 columns
    def test_layout_custom12(self):
        driver = self.driver
        #Getting element not found in cache without this.
        driver.implicitly_wait(10)
        
        self.layout_custom( driver, 1, 2 )
        
        #Check that there are the correct number of Windows
        windowCount = Util.get_window_count( self, driver )
        print windowCount
        self.assertEqual( windowCount, 2, "Custom Layout does not have 1 row and 2 column")
        
    #Test that we can set a custom layout with 1 rows and 2 columns
    def test_layout_custom21(self):
        driver = self.driver
        #Getting element not found in cache without this.
        driver.implicitly_wait(10)
        
        self.layout_custom( driver, 2, 1 )
        
        #Check that there are the correct number of Windows
        windowCount = Util.get_window_count( self, driver )
        print windowCount
        self.assertEqual( windowCount, 2, "Custom Layout does not have 2 row and 1 column")
        
    #Test that we can set a custom layout with 5 rows and 3 columns
    def test_layout_custom53(self):
        driver = self.driver
        #Getting element not found in cache without this.
        driver.implicitly_wait(10)
        
        self.layout_custom( driver, 5, 3 )
        
        #Check that there are the correct number of Windows
        windowCount = Util.get_window_count( self, driver )
        print windowCount
        self.assertEqual( windowCount, 15, "Custom Layout does not have 5 row and 3 column")
    
    def tearDown(self):
        self.driver.close()

if __name__ == "__main__":
    unittest.main()        
        
