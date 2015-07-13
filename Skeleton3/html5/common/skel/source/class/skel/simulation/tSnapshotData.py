import tSnapshot
import unittest
import Util
import time
import selectBrowser
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

#Test that a data snapshot can be saved/restored.
class tSnapshotData(tSnapshot.tSnapshot):

    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp(self, browser)

    def _verifyImage(self, driver, count ):
        driver.implicitly_wait(5) 
        #Get the upper bound of images from the image animator
        imageUpperSpin = driver.find_element_by_xpath( "//div[@id='ImageUpperBoundSpin']/input")
        self.assertIsNotNone( imageUpperSpin, "Could not find upper bound for image animator")
        imageCount = imageUpperSpin.get_attribute( "value")
        self.assertEqual( int(imageCount), count, "Incorrect image count")
    
    # Set the channel animator to the last channel.  Save a data snapshot.
    # Set the channel animator back to 0, the first channel.  Restore a data snapshot.
    def test_animator_channel(self):    
        driver = self.driver
        time.sleep(5)

        # Load an image so that there are a non-trivial number of channels.
        #At some point this test will need to be rewritten to use a
        #test image available where the tests are running.
        Util.load_image(self, driver, "Default")
        
        # Find the last channel by finding the value of the upper bound spin box
        upperSpin = driver.find_element_by_xpath( "//div[@id='ChannelUpperBoundSpin']/input")
        self.assertIsNotNone( upperSpin, "Could not find channel animator upper bound spin box")
        lastChannel = upperSpin.get_attribute( "value")
        print 'Last channel', lastChannel
        
        # Set the channel animator to the last channel by typing into the text box.
        indexText = driver.find_element_by_xpath( "//input[@id='ChannelIndexText']")
        self.assertIsNotNone( indexText, "Could not find channel input text")
        Util._changeElementText( self, driver, indexText, lastChannel)
                
        # Find the session button on the menu bar and click it.
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
        Util._changeElementText(self, driver, indexText, 0)
        
        # Click the restore sessions button
        self._clickSessionButton( driver )
        self._clickSessionRestoreButton( driver )
        
        # Select tSnapshotData in the restore combo box
        self._selectRestoreSnapshot( driver, "tSnapshotData")
        
        # Hit the restore button
        self._restoreSnapshot( driver )
        
        # Close the restore dialog
        self._closeRestore( driver )
        
        # Allow the session to be restored
        time.sleep(2)

        # Verify the animator channel is back to the last one
        #indexText = driver.find_element_by_xpath( "//input[@id='ChannelIndexText']")
        #self.assertIsNotNone( indexText, "Could not find channel input text")
        channelVal = indexText.get_attribute( "value")
        self.assertEqual( channelVal, lastChannel, "Channel animator did not get restored to last channel")
        

    # Load a particular image.  Save a data snapshot.
    # Load a new image.  Restore a data snapshot.
    # Test that the original image is loaded but the second one is not
    def test_image_load(self):    
        driver = self.driver
        time.sleep(5)
        
        #Getting element not found in cache without this.
        driver.implicitly_wait(20)

        #Click on the animation window so that its actions will be enabled 
        animationWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")
        self.assertIsNotNone( animationWindow, "Could not find animation window")
        
        #Make sure the animation window is clicked by clicking an element within the window
        channelText = driver.find_element_by_id("ChannelIndexText")
        ActionChains(driver).click( channelText).perform()
        ActionChains(driver).context_click( channelText ).perform()

        # Show the image animator
        ActionChains( driver).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ARROW_DOWN).send_keys(
            Keys.ARROW_DOWN).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ARROW_RIGHT).send_keys(
            Keys.ARROW_DOWN).send_keys( Keys.ENTER).perform()
        time.sleep(2)

        imageUpperSpin = driver.find_element_by_xpath( "//div[@id='ImageUpperBoundSpin']/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", imageUpperSpin ) 
        
        # Load an image
        Util.load_image(self, driver, "Default")
        
        # Find the session button on the menu bar and click it.
        menuBar = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Menu.MenuBar']")
        self.assertIsNotNone( menuBar, "Could not find the menu bar")
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
        
        # Verify there are two images loaded (they go from 0 to 1).
        self._verifyImage( driver, 1)
        
        # Click the restore sessions button
        self._clickSessionButton( driver )
        self._clickSessionRestoreButton( driver )
        
        # Select tSnapshotData in the restore combo box
        self._selectRestoreSnapshot( driver, "tSnapshotData")
        
        # Hit the restore button
        self._restoreSnapshot( driver )
        
        # Close the restore dialog
        self._closeRestore( driver )
        
        # Verify that only the original image is loaded
        self._verifyImage( driver, 0 )

if __name__ == "__main__":
    unittest.main()   