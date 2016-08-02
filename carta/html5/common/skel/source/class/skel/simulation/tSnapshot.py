import unittest
import selectBrowser
import Util
import time
from selenium import webdriver
from flaky import flaky
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.by import By

#Test that a layout snapshot can be saved/restored.
@flaky(max_runs=3)
class tSnapshot(unittest.TestCase):

    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp(self, browser)



    # Set the checked status of the checkbox.
    def _setChecked(self, driver, checkBox, checked):
        oldChecked = Util.isChecked( self, checkBox )
        if checked != oldChecked :
            ActionChains(driver).click( checkBox ).perform()

    # Click the Restore... option in the Sessions submenu
    def _clickSessionRestoreButton(self,driver):
        ActionChains(driver).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

    # Click the Save... option in the Sessions submenu
    def _clickSessionSaveButton(self,driver):
        sessionButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Session']/..")))
        ActionChains( driver ).click( sessionButton );
        ActionChains( driver ).send_keys(Keys.ARROW_DOWN).send_keys( Keys.ENTER ).perform()

    # Click the "Sessions" menu item
    def _clickSessionButton(self, driver ):
        # Find the session button on the menu bar and click it.
        sessionButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Session']/..")))
        ActionChains(driver).click(sessionButton).perform()

    # Close the save session pop-up
    def _closeSave(self, driver):
        closeButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, ".//div[@qxclass='qx.ui.form.Button']/div[text()='Close']/..")))
        ActionChains(driver).click( closeButton).perform()

    # Close the restore session pop-up
    def _closeRestore(self, driver):
        closeButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, ".//div[@qxclass='qx.ui.form.Button']/div[text()='Close']/..")))
        ActionChains(driver).click( closeButton).perform()

    # Select the name of the snapshot to restore
    def _selectRestoreSnapshot(self, driver, restoreName):
        tableRowLocator = "//div[text()='" + restoreName+ "']/.."
        tableRow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, tableRowLocator)))
        ActionChains(driver).click( tableRow ).perform()

    # Set the type of state that should be saved
    def _setSaveOptions(self, driver, savePreferences, saveLayout, saveData ):
        # Find preferences div
        prefCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.CheckBox']/div[text()='Preferences']/../div[@class='qx-checkbox']")))
        self._setChecked( driver, prefCheck, savePreferences )
        # Find data div
        dataDiv = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.CheckBox']/div[text()='Session']/..")))
        dataCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.CheckBox']/div[text()='Session']/../div[@class='qx-checkbox']")))
        self._setChecked( driver, dataCheck, saveData )
        # Find layout div
        layoutCheck = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.CheckBox']/div[text()='Layout']/following-sibling::div")))
        self._setChecked( driver, layoutCheck, saveLayout )

    # Set the name of the session to save.
    def _setSaveName(self, driver, saveName):
        snapshotSaveText = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "snapshotSaveName")))
        snapshotSaveText.clear()
        snapshotSaveText.send_keys( saveName )

    # Save the snapshot.
    def _saveSnapshot(self, driver ):
        saveSnapButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, ".//div[@qxclass='qx.ui.form.Button']/div[text()='Save']/..")))
        ActionChains(driver).click( saveSnapButton).perform()

    # Restore the snapshot
    def _restoreSnapshot(self, driver ):
        restoreSnapButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, ".//div[@qxclass='qx.ui.form.Button']/div[text()='Restore']/..")))
        ActionChains(driver).click( restoreSnapButton).perform()

    def tearDown(self):
        #Close the browser
        self.driver.close()
        #Allow browser to fully close before continuing
        time.sleep(2)
        #Close the session and delete temporary files
        self.driver.quit()
