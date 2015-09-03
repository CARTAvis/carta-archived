import os
import cartavis
import pyautogui
import ImageUtil

def test_layout_analysis(cartavisInstance, cleanSlate):
    """
    Test that the default layout is the analysis layout.
    Check that there is one animator, one histogram, one
    colormap and one image viewer window. 
    """
    # Set the layout to an analysis layout
    cartavisInstance.setAnalysisLayout()
    time.sleep(2)

    assert len(cartavisInstance.getImageViews()) == 1
    assert len(cartavisInstance.getHistogramViews()) == 1
    assert len(cartavisInstance.getColormapViews()) == 1
    assert len(cartavisInstance.getAnimatorViews()) == 1
    # Expect one more because of 'hidden' plugin in list
    assert len(cartavisInstance.getPluginList()) == 5 

def test_layout_custom(cartavisInstance, cleanSlate):
    """
    Test that we can switch to an image layout using the 'Layout' menu button
    """
    # Set the layout to an image layout
    cartavisInstance.setImageLayout()
    time.sleep(2)

    # Check that there is an Image Window and no other windows
    assert len(cartavisInstance.getImageViews()) == 1
    # Expect one more because of 'hidden' plugin in list
    assert len(cartavisInstance.getPluginList()) == 2

def test_layout_custom(cartavisInstance, cleanSlate):
    """
    Test that we can set a custom layout with 5 rows and 3 columns
    """
    # Set the custom layout to 5 rows and 3 columns 
    cartavisInstance.setCustomLayout(5, 3)

    # Check that there are 15 windows 
    windowCount = len(cartavisInstance.getPluginList()) 
    print 'windowCount=', windowCount
    assert windowCount == 15