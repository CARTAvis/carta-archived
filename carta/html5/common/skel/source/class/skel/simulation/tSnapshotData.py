import tSnapshot
import unittest
import Util
import time
import selectBrowser
from selenium import webdriver
from flaky import flaky
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.by import By


#Test that a data snapshot can be saved/restored.
@flaky(max_runs=3)
class tSnapshotData(tSnapshot.tSnapshot):

    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp(self, browser)
        
    def open_restore(self, driver):
        sessionButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Session']/..")))
        ActionChains( driver ).click( sessionButton ).perform();
        ActionChains(driver).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()


    def _verifyImage(self, driver, count ):
        #Get the upper bound of images from the image animator
        if ( count > 1 ):
            imageUpperSpin = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ImageUpperBoundSpin']/input")))
            imageCount = imageUpperSpin.get_attribute( "value")
            print "Expected count=",count," actualCount=",imageCount
            self.assertEqual( int(imageCount), count, "Incorrect image count")
        #In exact, but we verify the image animator is not present which means 0 or 1.
        else:
            try:
                imageUpperSpin = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ImageUpperBoundSpin']/input")))
                self.assertTrue( False, "Image animator should not be present")
            except Exception:
                print "0 or 1 images present"

    # Set the channel animator to the last channel.  Save a data snapshot.
    # Set the channel animator back to 0, the first channel.  Restore a data snapshot.
    def test_animator_channel(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 30).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # Load an image so that there are a non-trivial number of channels.
        Util.load_image(self, driver, "TWHydra_CO2_1line.image.fits")

        # Find the last channel by finding the value of the upper bound spin box
        time.sleep( timeout )
        upperSpin = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ChannelUpperBoundSpin']/input")))
        lastChannel = upperSpin.get_attribute( "value")
        print 'Last channel', lastChannel

        # Set the channel animator to the last channel by typing into the text box.
        indexText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//input[@id='ChannelIndexText']")))
        Util._changeElementText( self, driver, indexText, lastChannel)

        # Find the session button on the menu bar and click it.
        menuBar = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Menu.MenuBar']")))
        self._clickSessionButton( driver )

        # Find the save session button in the submenu and click it.
        self._clickSessionSaveButton( driver )

        # The save popup should be visible.  Make sure data is checked and
        # layout and preferences are not checked
        self._setSaveOptions( driver, False, False, True )

        # Type in tSnapshotData for the save name.
        self._setSaveName( driver, "tSnapshotData")

        # Hit the save button
        self._saveSnapshot( driver )

        # Close the dialog
        self._closeSave( driver )

        # Now set the channel animator to 0 by typing into the index text box.
        #indexText = driver.find_element_by_xpath( "//input[@id='ChannelIndexText']")
        #self.assertIsNotNone( indexText, "Could not find channel input text")
        Util._changeElementText( self, driver, indexText, 0)

        # Click the restore sessions button
        self.open_restore( driver )

        # Select tSnapshotData in the restore combo box
        self._selectRestoreSnapshot( driver, "tSnapshotData")

        # Hit the restore button
        self._restoreSnapshot( driver )

        # Close the restore dialog
        self._closeRestore( driver )
        time.sleep( timeout )

        # Verify the animator channel is back to the last one
        channelVal = indexText.get_attribute( "value")
        print "Channel value=",channelVal, " lastChannel=", lastChannel
        self.assertEqual( channelVal, lastChannel, "Channel animator did not get restored to last channel")


    # Load a particular image.  Save a data snapshot.
    # Load a new image.  Restore a data snapshot.
    # Test that the original image is loaded but the second one is not
    def test_image_load(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 30).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

         # There must be an image loaded with more than one channel to see the channel animator.
        Util.load_image(self,driver, "TWHydra_CO2_1line.image.fits" )

        #Click on the animation window so that its actions will be enabled
        animationWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")))

        #Make sure the animation window is clicked by clicking an element within the window
        channelText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ChannelIndexText")))
        ActionChains(driver).click( channelText).perform()

        # Load two images in order for the image animator to be visible
        Util.load_image(self, driver, "Default")

        # Show the image animator
        imageUpperSpin = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ImageUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", imageUpperSpin )

        # Find the session button on the menu bar and click it.
        menuBar = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Menu.MenuBar']")))
        self._clickSessionButton( driver )

        # Find the save session button in the submenu and click it.
        self._clickSessionSaveButton( driver )

        # The save popup should be visible.  Make sure data is checked and
        # layout and preferences are not checked
        self._setSaveOptions( driver, False, False, True )

        # Type in tSnapshotData for the save name.
        self._setSaveName( driver, "tSnapshotData")

        # Hit the save button
        self._saveSnapshot( driver )

        # Close the dialog
        self._closeSave( driver )

        # Load another image
        Util.load_image(self, driver, "aH.fits")

        # Verify there are three images loaded (they go from 0 to 2).
        self._verifyImage( driver, 2)

        # Click the restore sessions button
        self.open_restore( driver )

        # Select tSnapshotData in the restore combo box
        self._selectRestoreSnapshot( driver, "tSnapshotData")

        # Hit the restore button
        self._restoreSnapshot( driver )

        # Close the restore dialog
        self._closeRestore( driver )
        time.sleep( timeout )

        # Verify that only the original two images are loaded
        self._verifyImage( driver, 1 )
        
    
    
    # Test that a data snapshot can be taken with 3 images loaded and that they
    # will all be restored if the snapshot is reloaded.
    def test_multipleImagesRestored(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 30).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

         # Load 3 images.
        Util.load_image(self,driver, "TWHydra_CO2_1line.image.fits" )
        Util.load_image(self,driver, "aH.fits" )
        Util.load_image(self,driver, "aJ.fits" )
        time.sleep(3)
        
        # Verify 3 images are loaded
        self._verifyImage( driver, 2 )
        
        # Save a data snapshot.
         # Find the session button on the menu bar and click it.
        menuBar = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Menu.MenuBar']")))
        self._clickSessionButton( driver )

        # Find the save session button in the submenu and click it.
        self._clickSessionSaveButton( driver )

        # The save popup should be visible.  Make sure data is checked and
        # layout and preferences are not checked
        self._setSaveOptions( driver, False, False, True )

        # Type in tSnapshotData for the save name.
        self._setSaveName( driver, "tSnapshotData")

        # Hit the save button
        self._saveSnapshot( driver )

        # Close the dialog
        self._closeSave( driver )
        
        # Remove all 3 the image
        ActionChains(driver).double_click( imageWindow ).perform()
        dataButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Data']/..")))
        for i in range(0,2):
            ActionChains(driver).click( dataButton ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
                Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()
                
        # Verify no images are loaded
        self._verifyImage( driver, 0 )
        
         # Click the restore sessions button
        self.open_restore( driver )

        # Select tSnapshotData in the restore combo box
        self._selectRestoreSnapshot( driver, "tSnapshotData")

        # Hit the restore button
        self._restoreSnapshot( driver )

        # Close the restore dialog
        self._closeRestore( driver )
        time.sleep( timeout )
        
        # Verify 3 images are loaded
        self._verifyImage( driver, 2 );
        
    # Load an image. Save a data snapshot.  Remove the image.  Restore
    # the snapshot.  Check that the image is loaded.
    def test_image_remove(self):    
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 30).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

         # There must be an image loaded with more than one channel to see the channel animator.
        Util.load_image(self,driver, "TWHydra_CO2_1line.image.fits" )

        #Click on the animation window so that its actions will be enabled 
        animationWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")))
        
        #Make sure the animation window is clicked by clicking an element within the window
        channelText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ChannelIndexText")))
        ActionChains(driver).click( channelText).perform()

        # Check that the channel upper spin is visible to verify the image is loaded.
        channelUpperSpin = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ChannelUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", channelUpperSpin ) 
        
        # Find the session button on the menu bar and click it.
        menuBar = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Menu.MenuBar']")))
        self._clickSessionButton( driver )
        
        # Find the save session button in the submenu and click it.
        self._clickSessionSaveButton( driver )
        
        # The save popup should be visible.  Make sure data is checked and
        # layout and preferences are not checked
        self._setSaveOptions( driver, False, False, True )
        
        # Type in tSnapshotData for the save name.
        self._setSaveName( driver, "tSnapshotData")
        
        # Hit the save button
        self._saveSnapshot( driver )
        
        # Close the dialog
        self._closeSave( driver )
        
        # Remove the image
        ActionChains(driver).double_click( imageWindow ).perform()
        dataButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Data']/..")))
        ActionChains(driver).click( dataButton ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()
        
        # Verify there are no images loaded.
        try:
            channelUpperSpin = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ChannelUpperBoundSpin']/input")))
            driver.execute_script( "arguments[0].scrollIntoView(true);", channelUpperSpin )
            assertTrue(False,"Upper spin was present")
        except Exception:
            print "Test passed - all images are gone"
        
        # Click the restore sessions button
        self.open_restore( driver )
        
        # Select tSnapshotData in the restore combo box
        self._selectRestoreSnapshot( driver, "tSnapshotData")
        
        # Hit the restore button
        self._restoreSnapshot( driver )
        
        # Close the restore dialog
        self._closeRestore( driver )
        time.sleep( timeout )
        
        # Verify the original image is present - i.e., no image animator but a channel animator.
        self._verifyImage( driver, 0 )
        channelUpperSpin = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@id='ChannelUpperBoundSpin']/input")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", channelUpperSpin )

if __name__ == "__main__":
    unittest.main()
