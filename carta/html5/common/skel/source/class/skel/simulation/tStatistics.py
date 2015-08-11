import unittest
import time
import selectBrowser
import Util
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.by import By


# Tests of Statistics functionality
class tStatistics( unittest.TestCase ):

    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp( self, browser )

    # Check that the Stats Window is linked to the image window by default
    def test_statsDefaultLinking(self):
        driver = self.driver

        # Load a large test image. 
        Util.load_image( self, driver, "aH.fits")
        
        # Move to the center of the image window so data appears in the Stats Window
        imageWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
        ActionChains(driver).move_to_element( imageWindow ).perform()

        # Sometimes text does not appear, therefore move mouse cursor
        ActionChains(driver).move_by_offset( 100, 100 ).perform()

        # Check that the Stastics window is linked to the image window
        statsText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.boundWidgets.Label']")))
        statsText = statsText.get_attribute('textContent')        
        
        # Check that the Default sky text appears in the Stats Window
        statsText = statsText.startswith("Default sky")
        self.assertEqual( statsText, 1, "Stats window is not linked to image window by default")

    # Check that the Stats Window updates when it is linked to a different image
    # after removing the old link from the main image window 
    def test_statsChangeLinkUpdate(self):
        driver = self.driver 
        browser = selectBrowser._getBrowser()

        # Test only works on Chrome
        if browser ==2:
            # Load an image in a new image window
            imageWindow2 = Util.load_image_different_window( self, driver, "aH.fits")

            # Find and click on the Statistics window 
            statsWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='Statistics']")))
            self.assertIsNotNone( statsWindow, "Could not find statistics window")
            ActionChains(driver).click( statsWindow ).perform()

            # In Stastics context menu, open Link Settings
            ActionChains(driver).context_click( statsWindow ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
            
            # Remove the link from the Statistics Window to the image window
            imageWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
            ActionChains(driver).move_to_element( imageWindow ).context_click( imageWindow ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

            # Change the link to the loaded image 
            ActionChains(driver).move_to_element( statsWindow ).click( statsWindow ).drag_and_drop( statsWindow, imageWindow2).perform()

            # Exit links
            ActionChains(driver).move_to_element( statsWindow ).context_click( statsWindow ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

            # Move to the center of the image window so data appears in the Stats Window
            ActionChains(driver).move_to_element( imageWindow2 ).perform()

            # Sometimes text does not appear, therefore move mouse cursor
            ActionChains(driver).move_by_offset( 100, 100 ).perform()

            # First check that there is text in the Stats Window
            statsText = len(driver.find_elements_by_xpath("//div[@qxclass='skel.boundWidgets.Label']"))
            self.assertEqual( int(statsText), 1, "Should be able to find text in Stats Window")

            # Check that the Stastics window is linked to the image window
            statsText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.boundWidgets.Label']")))
            statsText = statsText.get_attribute('textContent')         
                
            # Check that the Default sky text appears in the Stats Window
            statsText = statsText.startswith("Default sky")
            self.assertEqual( statsText, 1, "Stats window did not link to image after previous link was removed and new link was created to different window")

    # Check that the Stats Window is not linked to the main window after changing links
    def test_statsChangeLink(self):
        driver = self.driver 
        browser = selectBrowser._getBrowser()

        # This test does not work on Firefox, only Chrome
        if browser == 2:
            # Find and click on the image window  
            imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
            self.assertIsNotNone( imageWindow, "Could not find image display window")
            ActionChains(driver).click( imageWindow ).perform()

            # Click the Window button
            windowButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Window']/..")))
            self.assertIsNotNone( windowButton, "Could not find window button in the context menu")
            ActionChains(driver).click(windowButton).perform()

            # Look for the add button in the submenu.
            addButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div/div[text()='Add']/..")))
            self.assertIsNotNone( addButton, "Could not click minimize button on window subcontext menu.")
            ActionChains(driver).click( addButton ).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()

            # Check that we now have a generic empty window in the display and that the window count has gone up by one.
            emptyWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowGenericPlugin']")))
            self.assertIsNotNone( emptyWindow, "Could not find empty display window")

            # Select the empty window
            ActionChains(driver).click( emptyWindow ).perform()
            
            # Change the plugin of the empty window to an image loader 
            ActionChains(driver).context_click( emptyWindow ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()

            # Find and click on the Statistics window 
            statsWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='Statistics']")))
            self.assertIsNotNone( statsWindow, "Could not find statistics window")
            ActionChains(driver).click( statsWindow ).perform()

            # In Stastics context menu, open Link Settings
            ActionChains(driver).context_click( statsWindow ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
            
            # Remove the link from the Statistics Window to the main image window
            imageWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
            ActionChains(driver).move_to_element( imageWindow ).context_click( imageWindow ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

            # Change the link to the new image window
            imageWindow2 = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='CasaImageLoader2']")))
            ActionChains(driver).move_to_element( statsWindow ).click( statsWindow ).drag_and_drop( statsWindow, imageWindow2).perform()

            # Exit links
            ActionChains(driver).move_to_element( statsWindow ).context_click( statsWindow ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

            # Load an image in the main image window
            Util.load_image( self, driver, "aH.fits")

            # Move to the center of the image window so data appears in the Stats Window
            ActionChains(driver).move_to_element( imageWindow ).perform()

            # Sometimes text does not appear, therefore move mouse cursor
            ActionChains(driver).move_by_offset( 100, 100 ).perform()

            # Check that the Stastics Window is not linked to the image window
            statsText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.boundWidgets.Label']")))
            statsText = statsText.get_attribute('textContent')        
            
            # Check that the Default sky text appears in the Stats Window
            statsText = statsText.startswith("Default sky")
            self.assertEqual( statsText, 0, "Stats window is still linked to the main image window after the link was removed")

    # Check that the Stats Window returns to default values when the linked
    # image is removed from the image window 
    def test_statsRemoveImage(self):
        driver = self.driver 

        # Load a large test image. 
        Util.load_image( self, driver, "aH.fits")

        # Click on the Data->Close->Image button to close the image.
        imageWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
        ActionChains(driver).context_click( imageWindow ).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()
        
        # Move to the center of the image window so data appears in the Stats Window
        ActionChains(driver).move_to_element( imageWindow ).perform()

        # Sometimes text does not appear, therefore move mouse cursor
        ActionChains(driver).move_by_offset( 100, 100 ).perform()

        # Check that no data appears in the Stastics window 
        statsText = len(driver.find_elements_by_xpath("//div[@qxclass='skel.boundWidgets.Label']"))
        self.assertEqual( int(statsText), 0, "Stats Window does not update after image removal")

    # Test that the Stats Window updates when an image is added to the main window
    def test_statsAddImage(self):
        driver = self.driver

        # Load a large test image. 
        Util.load_image( self, driver, "aH.fits")

        # Move to the center of the image window so data appears in the Stats Window
        imageWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
        ActionChains(driver).move_to_element( imageWindow ).perform()

        # Get the Statistics of the loaded image 
        statsText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.boundWidgets.Label']")))
        statsText = statsText.get_attribute('textContent')        
        
        # Load a different image in the same window
        Util.load_image( self, driver, "aK.fits")

        # Move to the center of the image window so data appears in the Stats Window
        ActionChains(driver).move_to_element( imageWindow ).perform()

        # Sometimes text does not appear, therefore move mouse cursor
        ActionChains(driver).move_by_offset( 100, 100 ).perform()

        # Get the Statistics of the loaded image 
        newStatsText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.boundWidgets.Label']")))
        newStatsText = newStatsText.get_attribute('textContent')

        # Check that the Statistics text changed when new image was loaded
        self.assertNotEqual( newStatsText, statsText, "Stats text did not update when new image was loaded in main image window")

    # Test that the Stats Window updates when the image in the image window is changed
    def test_statsChangeImage(self):
        driver = self.driver 

        # Load two images in the same window
        Util.load_image( self, driver, "Default")
        Util.load_image( self, driver, "aH.fits")

        # Move to the center of the image window so data appears in the Stats Window
        imageWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
        ActionChains(driver).move_to_element( imageWindow ).perform()
        
        # Sometimes text does not appear, therefore move mouse cursor
        ActionChains(driver).move_by_offset( 100, 100 ).perform()

        # Get the Statistics of the loaded image 
        statsText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.boundWidgets.Label']")))
        statsText = statsText.get_attribute('textContent')   

        # Find and click on the animation window 
        animWindow = driver.find_element_by_xpath( "//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")
        self.assertIsNotNone( animWindow, "Could not find animation window")
        ActionChains(driver).click( animWindow ).perform()   

        # Make sure the animation window is enabled by clicking an element within the window
        # From the context menu, uncheck the Channel Animator and check the Image Animator
        channelText = driver.find_element_by_id( "ChannelIndexText")
        ActionChains(driver).click( channelText ).perform()
        ActionChains(driver).context_click( channelText ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys( 
            Keys.ARROW_DOWN ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.SPACE).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        # Find the first value button and click the button
        firstValueButton = driver.find_element_by_xpath( "//div[@class='qx-toolbar']/div[@qxclass='qx.ui.toolbar.Button'][1]")
        self.assertIsNotNone( firstValueButton, "Could not find button to go to the first valid value")
        driver.execute_script( "arguments[0].scrollIntoView(true);", firstValueButton)
        ActionChains(driver).click( firstValueButton ).perform()

        # Move to the center of the image window so data appears in the Stats Window
        imageWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
        ActionChains(driver).move_to_element( imageWindow ).perform()

        # Sometimes text does not appear, therefore move mouse cursor
        ActionChains(driver).move_by_offset( 100, 100 ).perform()

        # Get the Statistics of the loaded image 
        newStatsText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.boundWidgets.Label']")))
        newStatsText = newStatsText.get_attribute('textContent')   

        # Check that the Statistics text changed when image in the image window was changed
        self.assertNotEqual( newStatsText, statsText, "Stats text did not update when the image was changed in the window")

    # Test that the Stats Window updates its values during the animation of an image
    def test_statsAnimation(self):
        driver = self.driver

        # Load a test image with animation
        Util.load_image( self, driver, "Default")

        # Move to the center of the image window so data appears in the Stats Window
        imageWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
        ActionChains(driver).move_to_element( imageWindow ).perform()

        # Get the Statistics of the loaded image 
        statsText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.boundWidgets.Label']")))
        statsText = statsText.get_attribute('textContent')   

        # Click the forward animation button
        forwardAnimateButton = driver.find_element_by_xpath("//div[@class='qx-toolbar']/div[@class='qx-button'][2]")
        self.assertIsNotNone( forwardAnimateButton, "Could not find forward animation button")
        driver.execute_script( "arguments[0].scrollIntoView(true);", forwardAnimateButton)
        ActionChains(driver).click( forwardAnimateButton ).perform()
        time.sleep(3)

        # Move to the center of the image window so data appears in the Stats Window
        imageWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
        ActionChains(driver).move_to_element( imageWindow ).perform()
        
        # Sometimes text does not appear, therefore move mouse cursor
        ActionChains(driver).move_by_offset( 100, 100 ).perform()

        # Get the Statistics of the loaded image 
        newStatsText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.boundWidgets.Label']")))
        newStatsText = newStatsText.get_attribute('textContent')   

        # Check that the Statistics text changed when image in the image window was changed
        self.assertNotEqual( newStatsText, statsText, "Stats text did not update during animation of the image")

    # Test that the Stats Window can only be linked to one image window 
    def test_statsAddLink(self):
        driver = self.driver 
        time.sleep(5)

        # Load a different image in a different window
        imageWindow2 = Util.load_image_different_window( self, driver, "aK.fits")

        # Find and click on the Statistics window 
        statsWindow = driver.find_element_by_xpath("//div[@id='Statistics']")
        self.assertIsNotNone( statsWindow, "Could not find statistics window")
        ActionChains(driver).click( statsWindow ).perform()

        # In Stastics context menu, open Link Settings
        ActionChains(driver).context_click( statsWindow ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        # Add a link to the new image window
        imageWindow2 = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='CasaImageLoader2']")))
        ActionChains(driver).move_to_element( statsWindow ).click( statsWindow ).drag_and_drop( statsWindow, imageWindow2).perform()

        # Exit links
        ActionChains(driver).move_to_element( statsWindow ).context_click( statsWindow ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        # Move to the center of the image window so data appears in the Stats Window
        ActionChains(driver).move_to_element( imageWindow2 ).perform()
        
        # Sometimes text does not appear, therefore move mouse cursor
        ActionChains(driver).move_by_offset( 100, 100 ).perform()

        # Check that the Stastics window is not linked to the main image window
        statsText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.boundWidgets.Label']")))
        statsText = statsText.get_attribute('textContent')        
        
        # Check that the Default sky text appears in the Stats Window
        statsText = statsText.startswith("Default sky")
        self.assertEqual( statsText, 0, "Statistics window should not be linked to multiple windows")

    # Check that a link to the Statistics window can be removed
    def test_statsRemoveLink(self):
        driver = self.driver 
        browser = selectBrowser._getBrowser()

        # This test only works on Chrome
        if browser == 2:
            # Find and click on the Statistics window 
            statsWindow = driver.find_element_by_xpath("//div[@id='Statistics']")
            self.assertIsNotNone( statsWindow, "Could not find statistics window")
            ActionChains(driver).click( statsWindow ).perform()

            # In Stastics context menu, open Link Settings
            ActionChains(driver).context_click( statsWindow ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
            
            # Remove the link from the Statistics Window to the image window
            imageWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
            ActionChains(driver).move_to_element( imageWindow ).context_click( imageWindow ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

            # Exit links
            ActionChains(driver).move_to_element( imageWindow ).context_click( imageWindow ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

            # Load a large test image. 
            Util.load_image( self, driver, "aH.fits")
            
            # Move to the center of the image window so data appears in the Stats Window
            imageWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
            ActionChains(driver).move_to_element( imageWindow ).perform()
            
            # Sometimes text does not appear, therefore move mouse cursor
            ActionChains(driver).move_by_offset( 100, 100 ).perform()

            # Check that the Stastics Window is not linked to the image window
            statsText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.boundWidgets.Label']")))
            statsText = statsText.get_attribute('textContent')        
            
            # Check that the Default sky text appears in the Stats Window
            statsText = statsText.startswith("Default sky")
            self.assertEqual( statsText, 0, "Stats window is still linked to the main image window after the link was removed")


    def tearDown(self):
        # Close the browser
        self.driver.close()
        # Allow browser to fully close before continuing
        time.sleep(2)
        # Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()      
            
        
