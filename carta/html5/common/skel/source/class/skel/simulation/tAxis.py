import unittest
import Util
import time
import selectBrowser
from selenium import webdriver
from flaky import flaky
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.ui import WebDriverWait

#Test that we can successfully load images with various axis permutations.
@flaky(max_runs=3)
class tAxis(unittest.TestCase):
    def setUp(self):
        browser = selectBrowser._getBrowser()
        Util.setUp(self, browser)

    # Click the generate contour set tab.
    def _clickGridAxesTab(self, driver ):
        gridAxesTab = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='qx.ui.tabview.TabButton']/div[contains(text(),'Axes/Border')]/..")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", gridAxesTab)
        ActionChains(driver).click( gridAxesTab ).perform()

    # Get the first animation window
    def _getAnimationWindow(self, driver ):
        animWindow = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.XPATH, "//div[@qxclass='skel.widgets.Window.DisplayWindowAnimation']")))
        return animWindow

    # Get the animator of the type specified by the key parameter.
    def _getAnimatorType(self, driver, key ):
        animatorType = WebDriverWait( driver, 10).until(EC.presence_of_element_located((By.ID, key)))
        return animatorType

    # Test that we can add a default Contour set using the range method,
    # linear spacing, 3 levels, and a minimum level of 0.1 and a maximum level of 0.9.
    def test_velRaDecCube(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        #Load a default image
        Util.load_image( self, driver, "VelRaDec.fits")

        #Verify that there is a Declination Animator present and
        # NOT a Channel, Stokes, or RA animator
        animWindow = self._getAnimationWindow( driver )
        Util.verifyAnimationCount( self, animWindow, 1 )
        animatorType = self._getAnimatorType( driver, "DeclinationAnimatorType")

    # Test a cube with a tabular axis can be loaded
    def test_tabularAxisCube(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        #Load an image with a tabular axis
        Util.load_image( self, driver, "18h03m_tabular_axis.image")

        #Verify that there is a Tabular Animator present and
        # NOT any other animators
        animWindow = self._getAnimationWindow( driver )
        Util.verifyAnimationCount( self, animWindow, 1 )
        animatorType = self._getAnimatorType( driver,"TabularAnimatorType" )
        
    # Take a traditional 4D cube and test that the axes can be permuted.
    def test_permuteAxes(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        #Load an image RA/DEC/CHANNEL/STOKES with a non-trivial Stokes plane
        Util.load_image( self, driver, "Stokes2.image")

        #Verify that there are Channel and Stokes Animators present and
        # NOT any other animators.
        animWindow = self._getAnimationWindow( driver )
        Util.verifyAnimationCount( self, animWindow, 2 )
        animatorTypeChannel = self._getAnimatorType(driver, "ChannelAnimatorType")
        animatorTypeStokes = self._getAnimatorType(driver, "StokesAnimatorType")

        #Open the image settings tab
        Util.openSettings( self, driver, "Image", True )

        #Click on the Axes/Border tab to display controls for permuting axes
        self._clickGridAxesTab( driver )

        #Check that we can make the horizontal axis Channel.  The vertical axis should remain at DEC
        axisComboH = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "horizontalDisplayAxis")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", axisComboH )
        ActionChains(driver).click( axisComboH ).perform()
        ActionChains(driver).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        #Verify CHANNEL X DEC by checking animators
        animWindow = self._getAnimationWindow( driver )
        Util.verifyAnimationCount( self, animWindow, 2 )
        animatorTypeRA = self._getAnimatorType( driver, "Right AscensionAnimatorType")
        animatorTypeStokes = self._getAnimatorType( driver, "StokesAnimatorType")

        #Now see if we can plot channel vs Stokes by changing the vertical axis to Stokes
        axisComboV = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "verticalDisplayAxis")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", axisComboV )
        ActionChains(driver).click( axisComboV ).perform()
        ActionChains(driver).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ARROW_DOWN).send_keys(Keys.ENTER).perform()

        #Verify CHANNEL vs STOKES by checking animators
        animWindow = self._getAnimationWindow( driver )
        Util.verifyAnimationCount( self, animWindow, 2 )
        animatorTypeRA = self._getAnimatorType( driver, "Right AscensionAnimatorType")
        animatorTypeDEC = self._getAnimatorType( driver, "DeclinationAnimatorType")

        #Do a plot of DEC vs STOKES by changing the horizontal axis
        axisComboH = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "horizontalDisplayAxis")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", axisComboH )
        ActionChains(driver).click( axisComboH ).perform()
        ActionChains(driver).send_keys(Keys.ARROW_UP).send_keys(Keys.ENTER).perform()

        #Verify the DEC x STOKES plane by checking animators
        animWindow = self._getAnimationWindow( driver )
        Util.verifyAnimationCount( self, animWindow, 2 )
        animatorTypeRA = self._getAnimatorType( driver, "Right AscensionAnimatorType")
        animatorTypeChannel = self._getAnimatorType( driver, "ChannelAnimatorType")


        #Do a plot of DEC x RA by chaning the vertical axis
        axisComboV = WebDriverWait(driver, 10).until(EC.presence_of_element_located((By.ID, "verticalDisplayAxis")))
        driver.execute_script( "arguments[0].scrollIntoView(true);", axisComboV )
        ActionChains(driver).click( axisComboV ).perform()
        ActionChains(driver).send_keys(Keys.ARROW_UP).send_keys(Keys.ARROW_UP).send_keys(Keys.ARROW_UP).send_keys(Keys.ENTER).perform()

        #Verify the DEC x RA plane by checking animators
        animWindow = self._getAnimationWindow( driver )
        Util.verifyAnimationCount( self, animWindow, 2 )
        animatorTypeStokes = self._getAnimatorType( driver, "StokesAnimatorType")
        animatorTypeChannel = self._getAnimatorType( driver, "ChannelAnimatorType")

    #Test a traditional cube with RA/DEC/CHANNEL/STOKES
    def test_almaCube(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        #Load an image with a tabular axis
        Util.load_image( self, driver, "M100_alma.fits")

        #Verify that there is a Channel Animator present and
        # NOT any other animators (Nothing to animate on stokes)
        animWindow = self._getAnimationWindow( driver )
        Util.verifyAnimationCount( self, animWindow, 1 )
        animatorType = self._getAnimatorType( driver, "ChannelAnimatorType")
        
    #This test was written in  response to issue 184. Loading a 2D image and changing the
    #coordinate system produced a crash.
    def test_coordinateChange(self):
        driver = self.driver
        timeout = selectBrowser._getSleep()

        #Load an image with a tabular axis
        Util.load_image( self, driver, "aJ.fits")
        time.sleep( 3 )

        #Open the image settings tab
        Util.openSettings( self, driver, "Image", True )
        time.sleep( timeout )
        
         #Get the old coordinateSystem
        systemText = driver.find_element_by_xpath("//div[@id='ImageCoordinateSystem']/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", systemText )
        oldSystem = systemText.get_attribute('value')
        print "Old system=", oldSystem
        
        #Change the coordinate system
        csCombo = driver.find_element_by_xpath("//div[@id='ImageCoordinateSystem']/div")
        driver.execute_script( "arguments[0].scrollIntoView(true);", csCombo )
        ActionChains(driver).click(csCombo).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ARROW_DOWN
                ).send_keys( Keys.ARROW_DOWN).send_keys( Keys.ENTER).perform()
        time.sleep( timeout )
        
        #Verify the coordinate system is changed
        systemText = driver.find_element_by_xpath("//div[@id='ImageCoordinateSystem']/input")
        driver.execute_script( "arguments[0].scrollIntoView(true);", systemText )
        newSystem = systemText.get_attribute( 'value')
        print "New system=",newSystem
        self.assertTrue( newSystem != oldSystem, "Coordinate system did not change")
        

    def tearDown(self):
        #Close the browser
        self.driver.close()
        #Allow browser to fully close before continuing
        time.sleep(2)
        #Close the session and delete temporary files
        self.driver.quit()

if __name__ == "__main__":
    unittest.main()
