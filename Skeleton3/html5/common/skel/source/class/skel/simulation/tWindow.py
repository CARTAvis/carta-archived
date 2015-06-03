import unittest
import Util
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

#Test window manipulation.
class tWindow(unittest.TestCase):
    
    def setUp(self):
        self.driver = webdriver.Firefox()
        self.driver.get("http://localhost:8080/pureweb/app?client=html5&name=CartaSkeleton3&username=dan12&password=Cameron21")
        self.driver.implicitly_wait(10)
        
    #Test that a window can be minimized and then restored to its original position
    def test_minimize_restore(self):    
        driver = self.driver
        
        # Find a window capable of loading an image.
        imageWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        self.assertIsNotNone( imageWindow, "Could not find image display window")
               
        # Select the window
        ActionChains(driver).click(imageWindow).perform()
        
        # Click the Window button
        windowButton = driver.find_element_by_xpath("//div[text()='Window']/..")
        self.assertIsNotNone( windowButton, "Could not click window button in the context menu")
        ActionChains(driver).click(windowButton).perform()
        
        # Look for the minimize button in the submenu.
        minimizeButton = driver.find_element_by_xpath("//div/div[text()='Minimize']/..")
        self.assertIsNotNone(minimizeButton, "Could not click minimize button on window subcontext menu.")
        ActionChains(driver).click(minimizeButton).perform()
        
        # Verify that there is a Restore button on the status bar and no DisplayWindowImage.
        restoreButton = driver.find_element_by_xpath( "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[contains(text(), 'Restore: CasaImageLoader')]/..")
        self.assertIsNotNone( restoreButton, "Could not find a restore button on the status bar.")
        
        # Restore the window.  Verify the restore button is gone from the status bar and there is a DisplayWindowImage
        ActionChains(driver).click( restoreButton).perform()
        #Check that the clipping menu item is no longer available
        try: 
            restoreLabel = driver.find_element_by_xpath( "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[contains(text(), 'Restore: CasaImageLoader')]")
            self.assertTrue( False, "Should not be able to locate the restore image loader button")
        except Exception:
            print "Test restore button was successfully removed"
        imageWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        self.assertIsNotNone( imageWindow, "Image window was not restored.")
    
    def test_maximize_remove(self):    
        driver = self.driver
        
        # Find and select the colormap window.
        colorWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowColormap']")
        self.assertIsNotNone( colorWindow, "Could not find color map window")
        ActionChains(driver).click(colorWindow).perform();
        
        #For later use, determine the number of DisplayWindows.
        windowCount = Util.get_window_count(self,driver )
        print "Window Count=", windowCount
        
        # Click the Window button
        windowButton = driver.find_element_by_xpath("//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Window']/..")
        self.assertIsNotNone( windowButton, "Could not find window button in the context menu")
        ActionChains(driver).click(windowButton).perform()
        
        # Look for the maximize button in the submenu.
        maximizeButton = driver.find_element_by_xpath("//div[text()='Maximize']/..")
        self.assertIsNotNone(maximizeButton, "Could not find maximize button on window subcontext menu.")
        ActionChains(driver).click(maximizeButton).perform()
        
        # Verify that there is single colormap window.
        colorWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowColormap']")
        self.assertIsNotNone( colorWindow, "Could not find color map window")
        
        # Now right click the context menu to remove the colormap window
        ActionChains(driver).context_click(colorWindow).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
        
        # Verify that there is one less window than was there originally and the colormap window is not in the list.
        newWindowCount = Util.get_window_count( self, driver )
        print "New Window Count=", newWindowCount
        self.assertEqual( windowCount, newWindowCount+1, "Window was not removed")
        try: 
            colorWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowColormap']")
            self.assertTrue( False, "Colormap window should be removed")
        except Exception:
            print "Colormap window was successfully removed"
        
    def test_maximize_restore(self):    
        driver = self.driver
        
        # Find and select the colormap window.
        colorWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowColormap']")
        self.assertIsNotNone( colorWindow, "Could not find color map window")
        ActionChains(driver).click(colorWindow).perform();
        
        #For later use, determine the number of DisplayWindows.
        windowCount = Util.get_window_count( self, driver )
        print "Window Count=", windowCount
        
        # Click the Window button
        windowButton = driver.find_element_by_xpath("//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Window']/..")
        self.assertIsNotNone( windowButton, "Could not find window button in the context menu")
        ActionChains(driver).click(windowButton).perform()
        
        # Look for the maximize button in the submenu.
        maximizeButton = driver.find_element_by_xpath("//div[text()='Maximize']/..")
        self.assertIsNotNone(maximizeButton, "Could not find maximize button on window subcontext menu.")
        ActionChains(driver).click(maximizeButton).perform()
        
        # Verify that there is single colormap window.
        colorWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowColormap']")
        self.assertIsNotNone( colorWindow, "Could not find color map window")
        
        # Now right click the context menu to restore the colormap window
        ActionChains(driver).context_click(colorWindow).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()
        
        # Verify that there are exactly the same number of windows as was there originally and the colormap window is present.
        newWindowCount = Util.get_window_count( self, driver )
        print "New Window Count=", newWindowCount
        self.assertEqual( windowCount, newWindowCount, "Window count has changed")
        colorWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowColormap']")
        self.assertIsNotNone( colorWindow, "Colormap window was not restored")
        
    # Test that we can add a window and change it into a statistics view.
    def test_add_window(self):
        driver = self.driver
        
        #For later use, determine the number of DisplayWindows.
        windowCount = Util.get_window_count( self, driver )
        print "Window Count=", windowCount
        
        #For later use, we also determine the number of DisplayWindows displaying a statistics plugin
        statWindowList = driver.find_elements_by_xpath( "//div[@qxclass='skel.widgets.Window.DisplayWindowStatistics']")
        statCount = len( statWindowList )
        print "Stat Window Count=", statCount
        
        # Find a window capable of loading an image.
        imageWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        self.assertIsNotNone( imageWindow, "Could not find image display window")
               
        # Select the window
        ActionChains(driver).click(imageWindow).perform()
        
        # Click the Window button
        windowButton = driver.find_element_by_xpath("//div[text()='Window']/..")
        self.assertIsNotNone( windowButton, "Could not click window button in the context menu")
        ActionChains(driver).click(windowButton).perform()
        
        # Look for the add button in the submenu.
        addButton = driver.find_element_by_xpath("//div/div[text()='Add']/..")
        self.assertIsNotNone(addButton, "Could not click minimize button on window subcontext menu.")
        ActionChains(driver).click(addButton).perform()
        
        # Check that we now have a generic empty window in the display and that the window count has gone up by one.
        emptyWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowGenericPlugin']")
        self.assertIsNotNone( emptyWindow, "Could not find empty display window")
        newWindowCount = Util.get_window_count( self, driver )
        print "New Window Count=", newWindowCount
        self.assertEqual( windowCount+1, newWindowCount, "Window count did not go up")
        
        # Select the empty window
        ActionChains(driver).click(emptyWindow).perform()
        
        # Change the plugin of the empty window to statistics by clicking the view menu and the statistics
        # plugin in the submenu.
        ActionChains(driver).context_click(emptyWindow).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
        
        #Verify that we have increased the number of statistics windows by one.
        newStatWindowList = driver.find_elements_by_xpath( "//div[@qxclass='skel.widgets.Window.DisplayWindowStatistics']")
        newStatCount = len( newStatWindowList )
        print "New statistics window count=", newStatCount
        self.assertEqual( newStatCount, statCount+1, "Statistics window count did not go up by one")
    
    
    # Test that an existing window can be removed.
    def stest_remove(self):
        driver = self.driver
        
        # Find and select the colormap window.
        colorWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowColormap']")
        self.assertIsNotNone( colorWindow, "Could not find color map window")
        ActionChains(driver).click(colorWindow).perform();
        
        #For later use, determine the number of DisplayWindows.
        windowCount = Util.get_window_count( self, driver )
        print "Window Count=", windowCount
        
        # Click the Window button
        windowButton = driver.find_element_by_xpath("//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Window']/..")
        self.assertIsNotNone( windowButton, "Could not find window button in the context menu")
        ActionChains(driver).click(windowButton).perform()
        
        # Look for the remove button in the submenu.
        removeButton = driver.find_element_by_xpath("//div[text()='Remove']/..")
        self.assertIsNotNone(removeButton, "Could not find remove button on window subcontext menu.")
        ActionChains(driver).click(removeButton).perform()
        
        # Verify that there is one less window than was there originally and the colormap window is not in the list.
        newWindowCount = Util.get_window_count( self, driver )
        print "New Window Count=", newWindowCount
        self.assertEqual( windowCount, newWindowCount+1, "Window was not removed")
        try: 
            colorWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowColormap']")
            self.assertTrue( False, "Colormap window should be removed")
        except Exception:
            print "Colormap window was successfully removed"
        
        
        
    def tearDown(self):
        self.driver.close()

if __name__ == "__main__":
    unittest.main()        
        
