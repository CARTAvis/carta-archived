import unittest
import tBinCount
import tLayout
import tLoadImage
import tMenuToolVisibility

def suite():
    test_suite = unittest.TestSuite()
    test_suite.addTest( unittest.makeSuite(tBinCount.tBinCount))
    test_suite.addTest( unittest.makeSuite(tLayout.tLayout))
    test_suite.addTest( unittest.makeSuite(tLoadImage.tLoadImage))
    test_suite.addTest( unittest.makeSuite(tMenuToolVisibility.tMenuToolVisibility))
    return test_suite

mySuite = suite()

runner = unittest.TextTestRunner()
runner.run( mySuite )
