import time
import unittest
import Util
import selectBrowser
from selenium import webdriver
from flaky import flaky
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.by import By


# Test window manipulation.
@flaky(max_runs=3)
class tWindow(unittest.TestCase):

    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp(self, browser)

    def _clickLayoutButton(self, driver):
        # Find the layout button on the menu bar and click it.
        layoutButton = WebDriverWait(driver, 20).until(EC.visibility_of_element_located((By.XPATH, "//div[text()='Layout']/..")))
        ActionChains(driver).click( layoutButton ).perform()

    # Test that a window can be minimized and then restored to its original position
    def test_minimize_restore(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Find a window capable of loading an image and select the window
        imageWindow = WebDriverWait(driver, 30).until(EC.visibility_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
        ActionChains(driver).click( imageWindow ).perform()
        time.sleep( timeout )

        # Click the Window button
        windowButton = driver.find_element_by_xpath( "//div[text()='Window']/..")
        ActionChains(driver).click( windowButton ).perform()

        # Look for the minimize button in the submenu.
        minimizeButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div/div[text()='Minimize']/..")))
        ActionChains(driver).click( minimizeButton ).perform()

        # Verify that there is a Restore button on the status bar and no DisplayWindowImage.
        restoreButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[contains(text(), 'Restore: ImageViewer')]/..")))

        # Restore the window.  Verify the restore button is gone from the status bar and there is a DisplayWindowImage
        ActionChains(driver).click( restoreButton ).perform()

        #Check that the clipping menu item is no longer available
        try:
            restoreLabel = WebDriverWait(driver, 5).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[contains(text(), 'Restore: ImageViewer')]")))
            self.assertTrue( False, "Should not be able to locate the restore image loader button")
        except Exception:
            print "Test restore button was successfully removed"
        imageWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

    def test_maximize_remove(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

         # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Find and select the colormap window.
        colorWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowColormap']")))
        ActionChains(driver).click(colorWindow).perform();

        #For later use, determine the number of DisplayWindows.
        windowCount = Util.get_window_count( self,driver )
        print "Window Count=", windowCount

        # Click the Window button
        windowButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Window']/..")))
        ActionChains(driver).click( windowButton ).perform()

        # Look for the maximize button in the submenu.
        maximizeButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Maximize']/..")))
        ActionChains(driver).click(maximizeButton).perform()

        # Verify that there is single colormap window.
        colorWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowColormap']")))
        ActionChains(driver).click( colorWindow).perform()
        ActionChains(driver).click( colorWindow).perform()

        # Remove the colormap window
        ActionChains(driver).context_click(colorWindow).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN
            ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
        time.sleep( timeout )

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
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Find and select the colormap window.
        colorWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowColormap']")))
        ActionChains(driver).click(colorWindow).perform();

        #For later use, determine the number of DisplayWindows.
        windowCount = Util.get_window_count( self, driver )
        print "Window Count=", windowCount

        # Click the Window button
        windowButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Window']/..")))
        ActionChains(driver).click(windowButton).perform()

        # Look for the maximize button in the submenu.
        maximizeButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Maximize']/..")))
        ActionChains(driver).click(maximizeButton).perform()

        # Verify that there is single colormap window.
        colorWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowColormap']")))

        # Now right click the context menu to restore the colormap window
        ActionChains(driver).context_click(colorWindow).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()
        time.sleep( timeout )

        # Verify that there are exactly the same number of windows as was there originally and the colormap window is present.
        newWindowCount = Util.get_window_count( self, driver )
        print "New Window Count=", newWindowCount
        self.assertEqual( windowCount, newWindowCount, "Window count has changed")
        colorWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowColormap']")))

    # Test that we can add a window and change it into a histogram view.
    def test_add_window(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # For later use, determine the number of DisplayWindows.
        windowCount = Util.get_window_count( self, driver )
        print "Window Count=", windowCount

        #For later use, we also determine the number of DisplayWindows displaying a histogram plugin
        histWindowList = driver.find_elements_by_xpath( "//div[@qxclass='skel.widgets.Window.DisplayWindowHistogram']")
        histCount = len( histWindowList )
        print "Histogram Window Count=", histCount

        # Find a window capable of loading an image.
        imageWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")

        # Select the window
        ActionChains(driver).click(imageWindow).perform()

        # Click the Window button
        windowButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Window']/..")))
        ActionChains(driver).click(windowButton).perform()

        # Look for the add button in the submenu.
        addButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div/div[text()='Add']/..")))
        ActionChains(driver).click( addButton ).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()
        time.sleep( timeout )

        # Check that we now have a generic empty window in the display and that the window count has gone up by one.
        emptyWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowGenericPlugin']")))
        newWindowCount = Util.get_window_count( self, driver )
        print "New Window Count=", newWindowCount
        self.assertEqual( windowCount+1, newWindowCount, "Window count did not go up")

        # Select the empty window
        ActionChains(driver).click( emptyWindow ).perform()

        # Change the plugin of the empty window to histogram by clicking the view menu and the histogram
        # plugin in the submenu.
        viewMenuButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='View']/..")))
        ActionChains(driver).click( viewMenuButton ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
        time.sleep( timeout )

        #Verify that we have increased the number of histogram windows by one.
        newHistWindowList = driver.find_elements_by_xpath( "//div[@qxclass='skel.widgets.Window.DisplayWindowHistogram']")
        newHistCount = len( newHistWindowList )
        print "New histogram window count=", newHistCount
        self.assertEqual( newHistCount, histCount+1, "Histogram window count did not go up by one")

    # Test that an existing window can be removed.
    def test_remove(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Find and select the colormap window.
        colorWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowColormap']")))
        self.assertIsNotNone( colorWindow, "Could not find color map window")
        ActionChains(driver).click( colorWindow ).perform();

        # For later use, determine the number of DisplayWindows.
        windowCount = Util.get_window_count( self, driver )
        print "Window Count=", windowCount

        # Click the Window button
        windowButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Window']/..")))
        self.assertIsNotNone( windowButton, "Could not find window button in the context menu")
        ActionChains(driver).click(windowButton).perform()

        # Look for the remove button in the submenu.
        removeButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Remove']/..")))
        self.assertIsNotNone( removeButton, "Could not find remove button on window subcontext menu.")
        ActionChains(driver).click( removeButton ).perform()
        time.sleep( timeout )

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
        # Close the browser
        self.driver.close()
        # Allow browser to fully close before continuing
        time.sleep(2)
        # Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()
