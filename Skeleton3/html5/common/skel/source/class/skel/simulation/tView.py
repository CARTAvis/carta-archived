import unittest
import Util
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains

#Test that we can view different plugins.
class tView(unittest.TestCase):
    def setUp(self):
        self.driver = webdriver.Firefox()
        self.driver.get("http://localhost:8080/pureweb/app?client=html5&name=CartaSkeleton3&username=dan12&password=Cameron21")
        self.driver.implicitly_wait(10)
        
        
    #Test that we can change an animator to a CasaImageLoader
    def test_animator_to_casaimageloader(self):    
        driver = self.driver
        
        #Get the animation window count and make sure it is non-zero
        animWindowList = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")
        animWindowCount = len( animWindowList )
        self.assertGreater( animWindowCount, 0, "There are not any animators")
        
         #Get the image window count
        imageWindowList = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        imageWindowCount = len( imageWindowList )
        
        # Locate an animator window and bring up the right-context menu,
        # changing to a CasaImageLoader.
        Util.animation_to_image_window( self, driver )
        
        # Verify that the animation count has gone down by one and the image count
        # has increased by one.
        animWindowList = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")
        newAnimWindowCount = len( animWindowList )
        self.assertEqual( animWindowCount-1, newAnimWindowCount, "Animation count did not decrease")
        imageWindowList = driver.find_elements_by_xpath("//div[@qxclass='skel.widgets.Window.DisplayWindowImage']")
        newImageWindowCount = len( imageWindowList )
        self.assertEqual( newImageWindowCount - 1, imageWindowCount, "Image window count did not increase")
        
    
    
    def tearDown(self):
        self.driver.close()

if __name__ == "__main__":
    unittest.main()        
        
