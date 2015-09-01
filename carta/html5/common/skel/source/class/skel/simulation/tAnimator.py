import unittest
import Util
import selectBrowser
import time
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.by import By


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
    
    # Go to the first channel value of the test image
    def _getFirstValue(self, driver):
        timeout = selectBrowser._getSleep()
        firstValueButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@class='qx-toolbar']/div[@qxclass='qx.ui.toolbar.Button'][1]"))) 
        self.assertIsNotNone( firstValueButton, "Could not find button to go to the first valid value")
        driver.execute_script( "arguments[0].scrollIntoView(true);", firstValueButton)
        ActionChains(driver).click( firstValueButton ).perform()
        time.sleep( timeout )

    # Go to the last channel value of the test image 
    def _getLastValue(self, driver):    
        timeout = selectBrowser._getSleep()
        lastValueButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@class='qx-toolbar']/div[@qxclass='qx.ui.toolbar.Button'][5]"))) 
        self.assertIsNotNone( lastValueButton, "Could not find button to go to the last valid value")
        driver.execute_script( "arguments[0].scrollIntoView(true);", lastValueButton)
        ActionChains(driver).click( lastValueButton ).perform()
        time.sleep( timeout )

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
        forwardAnimateButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@class='qx-toolbar']/div[@class='qx-button'][2]"))) 
        self.assertIsNotNone( forwardAnimateButton, "Could not find forward animation button")
        driver.execute_script( "arguments[0].scrollIntoView(true);", forwardAnimateButton)
        ActionChains(driver).click( forwardAnimateButton ).perform()
        
    # Forward animation button on the image animator
    def _animateForwardImage(self, driver):
        forwardAnimateButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ImageTapeDeckIncrement"))) 
        self.assertIsNotNone( forwardAnimateButton, "Could not find forward animation button")
        driver.execute_script( "arguments[0].scrollIntoView(true);", forwardAnimateButton)
        ActionChains(driver).click( forwardAnimateButton ).perform()


    # Change the Channel Animator to an Image Animator
    def channel_to_image_animator(self, driver):
        timeout = selectBrowser._getSleep()
        # Find and click on the animation window
        animWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']"))) 
        self.assertIsNotNone( animWindow, "Could not find animation window")
        ActionChains(driver).click( animWindow ).perform()   

        # Make sure the animation window is enabled by clicking an element within the window
        # From the context menu, uncheck the Channel Animator and check the Image Animator
        channelText = driver.find_element_by_id( "ChannelIndexText")
        driver.execute_script( "arguments[0].scrollIntoView(true);", channelText)
        ActionChains(driver).click( channelText ).perform()
        ActionChains(driver).context_click( channelText ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN
            ).send_keys(Keys.ARROW_DOWN).send_keys( Keys.ARROW_DOWN ).send_keys(Keys.ARROW_DOWN
            ).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.SPACE).send_keys(Keys.ARROW_DOWN
            ).send_keys(Keys.ENTER).perform()
        time.sleep(timeout)

    # Open Settings
    def _openSettings(self, driver):
        # Click Settings to reveal animation settings
        settingsCheckBox = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.form.CheckBox']/div[text()='Settings...']")))  
        self.assertIsNotNone( settingsCheckBox, "Could not find settings check box")
        ActionChains(driver).click( settingsCheckBox ).perform()
