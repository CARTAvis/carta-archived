import unittest
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

#Test that the customize dialog can be used to show/hide functionality on the
#menu and tool bars.
class tMenuToolVisibility(unittest.TestCase):
    
    def setUp(self):
        self.driver = webdriver.Firefox()
        self.driver.get("http://localhost:8080/pureweb/app?client=html5&name=CartaSkeleton3&username=dan12&password=Cameron21")
        self.driver.implicitly_wait(10)
           
    def _verifyClips(self, parentWidget, expectedCount ):
        #Check that there are not clip buttons on the parentWidget by looking for labels with a %
        clipList = parentWidget.find_elements_by_xpath( ".//div[contains(text(), '%')]" )
        clipCount = len( clipList )
        print "clip list count=", clipCount
        self.assertEqual( clipCount, expectedCount, "Clip count does not match expected count")
           
    #Test that we can add a clip command to the toolbar and then remove it
    def test_tool_showHideClip(self):    
        driver = self.driver
        
        # Right click on the toolbar to bring up the context menu.
        toolBar = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Menu.ToolBar']")
        self.assertIsNotNone( toolBar, "Could not find the tool bar")
        actionChains = ActionChains(driver)
        actionChains.context_click(toolBar).perform()
        
        #Click the customize item on the menu
        customizeButton = driver.find_element_by_xpath( "//div[text()='Customize...']/..")
        self.assertIsNotNone( customizeButton, "Could not find the customize button in context")
        ActionChains(driver).click( customizeButton).perform()
        
        #First make sure no clips are checked
        clippingButton = driver.find_element_by_xpath( "//div[text()='Clipping']/preceding-sibling::div/div")
        self.assertIsNotNone( clippingButton, "Could not clipping button in customize dialog")
        styleAtt = clippingButton.get_attribute( "style");
        print "Style", styleAtt
        if "checked.png" in styleAtt:
            print "Clipping checked"
            clipParent = clippingButton.find_element_by_xpath( '..')
            ActionChains(driver).click( clipParent ).perform()
            
        #Check that there are not clip buttons on the toolbar by looking for labels with a %
        self._verifyClips( toolBar, 0 )
        
        #Open the Clipping folder by performing 2 clicks so that the 98% clip button is visible
        #clippingTreeItem = clippingButton.find_element_by_xpath( '../..')
        clippingOpenButton = driver.find_element_by_xpath( "//div[text()='Clipping']/../div[@qxclass='qx.ui.tree.core.FolderOpenButton']")
        self.assertIsNotNone( clippingOpenButton, "Could not find open folder button for clipping")
        ActionChains( driver ).click( clippingOpenButton ).perform()
        clipOpenButton = driver.find_element_by_xpath( "//div[text()='Clips']/../div[@qxclass='qx.ui.tree.core.FolderOpenButton']")
        self.assertIsNotNone( clipOpenButton, "Could not find open folder button for clip")
        ActionChains( driver ).click( clipOpenButton ).perform()
        
        #Now click the 98% clip button on the customize dialog
        clipButton = driver.find_element_by_xpath( "//div[text()='98%']/preceding-sibling::div/div/..")
        self.assertIsNotNone( clipButton, "Could not find clip button in customize dialog")
        ActionChains(driver).click( clipButton).perform()
        
        #Verify that the 98% clip appears on the tool bar
        clipRadio = toolBar.find_element_by_xpath( "//div[text()='98%']")
        self.assertIsNotNone( clipRadio, "Clip radio did not appear on tool bar")
        
        #Remove the 98% clip by unclicking the button on the customize dialog
        ActionChains(driver).click( clipButton).perform()
        
        #Verify the all clips are gone
        self._verifyClips( toolBar, 0 )
        
        
    #Test that we can remove clipping from appearing on the menu bar.
    def test_menu_hideClipping(self):    
        driver = self.driver
        
        # Click on an CasaImageLoader Window so that clipping is available on the menu.
        imageWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        ActionChains(driver).click(imageWindow).perform()
        
        # Verify that clipping commands can be found on the menu bar.
        menuBar = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Menu.MenuBar']")
        self.assertIsNotNone( menuBar, "Could not find the menu bar")
        clipping = menuBar.find_elements_by_xpath( "./div[contains(text(), 'Clipping')]" )
        self.assertIsNotNone( clipping, "Clipping not present on menu bar");
        
        #Open the context menu by right clicking the menu bar
        actionChains = ActionChains(driver)
        actionChains.context_click(menuBar).perform()
        
        #Click the customize item on the menu
        customizeButton = driver.find_element_by_xpath( "//div[text()='Customize...']/..")
        self.assertIsNotNone( customizeButton, "Could not find the customize button in context")
        ActionChains(driver).click( customizeButton).perform()
        
        #Uncheck clipping
        clippingButton = driver.find_element_by_xpath( "//div[text()='Clipping']/preceding-sibling::div/div")
        self.assertIsNotNone( clippingButton, "Could not clipping button in customize dialog")
        styleAtt = clippingButton.get_attribute( "style");
        print "Style", styleAtt
        if "checked.png" in styleAtt:
            print "Clipping checked"
            clipParent = clippingButton.find_element_by_xpath( '..')
            ActionChains(driver).click( clipParent ).perform()
            
        #Check that the clipping menu item is no longer available
        try: 
            clipButton = menuBar.find_element_by_xpath( "./div[contains(text(), 'Clipping')]" )
            self.assertTrue( False, "Should not be able to locate clipping button")
        except Exception:
            print "Test succeeded because we should not be able to find clipping"
    
        
    def tearDown(self):
        self.driver.close()

if __name__ == "__main__":
    unittest.main()        
        
