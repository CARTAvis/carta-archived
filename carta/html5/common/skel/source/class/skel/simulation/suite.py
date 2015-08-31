import unittest
import tHistogram
import tLayout
import tLoadImage
import tMenuToolVisibility
import tAnimatorTapeDeck
import tAnimatorSettings
import tAnimatorLinks
import tView
import tWindow
import tSnapshotData
import tSnapshotLayout
import tSnapshotPreferences

def suite():
    
    test_suite = unittest.TestSuite()
    test_suite.addTest( unittest.makeSuite(tLoadImage.tLoadImage))
    test_suite.addTest( unittest.makeSuite(tWindow.tWindow))
    test_suite.addTest( unittest.makeSuite(tLayout.tLayout))
    test_suite.addTest( unittest.makeSuite(tView.tView))
    test_suite.addTest( unittest.makeSuite(tMenuToolVisibility.tMenuToolVisibility))
    test_suite.addTest( unittest.makeSuite(tAnimatorTapeDeck.tAnimatorTapeDeck))
    test_suite.addTest( unittest.makeSuite(tAnimatorSettings.tAnimatorSettings))
    test_suite.addTest( unittest.makeSuite(tAnimatorLinks.tAnimatorLinks))
    test_suite.addTest( unittest.makeSuite(tHistogram.tHistogram)) 
    test_suite.addTest( unittest.makeSuite(tSnapshotData.tSnapshotData))   
    test_suite.addTest( unittest.makeSuite(tSnapshotLayout.tSnapshotLayout))
    test_suite.addTest( unittest.makeSuite(tSnapshotPreferences.tSnapshotPreferences))
    
    return test_suite

mySuite = suite()

runner = unittest.TextTestRunner()
runner.run( mySuite )
