import unittest
import Util
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

#Test of some animator functionality
class tAnimator(unittest.TestCase):
    def setUp(self):
        self.driver = webdriver.Firefox()
        self.driver.get("http://localhost:8080/pureweb/app?client=html5&name=CartaSkeleton3&username=dan12&password=Cameron21")
        self.driver.implicitly_wait(10)
        
    #Verify the number of animators that are visible is equal to the expected count
    def _verifyAnimationCount(parentWidget, expectedCount):
        animatorList = parentWidget.find_elements_by_xpath( ".//div[@qxclass='skel.boundWidgets.Animator']" )
        animatorCount = len( animatorList )
        print "Animator list count=", animatorCount
        self.assertEqual( animatorCount, expectedCount, "Animator count does not match expected count")
        
    #Return whether or not a radio button is checked
    def _isChecked(self, checkButton ):
        styleAtt = checkButton.get_attribute( "style")
        print "Style=", styleAtt
        buttonChecked = False
        if "checked.png" in styleAtt:
            buttonChecked = True
        return buttonChecked
    
    #Click the radio button
    def _click(self, driver, checkButton):
        channelParent = checkButton.find_element_by_xpath( '..')
        ActionChains(driver).click( channelParent ).perform()
           
           
    #Test that we can add the add/remove animator buttons to the toolbar if they are
    #not already there.  Then test that we can check/uncheck them and have the corresponding
    #animator added/removed.
    def test_animatorAddRemove(self):    
        driver = self.driver
        
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
        channelAnimator = driver.find_element_by_xpath( "//div[@qxclass='skel.boundWidgets.Animator']/div/div[text()='Channel']")
        self.assertIsNotNone( channelAnimator, "Channel animator did not appear")
        self._verifyAnimationCount( animationWindow, 2 )
        
        
    #Test that we can increment the animator channel by one.
    def test_animatorIncreaseFrame(self):    
        driver = self.driver
        
        #Open our test image so we have something to animate.
        origImage = "Orion.methanol.cbc.contsub.image.fits"
        Util.load_image(self, driver, origImage)
        
        #Find the increment by one button on the animator and click it
        incrementButton = driver.find_element_by_id( "ChannelTapeDeckIncrement")
        self.assertIsNotNone( incrementButton, "Could not find button to increment the channels")
        ActionChains(driver).click( incrementButton).perform()
        
        #Check that the channel text box value is now 1.
        channelText = driver.find_element_by_id( "ChannelIndexText")
        self.assertIsNotNone( channelText, "Could not find animator channel text")
        currChannel = channelText.get_attribute( "value")
        print "Channel is now", currChannel
        self.assertEqual( int(currChannel), 1, "Failed to increment the channel animator")
        
        
    def tearDown(self):
        self.driver.close()

if __name__ == "__main__":
    unittest.main()        
        
