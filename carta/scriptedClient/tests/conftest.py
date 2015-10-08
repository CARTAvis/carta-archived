import os
import pytest
import cartavis

def pytest_addoption(parser):
    parser.addoption("--directory", action="store", help="The directory where\
                     the Carta executable file is located.")
    parser.addoption("--executable", action="store", help="The name of the\
                     Carta executable file.")
    parser.addoption("--configFile", action="store", help="The full path of\
                     the cartavis config.json file.")
    parser.addoption("--port", action="store", help="The port which will be\
                     used to send commands to C++ and receive results.")
    parser.addoption("--htmlFile", action="store", help="The full path of the\
                     desktopIndex.html file.")
    parser.addoption("--imageFile", action="store", help="The full path of a\
                     compatible image file to load.")

@pytest.fixture(scope="module")
def cartavisInstance(request):
    """
    Return an instance of the Carta application for use by the tests.
    The "module" scope means that each test will receive the same Carta
    instance.
    """
    directory = request.config.getoption("--directory")
    executable = request.config.getoption("--executable")
    configFile = request.config.getoption("--configFile")
    port = request.config.getoption("--port")
    htmlFile = request.config.getoption("--htmlFile")
    imageFile = request.config.getoption("--imageFile")
    currentDir = os.getcwd()
    os.chdir(directory)
    v = cartavis.Cartavis(directory + '/' + executable, configFile, int(port),
                          htmlFile, imageFile)
    os.chdir(currentDir)
    def fin():
        v.quit()
    request.addfinalizer(fin)
    return v

@pytest.fixture(scope="module")
def tempImageDir(request):
    """
    Create a temporary directory for saving images.
    Return the directory name.
    When all tests using this fixture are done, remove this directory
    and its contents.
    """
    imageDir = '/tmp/cartavis-test'
    if not os.path.isdir(imageDir):
        print "Making tempImageDir"
        os.makedirs(imageDir)
    def fin():
        print "fin()"
        print "imageDir = " + imageDir
        for file in os.listdir(imageDir):
            print "deleting " + imageDir + '/' + file
            os.remove(imageDir + '/' + file)
        os.rmdir(imageDir)
    #request.addfinalizer(fin)
    return imageDir

@pytest.fixture(scope="function")
def cleanSlate(request, cartavisInstance):
    """
    Reset certain features of the GUI to some default values.
    This helps the tests to become more independent of running order;
    for example, if one test modifies the colormap before another test
    that depends on the colormap being Gray, the second test may fail
    if the colormap is not reset first.
    """
    i = cartavisInstance.getImageViews()
    c = cartavisInstance.getColormapViews()
    #h = cartavisInstance.getHistogramViews()
    a = cartavisInstance.getAnimatorViews()
    # Reset the layout to the default analysis layout
    # Set it back to image layout first, otherwise, resetting may not work
    cartavisInstance.setImageLayout() 
    cartavisInstance.setAnalysisLayout()
    # Reset the colormap
    c[0].setColormap('Gray')
    c[0].invertColormap(False)
    c[0].reverseColormap(False)
    c[0].setColorMix(1,1,1)
    c[0].setDataTransform('none')
    c[0].setGamma(1)
    # Reset the animator
    a[0].setChannel(0)
    # Reset the histogram
    #h[0].setPlaneMode('all')
    # Close all open images
    for imageName in i[0].getImageNames():
        i[0].closeImage(imageName)