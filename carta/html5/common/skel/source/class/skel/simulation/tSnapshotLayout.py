import tSnapshot
import Util
import time
import unittest
import selectBrowser
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.by import By

#Test that a layout snapshot can be saved/restored.
class tSnapshotLayout(tSnapshot.tSnapshot):
        
    # Take a snapshot of the analysis layout.  Change to an image layout.  Restore
    # the analysis layout
    def test_analysis_saveRestore(self):    
        driver = self.driver
        timeout = selectBrowser._getSleep()

        #For later use, determine the number of DisplayWindows.
        windowList = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayDesktop']")
        windowCount = len( windowList )
        print "Window Count=", windowCount

        # Find the session button on the menu bar and click it.
        self._clickSessionButton( driver )
        
        # Find the save session button in the submenu and click it.
        self._clickSessionSaveButton( driver )
        
        # The save popup should be visible.  Make sure preferences and data are not checked;
        # layout is checked.
        self._setSaveOptions( driver, False, True, False)
        
        # Type in tSnapshotLayout for the save name.
        self._setSaveName( driver, "tSnapshotLayout")
        
        # Hit the save button
        self._saveSnapshot( driver )
        
        # Close the dialog
        self._closeSave( driver )
        
        # Change to an image layout
        layoutButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Layout']/..")))
        ActionChains(driver).click(layoutButton).perform()
        imageLayoutButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Image Layout']/..")))
        ActionChains(driver).click( imageLayoutButton).perform()
        
        # Click the restore sessions button
        self._clickSessionButton( driver )
        self._clickSessionRestoreButton( driver )
        
        # Select tSnapshotLayout in the combo box
        self._selectRestoreSnapshot( driver, "tSnapshotLayout")
        
        # Hit the restore button
        self._restoreSnapshot( driver )
        
        # Close the restore dialog
        self._closeRestore( driver )
        time.sleep( timeout )

        # Verify the window count is the same
        newWindowList = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayDesktop']")
        newWindowCount = len( newWindowList )
        print "New Window Count=", newWindowCount
        self.assertEqual( windowCount, newWindowCount, "Window count changed with restore")

if __name__ == "__main__":
    unittest.main()   