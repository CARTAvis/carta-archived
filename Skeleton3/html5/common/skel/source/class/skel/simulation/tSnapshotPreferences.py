import tSnapshot
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

#Test that a preference snapshot can be saved/restored.
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
        
    # Set the channel animator to jump end behavior.  Save a preference snapshot.  
    # Set the channel animator to wrap end behavior.  Restore the preference snapshot.
    # Check that the animator reverts back to jump behavior.
    def test_animator_jump(self):    
        driver = self.driver
        
        # Find the settings button on the animator and click it.
        settingsButton = driver.find_element_by_xpath( "//div[@qxclass='qx.ui.form.CheckBox']//div[text()='Settings...']/..")
        self.assertIsNotNone( settingsButton, "Could not find animator settings button")
        ActionChains( driver).click( settingsButton).perform()
        
        # Find the jump radio button in the settings and click it
        jumpButton = driver.find_element_by_xpath( "//div[@qxclass='qx.ui.form.RadioButton']/div[text()='Jump']/following-sibling::div")
        self.assertIsNotNone( jumpButton, "Could not find jump button in settings")
        # Scroll the animator window so the jump button is visible.
        driver.execute_script( "arguments[0].scrollIntoView(true);", jumpButton)
        self._setChecked( driver, jumpButton, True )
        
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
        
        # Find the wrap radio button in the animator settings and click it
        wrapButton = driver.find_element_by_xpath( "//div[@qxclass='qx.ui.form.RadioButton']/div[text()='Wrap']/following-sibling::div")
        self.assertIsNotNone( wrapButton, "Could not find wrap button in settings")
        self._setChecked( driver, wrapButton, True )
        
        # Click the restore sessions button
        self._clickSessionButton( driver )
        self._clickSessionRestoreButton( driver )
        
        # Select tSnapshotPreferences in the restore combo box
        self._selectRestoreSnapshot( driver, "tSnapshotPreferences")
        
        # Hit the restore button
        self._restoreSnapshot( driver )
        
        # Close the restore dialog
        self._closeRestore( driver )
        
        # Verify the animator jump end behavior is checked
        jumpButton = driver.find_element_by_xpath( "//div[@qxclass='qx.ui.form.RadioButton']/div[text()='Jump']/following-sibling::div")
        self.assertIsNotNone( jumpButton, "Could not find jump button in settings")
        jumpClass = jumpButton.get_attribute( "class")
        print "jumpChecked=", jumpClass
        self.assertEqual( jumpClass, "qx-radiobutton-checked","Jump end behavior is not checked")

    # Set hide the toolbar from the preferences menu.  Save a preference snapshot.  
    # Show the toolbar.  Restore the preference snapshot.
    # Check that the toolbar is hidden.
    def test_toolbar_hide(self):    
        driver = self.driver
        
        # Find the preferences button on the menu bar and click it.
        menuBar = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Menu.MenuBar']")
        self.assertIsNotNone( menuBar, "Could not find the menu bar")
        preferencesButton = driver.find_element_by_xpath("//div[text()='Preferences']/..")
        self.assertIsNotNone( preferencesButton, "Could not find div with text Preferences")
        ActionChains(driver).click(preferencesButton).perform()
        
        # Click the show button on the sub menu.
        showButton = driver.find_element_by_xpath("//div/div[text()='Show']/..")
        self.assertIsNotNone(showButton, "Could not click open button on data subcontext menu.")
        ActionChains(driver).click( showButton).perform()
        showToolButton = driver.find_element_by_xpath( "//div[text()='Show Tool Bar']/..")
        self.assertIsNotNone( showToolButton, "Could not find show tool button")
        ActionChains(driver).click( showToolButton).perform()
        
        # Verify the toolbar is NOT visible
        toolVisible = self._isToolbarVisible( driver )
        self.assertFalse( toolVisible, "Tool bar was not hidden")
        
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
        
        # Show the toolbar
        # Find the preferences button on the menu bar and click it.
        preferencesButton = driver.find_element_by_xpath("//div[text()='Preferences']/..")
        self.assertIsNotNone( preferencesButton, "Could not find div with text Preferences")
        ActionChains(driver).click(preferencesButton).perform()
        
        # Click the show tool bar button on the sub menu.
        ActionChains(driver).send_keys( Keys.ARROW_DOWN ).send_keys( Keys.ARROW_RIGHT ).send_keys( Keys.ARROW_DOWN ).send_keys( Keys.ARROW_DOWN ).send_keys( Keys.ENTER ).perform()
        
        #Verify the toolbar is now visible
        toolBar = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Menu.ToolBar']")
        self.assertIsNotNone( toolBar, "Tool bar is not visible")
        
        # Click the restore sessions button
        self._clickSessionButton( driver )
        self._clickSessionRestoreButton( driver )
        
        # Select tSnapshotPreferences in the restore combo box
        self._selectRestoreSnapshot( driver, "tSnapshotPreferences")
        
        # Hit the restore button
        self._restoreSnapshot( driver )
        
        # Close the restore dialog
        self._closeRestore( driver )
        
        # Verify the toolbar is hidden again
        toolVisible = self._isToolbarVisible( driver)
        self.assertFalse( toolVisible, "Tool bar was not hidden with restore state")

