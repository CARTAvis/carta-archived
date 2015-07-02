import unittest
import Util
import selectBrowser
import time
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

# Animator functions
class tAnimator(unittest.TestCase):

    # Verify that the number of animators that are visible is equal to the expected count
    def _verifyAnimationCount(self, parentWidget, expectedCount):
        animatorList = parentWidget.find_elements_by_xpath( ".//div[@qxclass='skel.boundWidgets.Animator']" )
        animatorCount = len( animatorList )
        print "Animator list count=", animatorCount
        self.assertEqual( animatorCount, expectedCount, "Animator count does not match expected count")
        
    # Return whether or not a radio button is checked
    def _isChecked(self, checkButton ):
        styleAtt = checkButton.get_attribute( "style")
        print "Style=", styleAtt
        buttonChecked = False
        if "checked.png" in styleAtt:
            buttonChecked = True
        return buttonChecked
    
    # Click the radio button
    def _click(self, driver, checkButton):
        channelParent = checkButton.find_element_by_xpath( '..')
        ActionChains(driver).click( channelParent ).perform()
        time.sleep(2)
    
    # Go to the first channel value of the test image
    def _getFirstValue(self, driver):
        firstValueButton = driver.find_element_by_xpath( "//div[@class='qx-toolbar']/div[@qxclass='qx.ui.toolbar.Button'][1]")
        self.assertIsNotNone( firstValueButton, "Could not find button to go to the first valid value")
        driver.execute_script( "arguments[0].scrollIntoView(true);", firstValueButton)
        ActionChains(driver).click( firstValueButton ).perform()
        time.sleep(2)

    # Go to the last channel value of the test image 
    def _getLastValue(self, driver):    
        lastValueButton = driver.find_element_by_xpath( "//div[@class='qx-toolbar']/div[@qxclass='qx.ui.toolbar.Button'][5]")
        self.assertIsNotNone( lastValueButton, "Could not find button to go to the last valid value")
        driver.execute_script( "arguments[0].scrollIntoView(true);", lastValueButton)
        ActionChains(driver).click( lastValueButton ).perform()
        time.sleep(2)

    # Get the current channel value of the test image 
    def _getChannelValue(self, driver):
        channelText = driver.find_element_by_id( "ChannelIndexText")
        self.assertIsNotNone( channelText, "Could not find animator channel text")
        driver.execute_script( "arguments[0].scrollIntoView(true);", channelText)
        ChannelValue = channelText.get_attribute("value")
        return ChannelValue

    # Return the current image of the Image Animator
    def _getImageValue(self, driver):
        imageText = driver.find_element_by_id( "ImageIndexText")
        self.assertIsNotNone( imageText, "Could not find animator image text")
        driver.execute_script( "arguments[0].scrollIntoView(true);", imageText)
        ImageValue = imageText.get_attribute("value")
        return ImageValue 

    # Click the forward animation button 
    def _animateForward(self, driver):
        forwardAnimateButton = driver.find_element_by_xpath("//div[@class='qx-toolbar']/div[@class='qx-button'][2]")
        self.assertIsNotNone( forwardAnimateButton, "Could not find forward animation button")
        driver.execute_script( "arguments[0].scrollIntoView(true);", forwardAnimateButton)
        ActionChains(driver).click( forwardAnimateButton ).perform()

    # Change the Channel Animator to an Image Animator
    def channel_to_image_animator(self, driver):
        # Find and click on the animation window 
        animWindow = driver.find_element_by_xpath( "//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")
        self.assertIsNotNone( animWindow, "Could not find animation window")
        ActionChains(driver).click( animWindow ).perform()   

        # Make sure the animation window is enabled by clicking an element within the window
        # From the context menu, uncheck the Channel Animator and check the Image Animator
        channelText = driver.find_element_by_id( "ChannelIndexText")
        ActionChains(driver).click( channelText ).perform()
        ActionChains(driver).context_click( channelText ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys( 
            Keys.ARROW_DOWN ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.SPACE).send_keys(
            Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
        time.sleep(2)

    # Set default settings
    def _setDefaultSettings(self, driver):
        # Click Settings to reveal animation settings
        settingsCheckBox = driver.find_element_by_xpath( "//div[@qxclass='qx.ui.form.CheckBox']/div[text()='Settings...']")
        self.assertIsNotNone( settingsCheckBox, "Could not find settings check box")
        ActionChains(driver).click( settingsCheckBox ).perform()

        # Ensure the set increment is set to 1 and the rate is set to 20 (default settings)
        stepText = driver.find_element_by_xpath( "//div[@qxclass='skel.boundWidgets.Animator']/div[4]/div[2]/input")
        self.assertIsNotNone( stepText, "Could not find step increment text")
        driver.execute_script( "arguments[0].scrollIntoView(true);", stepText)
        stepValue = stepText.get_attribute("value")

        if int(stepValue) != 1:
            Util._changeElementText(self, driver, stepText, 1)
        
        rateText = driver.find_element_by_xpath(" //div[@qxclass='skel.boundWidgets.Animator']/div[4]/div[7]/input")
        self.assertIsNotNone( rateText, "Could not find rate text to set the speed of the animation")
        rateValue = rateText.get_attribute("value")
        if int(rateValue) != 20:
            Util._changeElementText(self, driver, rateText, 20)