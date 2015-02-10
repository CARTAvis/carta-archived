import unittest
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

class tLoadImage(unittest.TestCase):
    def setUp(self):
        self.driver = webdriver.Firefox()
           
    def test_load_image(self):    
        driver = self.driver
        driver.get("http://localhost:8080/pureweb/app?client=html5&name=Skeleton3&username=dan12&password=Cameron21")
        driver.implicitly_wait(10)
        
        # Find a window capable of loading an image.
        imageWindow = driver.find_element_by_xpath("//div[@qxclass='skel.widgets.DisplayWindowImage']")
               
        # Show the context menu
        ActionChains(driver).context_click(imageWindow).perform()
        
        # Click the data button
        dataButton = driver.find_element_by_xpath("//div[text()='Data']/..")
        ActionChains(driver).click(dataButton).perform()
        
        # Look for the open button and click it to open the file dialog.
        openDataButton = driver.find_element_by_xpath("//div[text()='Open...']/..")
        self.assertIsNotNone(openDataButton, "Could not click open button on context menu.")
        ActionChains(driver).click(openDataButton).perform()
        
        # Find the last tree folder child on the page and set it selected. This should be a file rather than a directory. 
        folder = driver.find_element_by_xpath("//div[@qxclass='qx.ui.tree.TreeFolder']")
        self.assertIsNotNone(folder, "Could not find file tree")
        folderImage = folder.find_element_by_xpath("div[@qxclass='qx.ui.basic.Label']")
        self.assertIsNotNone(folderImage, "Could not find folder image");
        ActionChains(driver).double_click(folderImage);
        i = 0
        print i
        
        while folder is not None:
            try:
                target = folder
                i = i + 1
                print i
                # driver.switch_to( folder )
                folder = target.find_element_by_xpath(".//div[@qxclass='qx.ui.tree.TreeFolder']")
            except:
                print "Finished going through folders"
                break
        ActionChains(driver).click(target)
        
        # Hit the load button
        loadButton = driver.find_element_by_xpath("//div[text()='Load']/..")
        self.assertIsNotNone(loadButton, "Could not find load button to click")
        ActionChains(driver).click(loadButton).perform()
        
        # Close the file dialog
        closeButton = driver.find_element_by_xpath("//div[text()='Close']/..")
        self.assertIsNotNone(closeButton, "Could not find close button to click")
        ActionChains(driver).click(closeButton).perform()
        
        # Message should be sent to server - check that the window is not displaying an image.
        # driver.switch_to( imageWindow )
        viewElement = driver.find_element_by_xpath("//div[@qxclass='skel.boundWidgets.SuffixedView']")
        self.assertIsNotNone(viewElement, "Could not find view element on page.")
        # driver.switchTo( viewElement )
        imageElement = driver.find_element_by_id("img_pwUID0")
        self.assertIsNotNone(imageElement, "Could not find image on the page")
        
    def tearDown(self):
        self.driver.close()

if __name__ == "__main__":
    unittest.main()        
        
