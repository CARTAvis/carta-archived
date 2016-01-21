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


# Animator functions
@flaky(max_runs=3)
class tAnimator(unittest.TestCase):



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

    # Go to the first valid value
    def _getFirstValue(self, driver, animator):
        timeout = selectBrowser._getSleep()
        firstValueButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, animator+"TapeDeckFirstValue")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", firstValueButton)
        ActionChains(driver).click( firstValueButton ).perform()
        time.sleep( timeout )

    # Go to the last valid value
    def _getLastValue(self, driver, animator):
        timeout = selectBrowser._getSleep()
        lastValueButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, animator+"TapeDeckLastValue")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", lastValueButton)
        ActionChains(driver).click( lastValueButton ).perform()
        time.sleep( timeout )

    # Get the current value
    def _getCurrentValue(self, driver, animator):
        channelText = driver.find_element_by_id( animator+"IndexText")
        driver.execute_script( "arguments[0].scrollIntoView(true);", channelText)
        CurrentValue = channelText.get_attribute("value")
        return CurrentValue

    # Go to the next valid value
    def _getNextValue(self, driver, animator):
        timeout = selectBrowser._getSleep()
        incrementButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, animator+"TapeDeckIncrement")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", incrementButton)
        ActionChains(driver).click( incrementButton ).perform()
        time.sleep( timeout )

    # Click the forward animation button on the tape deck
    def _animateForward(self, driver, animator):
        forwardAnimateButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, animator+"TapeDeckPlayButton")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", forwardAnimateButton)
        ActionChains(driver).click( forwardAnimateButton ).perform()

    # Forward animation button on the image animator
    def _animateForwardImage(self, driver):
        forwardAnimateButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "ImageTapeDeckIncrement")))
        self.assertIsNotNone( forwardAnimateButton, "Could not find forward animation button")
        driver.execute_script( "arguments[0].scrollIntoView(true);", forwardAnimateButton)
        ActionChains(driver).click( forwardAnimateButton ).perform()
    

    # Open Settings for a particular animation window
    def _openSettings(self, driver, name):
        settingsId = name + "SettingsCheck"
        print "Settings id ", settingsId
        settingsXPath = "//div[@id='" + settingsId + "']/div[@qxclass='qx.ui.basic.Image']"
        settingsCheckBox = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, settingsXPath )))
        driver.execute_script( "arguments[0].scrollIntoView(true);", settingsCheckBox)
        ActionChains(driver).click( settingsCheckBox ).perform()
        
        
    # Click the stop button on the tapedeck
    def _stopAnimation(self, driver, animator):
        stopButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, animator+"TapeDeckStopAnimation")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", stopButton)
        ActionChains(driver).click( stopButton ).perform()