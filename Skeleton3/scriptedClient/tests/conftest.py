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
    return v
