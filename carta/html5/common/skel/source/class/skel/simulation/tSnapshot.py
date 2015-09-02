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

#Test that a layout snapshot can be saved/restored.
class tSnapshot(unittest.TestCase):
    
    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp(self, browser)
        
    # Determine whether the check box is checked
    def _isChecked(self, checkBox):
        styleAtt = checkBox.get_attribute( "style");
        #print "Style", styleAtt
        oldChecked = False
        if "checked.png" in styleAtt:
            oldChecked = True
        return oldChecked
    
    # Set the checked status of the checkbox.
    def _setChecked(self, driver, checkBox, checked):
        oldChecked = self._isChecked( checkBox )
        if checked != oldChecked :
            checkParent = checkBox.find_element_by_xpath( '..')
            ActionChains(driver).click( checkParent ).perform()
            
    # Click the Restore... option in the Sessions submenu
    def _clickSessionRestoreButton(self,driver):
        restoreButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Manage/Restore...']/..")))
        self.assertIsNotNone( restoreButton, "Could not find restore session button in submenu")
        ActionChains(driver).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

    # Click the Save... option in the Sessions submenu
    def _clickSessionSaveButton(self,driver): 
        sessionButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Session']/..")))
        self.assertIsNotNone( sessionButton, "Could not find div with text session")
        ActionChains( driver ).click( sessionButton );
        # Find the save session button in the submenu and click it.
        saveButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Save...']/..")))
        self.assertIsNotNone( saveButton, "Could not find save session button in submenu")
        ActionChains(driver).click( saveButton).perform()
        #ActionChains( driver).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys( Keys.ENTER).perform()
            
    # Click the "Sessions" menu item
    def _clickSessionButton(self, driver ):
        # Find the session button on the menu bar and click it.
        sessionButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Session']/..")))
        self.assertIsNotNone( sessionButton, "Could not find div with text session")
        ActionChains(driver).click(sessionButton).perform()
        
    # Close the save session pop-up
    def _closeSave(self, driver):
        closeButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, ".//div[@qxclass='qx.ui.form.Button']/div[text()='Close']/..")))
        self.assertIsNotNone( closeButton, "Could not find a button to close the snapshot popup")
        ActionChains(driver).click( closeButton).perform()
        
    # Close the restore session pop-up
    def _closeRestore(self, driver):
        closeButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, ".//div[@qxclass='qx.ui.form.Button']/div[text()='Close']/..")))
        self.assertIsNotNone( closeButton, "Could not find a button to close the snapshot popup")
        ActionChains(driver).click( closeButton).perform()
        
    # Select the name of the snapshot to restore
    def _selectRestoreSnapshot(self, driver, restoreName):
        tableRowLocator = "//div[text()='" + restoreName+ "']/.."
        tableRow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, tableRowLocator)))
        self.assertIsNotNone( tableRow, "Could not find the restore name combo")
        ActionChains(driver).click( tableRow ).perform()
    
    # Set the type of state that should be saved
    def _setSaveOptions(self, driver, savePreferences, saveLayout, saveData ):
        # Find preferences div 
        #prefDiv = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.CheckBox']/div[text()='Preferences']/..")))
        prefCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.CheckBox']/div[text()='Preferences']/../div[@class='qx-checkbox']")))
        self.assertIsNotNone( prefCheck, "Could not find preferences check")
        self._setChecked( driver, prefCheck, savePreferences )
        # Find data div
        dataDiv = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.CheckBox']/div[text()='Session']/..")))
        self.assertIsNotNone( dataDiv, "Could not find data div")
        dataCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.CheckBox']/div[text()='Session']/../div[@class='qx-checkbox']")))
        self.assertIsNotNone( dataCheck, "Could not find data check")
        self._setChecked( driver, dataCheck, saveData )
        # Find layout div
        layoutCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.CheckBox']/div[text()='Layout']/following-sibling::div")))
        self.assertIsNotNone( layoutCheck, "Could not find layout check")
        self._setChecked( driver, layoutCheck, saveLayout )
        
    # Set the name of the session to save.
    def _setSaveName(self, driver, saveName):
        snapshotSaveText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "snapshotSaveName")))
        self.assertIsNotNone( snapshotSaveText, "Could not find input for typing in snapshot name")
        snapshotSaveText.clear()
        snapshotSaveText.send_keys( saveName )
        
    # Save the snapshot.
    def _saveSnapshot(self, driver ):
        saveSnapButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, ".//div[@qxclass='qx.ui.form.Button']/div[text()='Save']/..")))
        self.assertIsNotNone( saveSnapButton, "Could not find a button to save the snapshot")
        ActionChains(driver).click( saveSnapButton).perform()

    # Restore the snapshot
    def _restoreSnapshot(self, driver ):
        restoreSnapButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, ".//div[@qxclass='qx.ui.form.Button']/div[text()='Restore']/..")))
        self.assertIsNotNone( restoreSnapButton, "Could not find a button to restore the snapshot")
        ActionChains(driver).click( restoreSnapButton).perform()
          
    def tearDown(self):
        #Close the browser
        self.driver.close()
        #Allow browser to fully close before continuing
        time.sleep(2)
        #Close the session and delete temporary files
        self.driver.quit()
