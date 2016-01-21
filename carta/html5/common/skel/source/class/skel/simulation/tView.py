import unittest
import Util
import time
import selectBrowser
from selenium import webdriver
from flaky import flaky
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait

#Test that we can view different plugins.
@flaky(max_runs=3)
class tView(unittest.TestCase):
    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp(self, browser)

    # Test that we can change an animator to a CasaImageLoader
    def test_animator_to_casaimageloader(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 30).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Get the animation window count and make sure it is non-zero
        animWindowList = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")
        animWindowCount = len( animWindowList )
        print "Original animation count ", animWindowCount
        self.assertGreater( animWindowCount, 0, "There are not any animators")

        # Get the image window count
        imageWindowList = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        imageWindowCount = len( imageWindowList )

        # Locate an animator window and bring up the right-context menu,
        # changing to a CasaImageLoader.
        Util.animation_to_image_window( self, driver )
        time.sleep( timeout )

        # Verify that the animation count has gone down by one and the image count
        # has increased by one.
        animWindowList = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")
        newAnimWindowCount = len( animWindowList )
        print "New Animation Count ", newAnimWindowCount
        self.assertEqual( animWindowCount - 1, newAnimWindowCount, "Animation count did not decrease")
        imageWindowList = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        newImageWindowCount = len( imageWindowList )
        self.assertEqual( newImageWindowCount - 1, imageWindowCount, "Image window count did not increase")

    #Test that we do not get a segfault if we try to change a CasaImageLoader into a CasaImageLoader.
    def test_sameViewTwice(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 30).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Get the image window count and make sure it is non-zero
        imageWindowList = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        imageWindowCount = len( imageWindowList )
        print "Original image count ", imageWindowCount
        self.assertGreater( imageWindowCount, 0, "There are not any images")

        # Locate an image window and bring up the right-context menu,
        # changing to a CasaImageLoader.
        imageWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
        ActionChains(driver).context_click( imageWindow ).perform();
        # Change the plugin to another image loader
        ActionChains(driver).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()
        time.sleep( timeout )

        # Verify that the image count has remained the same
        imageWindowList2 = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        newImageWindowCount = len( imageWindowList2 )
        print "New Image Count ", newImageWindowCount
        self.assertEqual( imageWindowCount, newImageWindowCount, "Image window count did not remain the same")

    #Test the following sequence which at one point generated a segfault:
    #1)  Change to a 2,1 layout
    #2)  Change the first window to a CasaImageLoader
    #3)  Change the second window to something not a CasaImageLoader
    #4)  Click on the first window.
    def test_replaceImageLoader(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 30).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Change to a 2,1 layout
        Util.layout_custom(self, driver, 2, 1 )
        time.sleep( timeout )

        # Change the first window, which should be empty to an image window
        emptyWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowGenericPlugin']")))
        ActionChains(driver).context_click( emptyWindow ).perform();
        # Change the plugin to an image loader
        ActionChains(driver).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()
        time.sleep( timeout )

        # There should now be 2 Image Loader Windows
        imageWindows = driver.find_elements(By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        imageWindowCount = len( imageWindows )
        print "Image window count=",imageWindowCount
        self.assertEqual( imageWindowCount, 2, "Image window count was not 2")
        time.sleep( timeout )

        # Now change the second image loader window to something else
        ActionChains(driver).click( imageWindows[1] ).perform()
        ActionChains(driver).context_click( imageWindows[1] ).perform();
        # Change the plugin to an image loader
        ActionChains(driver).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ARROW_RIGHT
                ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
        time.sleep( timeout )

        #Verify there is now just one image loader
        imageWindows = driver.find_elements(By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        imageWindowCount = len( imageWindows )
        print "Image window count=",imageWindowCount
        self.assertEqual( imageWindowCount, 1, "Image window count was not 1")
        time.sleep( timeout )

        # Finally, click on the first image window and verify we can load an image
        # in it
        ActionChains( driver).click( imageWindows[0]).perform()
        Util.load_image( self, driver, "aJ.fits", "img_pwUID2")

    def tearDown(self):
        #Close the browser
        self.driver.close()
        #Allow browser to fully close before continuing
        time.sleep(2)
        #Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()
