import unittest
import selectBrowser
import Util
import time
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait

# Test that the customize dialog can be used to show/hide functionality on the
# menu and tool bars.
class tMenuToolVisibility(unittest.TestCase):
    
    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp(self, browser)
           
    def _verifyClips(self, parentWidget, expectedCount ):
        # Check that there are not clip buttons on the parentWidget by looking for labels with a %
        clipList = parentWidget.find_elements_by_xpath( ".//div[contains(text(), '%')]" )
        clipCount = len( clipList )
        print "clip list count=", clipCount
        self.assertEqual( clipCount, expectedCount, "Clip count does not match expected count")
     
    # Test that we can add a clip command to the toolbar and then remove it
    def test_tool_showHideClip(self):    
        driver = self.driver

        # Wait for the image window to be present (ensures browser is fully loaded)
        imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
    
        toolBar = driver.find_element_by_xpath( "//div[@qxclass='skel.widgets.Menu.ToolBar']")
        actionChains = ActionChains(driver)
        actionChains.context_click(toolBar).perform()
        
        # Click the customize item on the menu
        customizeButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Customize...']/..")))
        ActionChains(driver).click( customizeButton).perform()
        
        # First make sure no clips are checked
        clippingButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Clipping']/preceding-sibling::div/div")))
        styleAtt = clippingButton.get_attribute( "style");
        print "Style", styleAtt
        if "checked.png" in styleAtt:
            print "Clipping checked"
            clipParent = clippingButton.find_element_by_xpath( '..')
            ActionChains(driver).click( clipParent ).perform()
            
        # Check that there are no  clip buttons on the toolbar by looking for labels with a %
        self._verifyClips( toolBar, 0 )
        
        # Open the Clipping folder by performing 2 clicks so that the 98% clip button is visible
        # clippingTreeItem = clippingButton.find_element_by_xpath( '../..')
        clippingOpenButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Clipping']/../div[@qxclass='qx.ui.tree.core.FolderOpenButton']")))
        ActionChains( driver ).click( clippingOpenButton ).perform()
        clipOpenButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Clips']/../div[@qxclass='qx.ui.tree.core.FolderOpenButton']")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", clipOpenButton)
        ActionChains( driver ).click( clipOpenButton ).perform()
        
        # Now click the 98% clip button on the customize dialog
        clipButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='98%']/preceding-sibling::div/div/..")))
        ActionChains(driver).click( clipButton).perform()
        
        # Verify that the 98% clip appears on the tool bar
        clipRadio = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='98%']")))
        
        # Remove the 98% clip by unclicking the button on the customize dialog
        ActionChains(driver).click( clipButton).perform()
        
        # Verify the all clips are gone
        self._verifyClips( toolBar, 0 )

    # Test that we can remove clipping from appearing on the menu bar.
    def test_menu_hideClipping(self):    
        driver = self.driver
        browser = selectBrowser._getBrowser()

        # Click on an CasaImageLoader Window so that clipping is available on the menu.
        imageWindow = WebDriverWait(driver, 30).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
        ActionChains(driver).click(imageWindow).perform()

        # Verify that clipping commands can be found on the menu bar.
        menuBar = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Menu.MenuBar']")))
        clipping = menuBar.find_elements_by_xpath( "./div[contains(text(), 'Clipping')]" )

        # Now right click the toolbar
        toolBar = driver.find_element_by_xpath( "//div[@qxclass='skel.widgets.Menu.ToolBar']")
        actionChains = ActionChains(driver)
        actionChains.context_click(toolBar).perform()

        # Find and click the customize button
        customizeButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Customize...']/..")))
        ActionChains(driver).click( customizeButton).perform()
        
        # Uncheck clipping
        clippingButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Clipping']/preceding-sibling::div/div")))
        styleAtt = clippingButton.get_attribute( "style");
        print "Style", styleAtt
        if "checked.png" in styleAtt:
            print "Clipping checked"
            clipParent = clippingButton.find_element_by_xpath( '..')
            ActionChains(driver).click( clipParent ).perform()
        
        # Check that the clipping menu item is no longer available
        try: 
            clipButton = menuBar.find_element_by_xpath( "./div[contains(text(), 'Clipping')]" )
            self.assertTrue( False, "Should not be able to locate clipping button")
        except Exception:
            print "Test succeeded because we should not be able to find clipping" 

    def tearDown(self):
        # Close the browser
        self.driver.close()
        # Allow browser to fully close before continuing
        time.sleep(2)
        # Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()      
        
