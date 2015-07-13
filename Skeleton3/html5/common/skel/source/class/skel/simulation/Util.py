import unittest
import time
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

# Adjust hyperlinks as necessary 
def setUp(self, browser):
    # Running on Ubuntu (Firefox)
    if browser == 1:
        self.driver = webdriver.Firefox()
        self.driver.get("http://localhost:8080/pureweb/app?client=html5&name=CartaSkeleton3&username=dan12&password=Cameron21")
        self.driver.implicitly_wait(10)

    # Running on Mac (Chrome)
    if browser == 2:
        # Change the path to where chromedriver is located
        chromedriver = "/Users/Madhatter/Downloads/chromedriver"
        self.driver = webdriver.Chrome(chromedriver)
        self.driver.get("http://199.116.235.162:8080/pureweb/app/unix:1.0/3/9737/6?client=html5&file=/home/ubuntu/Dropbox/CARTA-data/&name=CartaSkeleton3")
        time.sleep(5)
        self.driver.implicitly_wait(20)
        
# Clear text box and change value of the text box
# Selenium clear() method is sometimes unable to clear 
# default text, therefore a manual method has been implemented
def _changeElementText(self, driver, elementText, inputValue):
    # First click on the element and get the element value
    ActionChains(driver).click( elementText )
    elementValue = elementText.get_attribute("value")
    # Clear the content in the text box
    for x in range(0, len(str(elementValue))):
        elementText.send_keys( Keys.BACK_SPACE )
    # Enter a new value into the text box and wait for element to update value
    # otherwise Google Chrome browser will not receive the correct value
    elementText.send_keys( inputValue )
    elementText.send_keys( Keys.ENTER )
    time.sleep(4) 
    # Get the new element value
    elementValue = elementText.get_attribute("value")
    return elementValue

def animation_to_image_window(unittest, driver):
    animWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")
    unittest.assertIsNotNone( animWindow, "Could not find animation window")
    ActionChains(driver).click( animWindow ).perform()
        
    # Make sure the animation window is enabled by clicking an element within the window
    # Chrome browser is unable to enable the animation window by clicking on the window
    channelText = driver.find_element_by_id("ChannelIndexText")
    ActionChains(driver).click( channelText).perform()
    ActionChains(driver).context_click(channelText).send_keys( Keys.ARROW_RIGHT ).send_keys(Keys.ARROW_RIGHT).send_keys( Keys.ENTER ).perform()
    time.sleep(2)

def get_window_count(unittest, driver):
     windowList = driver.find_elements_by_xpath("//div[@class='qx-window']")
     windowCount = len( windowList )
     return windowCount

#Set a custom layout with the given number of rows and columns
def layout_custom(self, driver, rows, cols ):
    
    # Find the layout button on the menu bar and click it.
    self._clickLayoutButton( driver )
    
    # Find the layout custom button in the submenu and click it.
    customLayoutButton = driver.find_element_by_xpath( "//div[text()='Custom Layout']/..")
    self.assertIsNotNone( customLayoutButton, "Could not find custom layout button in submenu")
    ActionChains(driver).click( customLayoutButton).perform()
    
    #Get the row count spin and set its value.
    rowSpin = driver.find_element_by_xpath( "//div[starts-with(@id,'customLayoutRows')]/input")
    self.assertIsNotNone( rowSpin, "Could not find custom layout row indicator")
    rowSpin.send_keys( Keys.BACK_SPACE )
    rowSpin.send_keys(str(rows))
    
    #Get the column count spin and set its value.
    colSpin = driver.find_element_by_xpath( "//div[starts-with(@id,'customLayoutCols')]/input")
    self.assertIsNotNone( colSpin, "Could not find custom layout column indicator")
    colSpin.send_keys(str(cols))
    colSpin.send_keys(Keys.ARROW_LEFT)
    colSpin.send_keys( Keys.BACK_SPACE )
    
    #Hit the ok button
    okButton = driver.find_element_by_xpath( "//div[starts-with(@id,'customLayoutOK')]")
    self.assertIsNotNone( okButton, "Could not find custom layout ok button")
    ActionChains(driver).click(okButton).perform()


# Load an image
def load_image(unittest, driver, imageName): 
    # Allow browser to fully load before continuing
    time.sleep(10)

    # If image is not specified, load default image
    # Change this to a test image available where the tests are running
    # Test image will ideally have more than 3 channels for a successful test run
    if imageName == "Default":
        imageName = "N15693D.fits"

    # Find a window capable of loading an image and select the window
    imageWindow = driver.find_element_by_id( "CasaImageLoader")
    ActionChains(driver).click(imageWindow).perform()
    time.sleep(10)

    # Click the data button
    dataButton = driver.find_element_by_xpath("//div[text()='Data']/..")
    unittest.assertIsNotNone( dataButton, "Could not click data button in the context menu")
    ActionChains(driver).click(dataButton).perform()
    time.sleep(5)
    
    # Look for the open button and click it to open the file dialog.
    openDataButton = driver.find_element_by_xpath("//div/div[text()='Open...']/..")
    unittest.assertIsNotNone(openDataButton, "Could not click open button on data subcontext menu.")
    ActionChains(driver).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()
    #ActionChains(driver).click(openDataButton).perform()
    time.sleep(5)

    # Getting element not found in cache without this.
    driver.implicitly_wait(20) 

    # Select the specific image
    loadButton = driver.find_element_by_id( "loadFileButton")
    imageLocator = "//div[text()='"+imageName+"']"
    fileDiv = driver.find_element_by_xpath( imageLocator )
    driver.execute_script( "arguments[0].scrollIntoView(true);", fileDiv)
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
    time.sleep(5)

# Add a window, convert the window to an image window
# Use the new image window to load an image
def load_image_different_window(unittest, driver, imageName):
    # Allow browser to fully load before continuing
    time.sleep(10)
    
    # Find and click on the image window  
    imageWindow = driver.find_element_by_id( "CasaImageLoader")
    unittest.assertIsNotNone( imageWindow, "Could not find image display window")
    ActionChains(driver).click( imageWindow ).perform()

    # Click the Window button
    windowButton = driver.find_element_by_xpath( "//div[text()='Window']/..")
    unittest.assertIsNotNone( windowButton, "Could not click window button in the context menu")
    ActionChains(driver).click(windowButton).perform()
    
    # Look for the add button in the submenu.
    addButton = driver.find_element_by_xpath( "//div/div[text()='Add']/..")
    unittest.assertIsNotNone( addButton, "Could not click minimize button on window subcontext menu.")
    ActionChains(driver).click( addButton ).perform()
    
    # Choose to add at the bottom
    ActionChains( driver).send_keys( Keys.ARROW_RIGHT ).send_keys( Keys.ENTER ).perform()
    time.sleep(2)

    # Check that we now have a generic empty window in the display and that the window count has gone up by one.
    emptyWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowGenericPlugin']")
    unittest.assertIsNotNone( emptyWindow, "Could not find empty display window")

    # Select the empty window
    ActionChains(driver).click( emptyWindow ).perform()
    
    # Change the plugin of the empty window to an image loader 
    ActionChains(driver).context_click( emptyWindow ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()
    time.sleep(2)

    # Getting element not found in cache without this.
    driver.implicitly_wait(10) 

    imageWindow2 = driver.find_element_by_xpath("//div[@id='CasaImageLoader2']")
    unittest.assertIsNotNone( imageWindow2, "Could not find second image window")

    # Load separate image in the window
    ActionChains(driver).context_click(imageWindow2).send_keys(Keys.ARROW_DOWN).send_keys(
        Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(
        Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()

    # Getting element not found in cache without this.
    time.sleep(2)
    driver.implicitly_wait(10)

    # Select the specific image
    loadButton = driver.find_element_by_id( "loadFileButton")
    imageLocator = "//div[text()='"+imageName+"']"
    fileDiv = driver.find_element_by_xpath( imageLocator )
    driver.execute_script( "arguments[0].scrollIntoView(true);", fileDiv)
    fileDiv.click()
    
    # Click the load button
    loadButton = driver.find_element_by_id( "loadFileButton")
    unittest.assertIsNotNone(loadButton, "Could not find load button to click")
    loadButton.click()
    
    # Now close the file dialog
    closeButton = driver.find_element_by_id( "closeFileLoadButton")
    unittest.assertIsNotNone(closeButton, "Could not find button to close the file browser")
    closeButton.click()
    
    # Check that the window is displaying an image.
    viewElement = driver.find_element_by_xpath("//div[@qxclass='skel.boundWidgets.View.View']")
    unittest.assertIsNotNone(viewElement, "Could not find view element on page.")
    imageElement = driver.find_element_by_id("pwUID0")
    unittest.assertIsNotNone(imageElement, "Could not find image on the page")

    # Return the second image loader window for further linking tests
    return imageWindow2