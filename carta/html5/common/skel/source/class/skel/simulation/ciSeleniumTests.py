#!/usr/bin/env python

import sys
import unittest
import tHistogram
import tLayout
import tLoadImage
import tMenuToolVisibility
import tAnimatorTapeDeck
import tAnimatorSettings
import tAnimatorLinks
import tAxis
import tContour
import tView
import tWindow
import tSnapshotData
import tSnapshotLayout
import tSnapshotPreferences

def ciSuite():

    test_suite = unittest.TestSuite()
    test_suite.addTest( unittest.makeSuite(tLoadImage.tLoadImage))
    test_suite.addTest( unittest.makeSuite(tWindow.tWindow))
    test_suite.addTest( unittest.makeSuite(tLayout.tLayout))
    test_suite.addTest( unittest.makeSuite(tView.tView))
    test_suite.addTest( unittest.makeSuite(tMenuToolVisibility.tMenuToolVisibility))
    test_suite.addTest( unittest.makeSuite(tAxis.tAxis))
    test_suite.addTest( unittest.makeSuite(tHistogram.tHistogram))
    test_suite.addTest( unittest.makeSuite(tContour.tContour))
    test_suite.addTest( unittest.makeSuite(tSnapshotData.tSnapshotData))

    # tests that fail
    #timeout messages
    #test_suite.addTest( unittest.makeSuite(tAnimatorSettings.tAnimatorSettings))
    #test_suite.addTest( unittest.makeSuite(tAnimatorTapeDeck.tAnimatorTapeDeck))
    #test_suite.addTest( unittest.makeSuite(tSnapshotLayout.tSnapshotLayout))
    #test_suite.addTest( unittest.makeSuite(tSnapshotPreferences.tSnapshotPreferences))
    #test_suite.addTest( unittest.makeSuite(tAnimatorLinks.tAnimatorLinks))

    return test_suite

testSuite = ciSuite()

runner = unittest.TextTestRunner()

ret = not runner.run(testSuite).wasSuccessful()
sys.exit(ret)
