import os
import pytest
import cartavis

def pytest_addoption(parser):
    parser.addoption("--directory", action="store", help="The directory where\
                     the Carta executable file is located.")
    parser.addoption("--executable", action="store", help="The name of the\
                     Carta executable file.")
    parser.addoption("--configFile", action="store")
    parser.addoption("--port", action="store")
    parser.addoption("--htmlFile", action="store")
    parser.addoption("--imageFile", action="store")

@pytest.fixture(scope="module")
def cartavisInstance(request):
    directory = request.config.getoption("--directory")
    executable = request.config.getoption("--executable")
    configFile = request.config.getoption("--configFile")
    port = request.config.getoption("--port")
    htmlFile = request.config.getoption("--htmlFile")
    imageFile = request.config.getoption("--imageFile")
    os.chdir(directory)
    v = cartavis.Cartavis(directory + '/' + executable, configFile, int(port),
                          htmlFile, imageFile)
    return v
