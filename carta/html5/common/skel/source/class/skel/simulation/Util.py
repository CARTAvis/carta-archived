import unittest
import time
import selectBrowser
from selenium import webdriver
from pyvirtualdisplay import Display
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.by import By
from selenium.webdriver.chrome.options import Options

# Adjust hyperlinks as necessary
def setUp(self, browser):
    # Running on Ubuntu (Firefox)
    if browser == 1:
        # display = Display(visible=0, size=(1024, 768))
        # display.start()

        self.driver = webdriver.Firefox()
        self.driver.get("http://localhost:8080/pureweb/app?client=html5&name=CartaSkeleton3")
        #self.driver.get("http://199.116.235.164:8080/pureweb/app/unix:1.0/2/20801/2?client=html5&name=CartaSkeleton3")
        #self.driver.get("http://142.244.190.171:8080/pureweb/app/unix:0.0/4/143/1?client=html5&name=CartaSkeleton3")
        self.driver.implicitly_wait(20)

    # Running on Mac (Chrome)
    if browser == 2:
        # Change the path to where chromedriver is located
        # chromedriver = "/usr/local/bin/chromedriver"
        # display = Display(visible=0, size=(1024, 768))
        # display.start()

        options = Options()
        options.add_argument('--dns-prefetch-disable')

        self.driver = webdriver.Chrome(chrome_options=options) # chromedriver)

        self.driver.set_page_load_timeout(60)

        self.driver.get("http://localhost:8080/pureweb/app?client=html5&name=CartaSkeleton3")

        #self.driver.get("http://199.116.235.164:8080/pureweb/app/unix:0.0/4/143/1?client=html5&name=CartaSkeleton3")
        #self.driver.get("http://199.116.235.162:8080/pureweb/app/unix:0.0/4/143/1?client=html5&name=CartaSkeleton3")
        self.driver.implicitly_wait(20)
        time.sleep(5)


# Adds a window below the main image viewer. Return the window for future actions
def add_window(unittest, driver):
    timeout = selectBrowser._getSleep()

    # Click the Window button
    windowButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='Window']/..")))
    ActionChains(driver).click(windowButton).perform()

    # Look for the add button in the submenu.
    addButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div/div[text()='Add']/..")))
    ActionChains(driver).click( addButton ).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()

    # Check that we now have a generic empty window in the display and that the window count has gone up by one.
    emptyWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowGenericPlugin']")))
    ActionChains(driver).click( emptyWindow ).perform()

    return emptyWindow


# Change the animation window to an image window
def animation_to_image_window(unittest, driver):
    # Make sure the animation window is enabled by clicking an element within the window
    animWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")))
    ActionChains(driver).click( animWindow ).perform();
    # Change the plugin of the animation window to an image loader
    viewMenuButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.toolbar.MenuButton']/div[text()='View']/..")))
    ActionChains(driver).click( viewMenuButton ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()


# Clear text box and change value of the text box
# Selenium clear() method is sometimes unable to clear
# default text, therefore a manual method has been implemented
def _changeElementText(self, driver, elementText, inputValue):
    # First click on the element and get the element value
    ActionChains(driver).move_to_element( elementText ).click( elementText ).perform()
    elementValue = str(elementText.get_attribute("value"))
    # Clear the content in the text box
    for x in range(0, len(elementValue)):
        elementText.send_keys( Keys.BACK_SPACE )
    # Enter a new value into the text box and wait for element to update value
    ActionChains(driver).send_keys( inputValue ).send_keys( Keys.ENTER ).perform()
    # Get the new element value
    elementValue = elementText.get_attribute("value")
    return elementValue

# Click the tab with the given name
def clickTab( driver, tabName ):
    locator = "//div[@qxclass='qx.ui.tabview.TabButton']/div[text()='{0}']/..".format( tabName )
    print "Locator=",locator
    tab = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, locator )))
    driver.execute_script( "arguments[0].scrollIntoView(true);", tab)
    ActionChains(driver).click( tab ).perform()

# Return the number of windows present within the application window
def get_window_count(unittest, driver):
     windowList = driver.find_elements_by_xpath("//div[@qxclass='qx.ui.window.Desktop']")
     windowCount = len( windowList )
     return windowCount


# Determine whether the check box is checked
def isChecked(unittest, checkBox):
    styleAtt = checkBox.get_attribute( "style");
    print "Style", styleAtt
    oldChecked = False
    if "checked.png" in styleAtt:
        oldChecked = True
        print "Found checked.png"
    return oldChecked


#Set a custom layout with the given number of rows and columns
def layout_custom(unittest, driver, rows, cols ):
    timeout = selectBrowser._getSleep()

     # Now right click the toolbar
    toolBar = driver.find_element_by_xpath( "//div[@qxclass='skel.widgets.Menu.ToolBar']")
    actionChains = ActionChains(driver)
    actionChains.context_click(toolBar).perform()

    # Find and click the customize button
    customizeButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Customize...']/..")))
    ActionChains(driver).click( customizeButton).perform()

    # Uncheck clipping
    layoutButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Layout']/preceding-sibling::div/div")))
    styleAtt = layoutButton.get_attribute( "style");
    print "Style", styleAtt
    if not "checked.png" in styleAtt:
        print "Clipping checked"
        layoutParent = layoutButton.find_element_by_xpath( '..')
        ActionChains(driver).click( layoutParent ).perform()

    # Close the toolbar
    closeButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Close']/..")))
    ActionChains(driver).click( closeButton).perform()

    # Click the customize button on the toolbar
    customLayoutButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Custom Layout']")))
    ActionChains(driver).click( customLayoutButton ).perform()

    # Get the row count spin and set its value.
    rowSpin = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[starts-with(@id,'customLayoutRows')]/input")))
    rowSpin.send_keys( Keys.BACK_SPACE )
    rowSpin.send_keys( str(rows) )

    # Get the column count spin and set its value.
    colSpin = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[starts-with(@id,'customLayoutCols')]/input")))
    colSpin.send_keys( str(cols) )
    colSpin.send_keys( Keys.ARROW_LEFT )
    colSpin.send_keys( Keys.BACK_SPACE )

    # Close the custom layout dialog
    closeButton = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[starts-with(@id,'customLayoutOK')]")))
    ActionChains(driver).click(closeButton).perform()
    time.sleep(timeout)


# Link second image
def link_second_image(unittest, driver, imageWindow2):
    timeout = selectBrowser._getSleep()

    # Get window size
    windowSize = imageWindow2.size
    width = windowSize['width'] / 2
    height = windowSize['height'] / 2

    # Find the link canvas
    canvas = driver.find_element_by_xpath( "//canvas[@qxclass='skel.widgets.Link.LinkCanvas']")

    # Link to the second image
    ActionChains(driver).click( canvas ).move_by_offset( -width, height ).double_click().perform()
    time.sleep( timeout )

    # Exit links
    ActionChains(driver).send_keys(Keys.ESCAPE).perform()


# Load an image in the main image window
def load_image(unittest, driver, imageName, imageId = "pwUID0"):
    browser = selectBrowser._getBrowser()
    timeout = selectBrowser._getSleep()

    # If image is not specified, load default image
    # Change this to a test image available where the tests are running
    # Test image will ideally have more than 3 channels for a successful test run
    if imageName == "Default":
        #imageName = "N15693D.fits"
        #imageName="TWHydra_CO2_1line.image.fits"
        imageName="Orion.methanol.cbc.contsub.image.fits"

    # Wait 30 seconds for the imageWindow to appear on the page
    imageWindow = WebDriverWait(driver, 30).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))

    # Find a window capable of loading an image and select the window
    ActionChains(driver).double_click( imageWindow ).perform()

    # Pause, otherwise stale element for Chrome
    time.sleep( timeout)

    # Click the data button
    dataButton = driver.find_element_by_xpath( "//div[text()='Data']/..")
    ActionChains(driver).click(dataButton).perform()

    # Look for the open button and click it to open the file dialog.
    openDataButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div/div[text()='Open...']/..")))
    ActionChains(driver).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

    # Select the specific image
    loadButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.ID, "loadFileButton")))
    fileDiv = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[text()='"+imageName+"']")))
    driver.execute_script( "arguments[0].scrollIntoView(true);", fileDiv)
    fileDiv.click()

    # Click the load button
    loadButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.ID, "loadFileButton")))
    loadButton.click()

    # Now close the file dialog
    closeButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.ID, "closeFileLoadButton")))
    closeButton.click()

    # Check that the window is displaying an image.
    viewElement = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.boundWidgets.View.View']")))
    imageElement = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.ID, imageId)))

    return imageWindow


# Add a window, convert the window to an image window. Use the new image window to load an image
def load_image_different_window(unittest, driver, imageName):
    timeout = selectBrowser._getSleep()
    browser = selectBrowser._getBrowser()

    # Find a window capable of loading an image.
    imageWindow = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")))
    ActionChains(driver).click(imageWindow).perform()
    time.sleep( timeout )

    # Add a new window below the main casa image window
    emptyWindow = add_window( unittest, driver)

    # Change the plugin of the empty window to an image loader
    ActionChains(driver).context_click( emptyWindow ).send_keys(Keys.ARROW_DOWN
        ).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_RIGHT).send_keys(Keys.ENTER).perform()
    return load_image_windowIndex( unittest, driver,imageName, 2 )


def load_image_windowIndex( unittest,driver,imageName,windowIndex):
    # Ensure that there is a new image window
    windowId = "CasaImageLoader"+str(windowIndex)
    imageWindow = WebDriverWait(driver, 30).until(EC.presence_of_element_located((By.ID, windowId)))
    ActionChains(driver).double_click( imageWindow ).perform()

    # Click the data button
    dataButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[text()='Data']/..")))
    ActionChains(driver).click(dataButton).perform()

    # Look for the open button and click it to open the file dialog.
    openDataButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div/div[text()='Open...']/..")))
    ActionChains(driver).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

    # Select the specific image
    loadButton = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.ID, "loadFileButton")))
    fileDiv = WebDriverWait(driver, 20).until(EC.presence_of_element_located((By.XPATH, "//div[text()='"+imageName+"']")))
    driver.execute_script( "arguments[0].scrollIntoView(true);", fileDiv)
    fileDiv.click()

    # Click the load button
    loadButton = driver.find_element_by_id( "loadFileButton")
    loadButton.click()

    # Now close the file dialog
    closeButton = driver.find_element_by_id( "closeFileLoadButton")
    closeButton.click()

    # Check that the window is displaying an image.
    viewElement = driver.find_element_by_xpath("//div[@qxclass='skel.boundWidgets.View.View']")
    imageElement = driver.find_element_by_id("pwUID0")

    # Return the second image loader window for further linking tests
    return imageWindow


# Open settings by clicking the settings checkbox located on the menu bar
def openSettings(unittest, driver, name, open ):
    settingsText = name + " Settings"
    searchPath ="//div[@qxclass='qx.ui.form.CheckBox']/div[text()='{0}']/following-sibling::div[@class='qx-checkbox']".format( settingsText )
    settingsCheckbox = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, searchPath )))
    settingsOpen = isChecked(unittest, settingsCheckbox )
    if (not settingsOpen and open) or (settingsOpen and not open):
        ActionChains(driver).click( settingsCheckbox ).perform()
    return settingsCheckbox




# Remove link from main casa image loader
def remove_main_link(unittest, driver, imageWindow):
    timeout = selectBrowser._getSleep()

    # Get window size
    windowSize = imageWindow.size
    width = windowSize['width'] / 2

    # Find the link canvas
    canvas = driver.find_element_by_xpath( "//canvas[@qxclass='skel.widgets.Link.LinkCanvas']")

    # Remove the link
    ActionChains(driver).click( canvas ).move_by_offset( -width, 0 ).context_click().perform()
    ActionChains(driver).send_keys(Keys.DOWN).send_keys(Keys.DOWN).send_keys(Keys.ENTER).perform()
    time.sleep(timeout)

    # Exit links
    ActionChains(driver).send_keys(Keys.ESCAPE).perform()


# Set the checked status of the checkbox.
def setChecked(self, driver, checkBox, checked):
    oldChecked = isChecked( self, checkBox )
    print "oldChecked=",oldChecked
    if checked != oldChecked :
        ActionChains(driver).click( checkBox ).perform()

# Verify that the number of animators that are visible is equal to the expected count
def verifyAnimationCount(unittest, parentWidget, expectedCount):
    animatorList = parentWidget.find_elements_by_xpath( ".//div[@qxclass='skel.boundWidgets.Animator']" )
    animatorCount = len( animatorList )
    print "Animator list count=", animatorCount
    unittest.assertEqual( animatorCount, expectedCount, "Animator count does not match expected count")

# Verify that the number of animators that are visible is equal to the expected count
def verifyAnimatorUpperBound(unittest, driver, expectedCount, animatorName):
    fullId = animatorName + "AnimatorUpperBound"
    animatorLabel = WebDriverWait(driver, 10).until(EC.presence_of_element_located( ( By.ID, fullId ) ) )
    upperBound = animatorLabel.text
    print "Animator upper bound=", upperBound
    unittest.assertEqual( upperBound, str(expectedCount), "Animator upper bound was not correct")
