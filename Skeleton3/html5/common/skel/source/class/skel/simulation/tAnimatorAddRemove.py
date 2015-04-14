import unittest
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

#Test that we can add the add/remove animator buttons to the toolbar if they are
#not already there.  Then test that we can check/uncheck them and have the corresponding
#animator added/removed.
class tAnimatorAddRemove(unittest.TestCase):
    def setUp(self):
        self.driver = webdriver.Firefox()
        
    def _verifyAnimationCount(self, parentWidget, expectedCount):
        animatorList = parentWidget.find_elements_by_xpath( ".//div[@qxclass='skel.boundWidgets.Animator']" )
        animatorCount = len( animatorList )
        print "Animator list count=", animatorCount
        self.assertEqual( animatorCount, expectedCount, "Animator count does not match expected count")
        
    def _isChecked(self, checkButton ):
        styleAtt = checkButton.get_attribute( "style")
        print "Style=", styleAtt
        buttonChecked = False
        if "checked.png" in styleAtt:
            buttonChecked = True
        return buttonChecked
    
    def _click(self, driver, checkButton):
        channelParent = checkButton.find_element_by_xpath( '..')
        ActionChains(driver).click( channelParent ).perform()
           
    def test_animator(self):    
        driver = self.driver
        driver.get("http://localhost:8080/pureweb/app?client=html5&name=CartaSkeleton3&username=dan12&password=Cameron21")
        driver.implicitly_wait(10)
        
        #Click on the animation window so that its actions will be enabled 
        animationWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")
        self.assertIsNotNone( animationWindow, "Could not find animation window")
        ActionChains(driver).click( animationWindow ).perform()
        
         # Right click on the toolbar to bring up the context menu.
        toolBar = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Menu.ToolBar']")
        self.assertIsNotNone( toolBar, "Could not find the tool bar")
        actionChains = ActionChains(driver)
        actionChains.context_click(toolBar).perform()
        
        #Click the customize item on the menu
        customizeButton = driver.find_element_by_xpath( "//div[text()='Customize...']/..")
        self.assertIsNotNone( customizeButton, "Could not find the customize button in context")
        ActionChains(driver).click( customizeButton).perform()
        
        #First make sure animator is checked
        animateButton = driver.find_element_by_xpath( "//div[text()='Animate']/preceding-sibling::div/div")
        self.assertIsNotNone( animateButton, "Could not find animate button in customize dialog")
        styleAtt = animateButton.get_attribute( "style");
        if not "checked.png" in styleAtt:
            print "Clicking animate to make buttons visible on tool bar"
            animateParent = animateButton.find_element_by_xpath( '..')
            ActionChains(driver).click( animateParent ).perform()
        
        #Verify both the channel and image checkboxes are on the toolbar
        channelCheck = driver.find_element_by_xpath( "//div[text()='Channel']/following-sibling::div[@class='qx-checkbox']")
        self.assertIsNotNone( channelCheck, "Could not find animate channel check box on tool bar")
        animateCheck = driver.find_element_by_xpath( "//div[text()='Image']/following-sibling::div[@class='qx-checkbox']")
        self.assertIsNotNone( animateCheck, "Could not find animate image check box on tool bar")
        
        #Uncheck both buttons
        channelChecked = self._isChecked( channelCheck )
        print 'Channel checked', channelChecked
        if channelChecked:
            self._click( driver, channelCheck )
        animateChecked = self._isChecked( animateCheck )
        print 'Animate checked', animateChecked
        if animateChecked:
            self._click( driver, animateCheck )
            
        #verify that the animation window has no animators.
        self._verifyAnimationCount( animationWindow, 0)
        
        #Check the image animate button and verify that the image animator shows up
        self._click( driver, animateCheck )
        imageAnimator = driver.find_element_by_xpath( "//div[@qxclass='skel.boundWidgets.Animator']/div/div[text()='Image']")
        self.assertIsNotNone( imageAnimator, "Image animator did not appear")
        self._verifyAnimationCount( animationWindow, 1)
        
        #Check the channel animator button and verify there are now two animators, one channel, one image.
        self._click( driver, channelCheck )
        #imageAnimator = driver.find_element_by_xpath( "//div[@qxclass='skel.boundWidgets.Animator']/div/div[text()='Image']")
        #self.assertIsNotNone( imageAnimator, "Image animator did not appear")
        channelAnimator = driver.find_element_by_xpath( "//div[@qxclass='skel.boundWidgets.Animator']/div/div[text()='Channel']")
        self.assertIsNotNone( channelAnimator, "Channel animator did not appear")
        self._verifyAnimationCount( animationWindow, 2 )
        
        
    def tearDown(self):
        self.driver.close()

if __name__ == "__main__":
    unittest.main()        
        
