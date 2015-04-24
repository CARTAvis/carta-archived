import unittest
import tHistogram
import tLayout
import tLoadImage
import tMenuToolVisibility
import tAnimatorAddRemove
import tWindow
import tSnapshotData
import tSnapshotLayout
import tSnapshotPreferences

def suite():
    test_suite = unittest.TestSuite()
    test_suite.addTest( unittest.makeSuite(tAnimatorAddRemove.tAnimatorAddRemove))
    test_suite.addTest( unittest.makeSuite(tHistogram.tHistogram))
    test_suite.addTest( unittest.makeSuite(tLayout.tLayout))
    test_suite.addTest( unittest.makeSuite(tLoadImage.tLoadImage))
    test_suite.addTest( unittest.makeSuite(tMenuToolVisibility.tMenuToolVisibility))
    test_suite.addTest( unittest.makeSuite(tSnapshotData.tSnapshotData))
   
    test_suite.addTest( unittest.makeSuite(tSnapshotLayout.tSnapshotLayout))
    test_suite.addTest( unittest.makeSuite(tSnapshotPreferences.tSnapshotPreferences))
    test_suite.addTest( unittest.makeSuite(tWindow.tWindow))
    
    return test_suite

mySuite = suite()

runner = unittest.TextTestRunner()
runner.run( mySuite )
