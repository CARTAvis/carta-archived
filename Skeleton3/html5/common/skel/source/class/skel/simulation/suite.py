import unittest
import tBinCount
import tLayout
import tLoadImage
import tMenuToolVisibility
import tAnimatorAddRemove
import tWindow
import tSnapshotLayout
import tSnapshotPreferences

def suite():
    test_suite = unittest.TestSuite()
    test_suite.addTest( unittest.makeSuite(tBinCount.tBinCount))
    test_suite.addTest( unittest.makeSuite(tLayout.tLayout))
    test_suite.addTest( unittest.makeSuite(tLoadImage.tLoadImage))
    test_suite.addTest( unittest.makeSuite(tMenuToolVisibility.tMenuToolVisibility))
    test_suite.addTest( unittest.makeSuite(tAnimatorAddRemove.tAnimatorAddRemove))
    test_suite.addTest( unittest.makeSuite(tWindow.tWindow))
    test_suite.addTest( unittest.makeSuite(tSnapshotLayout.tSnapshotLayout))
    test_suite.addTest( unittest.makeSuite(tSnapshotPreferences.tSnapshotPreferences))
    
    return test_suite

mySuite = suite()

runner = unittest.TextTestRunner()
runner.run( mySuite )
