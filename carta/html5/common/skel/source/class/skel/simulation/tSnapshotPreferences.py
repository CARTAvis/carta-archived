import tSnapshot
import Util
import time
import unittest
import selectBrowser
from selenium import webdriver
from flaky import flaky
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.by import By

# Test that a preference snapshot can be saved/restored.
@flaky(max_runs=3)
class tSnapshotPreferences(tSnapshot.tSnapshot):

    def _isToolbarVisible(self, driver ):
        visible = False
        try:
            toolBar = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Menu.ToolBar']")
            self.assertIsNotNone( toolBar, "Tool bar was not hidden")
            visible = True
        except Exception:
            print "Tool bar is hidden"
        return visible

    def _savePreferences(self, driver ):
        # Find the session button on the menu bar and click it.
        self._clickSessionButton( driver )

        # Find the save session button in the submenu and click it.
        self._clickSessionSaveButton( driver )

        # The save popup should be visible.  Make sure preferences are checked and
        # layout and data are not checked
        self._setSaveOptions( driver, True, False, False )

        # Type in tSnapshotPreferences for the save name.
        self._setSaveName( driver, "tSnapshotPreferences")

        # Hit the save button
        self._saveSnapshot( driver )

        # Close the dialog
        self._closeSave( driver )


    def _restorePreferences(self, driver ):
        # Click the restore sessions button
        self._clickSessionButton( driver )
        time.sleep(2)
        self._clickSessionRestoreButton( driver )

        # Select tSnapshotPreferences in the restore combo box
        self._selectRestoreSnapshot( driver, "tSnapshotPreferences")

        # Hit the restore button
        self._restoreSnapshot( driver )

        # Close the restore dialog
        self._closeRestore( driver )


    def _setAnimatorToJump(self, driver ):
        timeout = selectBrowser._getSleep()
        # Necessary for Chrome
        time.sleep( timeout )
        # Find the settings button on the animator and click it.
        settingsButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.CheckBox']/div[text()='Settings...']")))
        ActionChains( driver).click( settingsButton).perform()

        # Find the jump radio button in the settings and click it
        jumpButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ChannelJumpRadioButton")))
        # Scroll the animator window so the jump button is visible.
        driver.execute_script( "arguments[0].scrollIntoView(true);", jumpButton)
        self._setChecked( driver, jumpButton, True )

    def _verifyChecked(self, driver, radButton ):
        jumpClass = radButton.get_attribute( "class")
        print "jumpChecked=", jumpClass
        self.assertEqual( jumpClass, "qx-radiobutton-checked","Jump end behavior is not checked")

    # The purpose of this test is to test that if we try to restore preferences for
    # a widget that is not present there are no problems.
    # We verify an animator is present.  We save the preferences.  We remove the animator
    # We verify that there is not a problem by checking error status is empty and window
    # count remains the same with no animator.
    def test_restore_missing(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
        ActionChains(driver).click( imageWindow).perform()
        time.sleep( timeout )

        # Store the window count
        windowCount = Util.get_window_count( self, driver )
        print "Window Count ", windowCount

        # Verify that there is just one animator
        animWindowList = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")
        animWindowCount = len( animWindowList )
        self.assertEqual( animWindowCount, 1, "There was not exactly one animator")

        # Save the preferences
        self._savePreferences( driver )

        # Locate the animator window and bring up the right-context menu,
        # changing to a CasaImageLoader.
        Util.animation_to_image_window( self, driver )
        time.sleep( timeout )

        # Verify that there are now no animation windows.
        animWindowList = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")
        animWindowCount = len( animWindowList )
        self.assertEqual( animWindowCount, 0, "An animator is still present")

        # Restore the preferences
        self._restorePreferences( driver )
        time.sleep( timeout )

        # Verify that there are no errors present
        statusLabel = driver.find_element_by_id( "statusLabel")
        statusChildren = statusLabel.find_elements_by_xpath(".//*")
        childCount = len(statusChildren)
        print "Error count", childCount
        self.assertEquals( childCount, 0, "There was an error restoring snapshot")

    # Set the channel animator to jump end behavior.  Save a preference snapshot.
    # Set the channel animator to wrap end behavior.  Restore the preference snapshot.
    # Check that the animator reverts back to jump behavior.
    def test_animator_jump(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

        # There must be an image loaded with more than one channel to see the animator.
        Util.load_image(self,driver, "TWHydra_CO2_1line.image.fits" )

        # Set the animator to jump
        self._setAnimatorToJump( driver );

        # Save the preferences
        self._savePreferences(driver )

        # Find the wrap radio button in the animator settings and click it
        wrapButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ChannelWrapRadioButton")))
        self._setChecked( driver, wrapButton, True )
        time.sleep(2)

        # Restore the preferences
        self._restorePreferences( driver )
        time.sleep( timeout )

        # Verify the animator jump end behavior is checked
        jumpButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.RadioButton']/div[text()='Jump']/following-sibling::div")))
        self._verifyChecked( driver, jumpButton )



    # Set hide the toolbar from the preferences menu.  Save a preference snapshot.
    # Show the toolbar.  Restore the preference snapshot.
    # Check that the toolbar is hidden.
    def test_toolbar_hide(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 30).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
        ActionChains(driver).click( imageWindow).perform()
        time.sleep( timeout )

        # Find the preferences button on the menu bar and click it.
        menuBar = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Menu.MenuBar']")))
        #self.assertIsNotNone( menuBar, "Could not find the menu bar")
        preferencesButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Preferences']/..")))
        #self.assertIsNotNone( preferencesButton, "Could not find div with text Preferences")
        ActionChains(driver).click(preferencesButton).perform()

        # Click the show button on the sub menu.
        showButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div/div[text()='Show']/..")))
        ActionChains(driver).click( showButton).perform()

        showToolButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Show Tool Bar']/..")))
        ActionChains(driver).click( showToolButton).perform()
        time.sleep( timeout )

        # Verify the toolbar is NOT visible
        toolVisible = self._isToolbarVisible( driver )
        self.assertFalse( toolVisible, "Tool bar was not hidden")

        # Save the preferences
        self._savePreferences( driver )

        # Show the toolbar
        # Find the preferences button on the menu bar and click it.
        preferencesButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Preferences']/..")))
        ActionChains(driver).click(preferencesButton).perform()

        # Click the show tool bar button on the sub menu.
        ActionChains(driver).send_keys( Keys.ARROW_DOWN ).send_keys( Keys.ARROW_RIGHT ).send_keys( Keys.ARROW_DOWN ).send_keys( Keys.ARROW_DOWN ).send_keys( Keys.ENTER ).perform()
        time.sleep( timeout )

        #Verify the toolbar is now visible
        toolBar = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Menu.ToolBar']")))

        # Click the restore sessions button
        self._clickSessionButton( driver )
        self._clickSessionRestoreButton( driver )
    
        # Select tSnapshotPreferences in the restore combo box
        self._selectRestoreSnapshot( driver, "tSnapshotPreferences")

        # Hit the restore button
        self._restoreSnapshot( driver )

        # Close the restore dialog
        self._closeRestore( driver )
        time.sleep( timeout )

        # Verify the toolbar is hidden again
        toolVisible = self._isToolbarVisible( driver)
        self.assertFalse( toolVisible, "Tool bar was not hidden with restore state")

if __name__ == "__main__":
    unittest.main()
