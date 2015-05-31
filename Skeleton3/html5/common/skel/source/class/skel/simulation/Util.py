import unittest
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

def animation_to_image_window( unittest, driver):
    animWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")
    unittest.assertIsNotNone( animWindow, "Could not find animation window")
    ActionChains(driver).context_click(animWindow).send_keys( Keys.ARROW_RIGHT ).send_keys(Keys.ARROW_RIGHT).send_keys( Keys.ENTER ).perform()

def get_window_count( unittest, driver):
     windowList = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayDesktop']")
     windowCount = len( windowList )
     return windowCount
 
def load_image(unittest, driver, imageName ):    
    
    # Find a window capable of loading an image.
    imageWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
           
    # Select the window
    ActionChains(driver).click(imageWindow).perform()
    
    # Click the data button
    dataButton = driver.find_element_by_xpath("//div[text()='Data']/..")
    unittest.assertIsNotNone( dataButton, "Could not click data button in the context menu")
    ActionChains(driver).click(dataButton).perform()
    
    # Look for the open button and click it to open the file dialog.
    openDataButton = driver.find_element_by_xpath("//div/div[text()='Open...']/..")
    unittest.assertIsNotNone(openDataButton, "Could not click open button on data subcontext menu.")
    ActionChains(driver).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
    #ActionChains(driver).click(openDataButton).perform()
    
    # Select the specific image
    loadButton = driver.find_element_by_id( "loadFileButton")
    imageLocator = "//div[text()='"+imageName+"']"
    fileDiv = driver.find_element_by_xpath( imageLocator )
    fileDiv.click()
    
    #Click the load button
    loadButton = driver.find_element_by_id( "loadFileButton")
    unittest.assertIsNotNone(loadButton, "Could not find load button to click")
    loadButton.click()
    
    #Now close the file dialog
    closeButton = driver.find_element_by_id( "closeFileLoadButton")
    unittest.assertIsNotNone(loadButton, "Could not find button to close the file browser")
    closeButton.click()
    
    # Check that the window is displaying an image.
    viewElement = driver.find_element_by_xpath("//div[@qxclass='skel.boundWidgets.View.View']")
    unittest.assertIsNotNone(viewElement, "Could not find view element on page.")
    imageElement = driver.find_element_by_id("pwUID0")
    unittest.assertIsNotNone(imageElement, "Could not find image on the page")
    
    