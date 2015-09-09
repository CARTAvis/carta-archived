import unittest
import selectBrowser
import Util
import time
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.by import By

#Test that we can use commands switch between different layouts.
class tLayout(unittest.TestCase):
    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp(self, browser)
    
    def _clickLayoutButton(self, driver):
        # Find the layout button on the menu bar and click it.
        layoutButton = WebDriverWait(driver, 20).until(EC.element_to_be_clickable((By.XPATH, "//div[text()='Layout']/..")))
        ActionChains(driver).click( layoutButton ).perform()
        
    # Test that we can switch to image layout using the 'Layout' menu button.
    def test_layout_image(self):    
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 30).until(EC.visibility_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Find the layout button on the menu bar and click it.
        self._clickLayoutButton( driver )
        
        # Find the layout image button in the submenu and click it.
        imageLayoutButton = driver.find_element_by_xpath( "//div[text()='Image Layout']/..")
        #imageLayoutButton = WebDriverWait(driver, 10).until(EC.element_to_be_clickable((By.XPATH, "//div[text()='Image Layout']/..")))
        ActionChains(driver).click( imageLayoutButton).perform()
        time.sleep( timeout )

        # Check that there is an Image Window
        imageWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
        
        # Check that there are no other Windows
        windowCount = Util.get_window_count(self, driver)
        self.assertEqual( windowCount, 1, "Image Layout should have only one window")
        
    # Test that we can switch to image layout using the 'Layout' menu button.
    def test_layout_analysis(self):    
        driver = self.driver        

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 30).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Find the layout button on the menu bar and click it.
        self._clickLayoutButton( driver )
        
        # Find the layout analysis button in the submenu and click it.
        analysisLayoutButton = driver.find_element_by_xpath( "//div[text()='Analysis Layout']")
        #analysisLayoutButton = WebDriverWait(driver, 10).until(EC.element_to_be_clickable((By.XPATH, "//div[text()='Analysis Layout']")))
        ActionChains(driver).click( analysisLayoutButton).perform()
        
        # Check that there is an Image Window
        imageWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
        
        # Check that there is a Histogram Window
        histogramWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowHistogram']")))

        # Check that there is a Colormap Window
        colormapWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowColormap']")))

        # Check that there is an AnimatorWindow
        animatorWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")))
        
        # Check that there are the correct number of Windows
        desktopList = driver.find_elements_by_xpath("//div[@qxclass='qx.ui.window.Desktop']")
        windowCount = len( desktopList )
        print windowCount
        self.assertEqual( windowCount, 4, "Image Layout does not have the correct number of window")
        
    
    # Test that we can set a custom layout with 5 rows and 3 columns
    def test_layout_custom(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()
        
        Util.layout_custom(self, driver, 5, 3 )
        time.sleep( timeout )
        
        # Check that there are the correct number of Windows
        windowCount = Util.get_window_count(self, driver)
        print windowCount
        time.sleep( timeout )
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
        
