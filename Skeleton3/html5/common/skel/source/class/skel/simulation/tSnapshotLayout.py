import unittest
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

#Test that a layout snapshot can be saved/restored.
class tSnapshotLayout(unittest.TestCase):
    
    def setUp(self):
        self.driver = webdriver.Firefox()
        self.driver.get("http://localhost:8080/pureweb/app?client=html5&name=CartaSkeleton3&username=dan12&password=Cameron21")
        self.driver.implicitly_wait(10)
        
    # Set the checked status of the checkbox.
    def _setChecked(self, checkBox, checked):
        styleAtt = checkBox.get_attribute( "style");
        print "Style", styleAtt
        oldChecked = False
        if "checked.png" in styleAtt:
            print "Clipping checked"
            oldChecked = True
        if checked != oldChecked :
            checkParent = checkBox.find_element_by_xpath( '..')
            ActionChains(driver).click( checkParent ).perform()
            
    def _clickSessionButton(self, driver ):
        # Find the session button on the menu bar and click it.
        sessionButton = driver.find_element_by_xpath("//div[text()='Session']/..")
        self.assertIsNotNone( sessionButton, "Could not find div with text session")
        ActionChains(driver).click(sessionButton).perform()
        
    # Take a snapshot of the analysis layout.  Change to an image layout.  Restore
    # the analysis layout
    def test_analysis_saveRestore(self):    
        driver = self.driver
        
        #For later use, determine the number of DisplayWindows.
        windowList = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayDesktop']")
        windowCount = len( windowList )
        print "Window Count=", windowCount
        
        # Find the session button on the menu bar and click it.
        self._clickSessionButton( driver )
        
        # Find the save session button in the submenu and click it.
        saveButton = driver.find_element_by_xpath( "//div[text()='Save...']/..")
        self.assertIsNotNone( saveButton, "Could not find save session button in submenu")
        ActionChains(driver).click( saveButton).perform()
        
        # The save popup should be visible.  Make sure preferences and data are not checked;
        # layout is checked.
        prefDiv = driver.find_element_by_xpath( "//div[@qxclass='qx.ui.form.CheckBox']/div[text()='Preferences']/..")
        prefCheck = prefDiv.find_element_by_xpath( "./div[@class='qx-checkbox']")
        self.assertIsNotNone( prefCheck, "Could not find preferences check")
        self._setChecked( prefCheck, False )
        dataDiv = driver.find_element_by_xpath( "//div[@qxclass='qx.ui.form.CheckBox']/div[text()='Data']/..")
        dataCheck = dataDiv.find_element_by_xpath( "./div[@class='qx-checkbox']")
        self.assertIsNotNone( dataCheck, "Could not find data check")
        self._setChecked( dataCheck, False )
        layoutDiv = driver.find_element_by_xpath( "//div[@qxclass='qx.ui.form.CheckBox']/div[text()='Layout']/..")
        layoutCheck = layoutDiv.find_element_by_xpath( "./div[@class='qx-checkbox']")
        self.assertIsNotNone( layoutCheck, "Could not find layout check")
        self._setChecked( layoutCheck, True )
        
        # Type in tSnapshotLayout for the save name.
        snapshotSaveText = driver.find_element_by_id( "snapshotSaveName")
        self.assertIsNotNone( snapshotSaveText, "Could not find input for typing in snapshot name")
        snapshotSaveText.clear()
        snapshotSaveText.send_keys( "tSnapshotLayout")
        
        # Hit the save button
        saveSnapButton = driver.find_element_by_xpath(".//div[@qxclass='qx.ui.form.Button']/div[text()='Save']/..")
        self.assertIsNotNone( saveSnapButton, "Could not find a button to save the snapshot")
        ActionChains(driver).click( saveSnapButton).perform()
        
        # Close the dialog
        closeButton = driver.find_element_by_xpath(".//div[@qxclass='qx.ui.form.Button']/div[text()='Close']/..")
        self.assertIsNotNone( closeButton, "Could not find a button to close the snapshot popup")
        ActionChains(driver).click( closeButton).perform()
        
        # Change to an image layout
        layoutButton = driver.find_element_by_xpath("//div[text()='Layout']/..")
        self.assertIsNotNone( layoutButton, "Could not find div with text layout")
        ActionChains(driver).click(layoutButton).perform()
        imageLayoutButton = driver.find_element_by_xpath( "//div[text()='Image Layout']/..")
        self.assertIsNotNone( imageLayoutButton, "Could not find layout image button in submenu")
        ActionChains(driver).click( imageLayoutButton).perform()
        
        # Click the restore sessions button
        self._clickSessionButton( driver )
        restoreButton = driver.find_element_by_xpath( "//div[text()='Restore...']/..")
        self.assertIsNotNone( restoreButton, "Could not find restore session button in submenu")
        ActionChains(driver).click( restoreButton).perform()
        
        # Select tSnapshotLayout in the combo box
        restoreCombo = driver.find_element_by_xpath( "//div[starts-with(@id,'snapshotRestoreName')]/input")
        self.assertIsNotNone( restoreCombo, "Could not find the restore name combo")
        restoreCombo.clear()
        restoreCombo.send_keys( "tSnapshotLayout")
        
        # Hit the restore button
        restoreSnapButton = driver.find_element_by_xpath(".//div[@qxclass='qx.ui.form.Button']/div[text()='Restore']/..")
        self.assertIsNotNone( restoreSnapButton, "Could not find a button to restore the snapshot")
        ActionChains(driver).click( restoreSnapButton).perform()
        
        # Close the restore dialog
        closeButton = driver.find_element_by_xpath(".//div[@qxclass='qx.ui.form.Button']/div[text()='Close']/..")
        self.assertIsNotNone( closeButton, "Could not find a button to close the snapshot popup")
        ActionChains(driver).click( closeButton).perform()
        
        # Verify the window count is the same
        newWindowList = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayDesktop']")
        newWindowCount = len( newWindowList )
        self.assertEqual( windowCount, newWindowCount, "Window count changed with restore")

    def tearDown(self):
        print 'Test done'
        #self.driver.close()

if __name__ == "__main__":
    unittest.main()        
        
