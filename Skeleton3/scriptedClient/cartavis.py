#!/usr/bin/env python
# -*- coding: utf-8 -*-

import subprocess
import socket
import time
import os.path

"""
Sample run:

import cartavis
v = cartavis.start()
v.setResolution( 1000, 1000)
v.openFile("1.fits")
v.applyAutoClip( 95)
v.applyColormap( "helix1")
v.showCoordinates()
v.saveScreenshot( "/scratch/1.png")
"""

#lastPort = 12345
lastPort = 9999

class Application:
    """Represents an application"""

    def __init__(self, executable, configFile, port, htmlFile, imageFile):
        global lastPort
        print "lastPort = ", lastPort
#        args = ["/home/jeff/scratch/build/2014-10-24/cpp/desktop/desktop",
#            "--config",
#            "/home/jeff/.cartavis/config.json",
#            "--scriptPort",
#            str(lastPort),
#            "--html",
#            "/home/jeff/dev/CARTAvis/Skeleton3/VFS/DesktopDevel/desktop/desktopIndex.html",
#            "/home/jeff/Dropbox/Astronomy/m31_cropped.fits"]
        args = [executable, "--scriptPort", 
                str(port), "--html", htmlFile, imageFile]
        print args
        self.popen = subprocess.Popen( args)
        print "Started process with pid=", self.popen.pid
        time.sleep( 3)
        self.visible = False
        self.socket = socket.socket( socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect(("localhost", lastPort))
        lastPort = lastPort + 1;
        return

    def __getListFromSocket(self):
        # Get a small amount of data from the socket
        bufferSize = 10
        stringData = self.socket.recv(bufferSize)
        # Figure out its length
        # (The separator character needs to be the same as 
        # ScriptedCommandListener::SIZE_DELIMITER)
        lengthStr, ignored, stringData = stringData.partition(':')
        goodDataLength = int(lengthStr)
        lengthSoFar = len(stringData)
        # If more data is needed, grab it before returning
        if (goodDataLength > bufferSize):
            stringData = stringData + self.socket.recv(goodDataLength - lengthSoFar)
        listData = stringData.splitlines()
        return listData

    # It would be nice if this function could actually tun the GUI on and off
    # with whatever the current state is.
    def setGuiVisible(self,flag):
        self.visible = flag
        return

    def isGuiVisible(self):
        return self.visible

    def kill(self):
        self.popen.kill()

    def quit(self):
        self.kill()

    def getColorMaps(self, substring=""):
        self.socket.sendall("getColorMaps " + substring + "\n")
        colorMapsList = self.__getListFromSocket()
        return colorMapsList

    def getFileList(self, substring=""):
        self.socket.sendall("getFileList " + substring + "\n")
        fileList = self.__getListFromSocket()
        return fileList

    def loadFile(self, imageViewId, fileName):
        self.socket.sendall("loadFile " + imageViewId + " " + fileName + "\n")

    def loadLocalFile(self, imageViewId, fileName):
        self.socket.sendall("loadLocalFile " + imageViewId + " " + fileName + "\n")

    def getImageViews(self):
        self.socket.sendall("getImageViews" + "\n")
        imageViewsList = self.__getListFromSocket()
        return imageViewsList

    def getColorMapViews(self):
        self.socket.sendall("getColorMapViews" + "\n")
        colorMapViewsList = self.__getListFromSocket()
        return colorMapViewsList

    def getAnimatorViews(self):
        self.socket.sendall("getAnimatorViews" + "\n")
        animatorViewsList = self.__getListFromSocket()
        return animatorViewsList

    def getHistogramViews(self):
        self.socket.sendall("getHistogramViews" + "\n")
        histogramViewsList = self.__getListFromSocket()
        return histogramViewsList

    def getStatisticsViews(self):
        self.socket.sendall("getStatisticsViews" + "\n")
        statisticsViewsList = self.__getListFromSocket()
        return statisticsViewsList

    def getLinkedColorMaps(self, controlId):
        self.socket.sendall("getLinkedColorMaps " + controlId + "\n")
        linkedColorMaps = self.__getListFromSocket()
        return linkedColorMaps

    def getLinkedAnimators(self, controlId):
        self.socket.sendall("getLinkedAnimators " + controlId + "\n")
        linkedAnimators = self.__getListFromSocket()
        return linkedAnimators

    def getLinkedHistograms(self, controlId):
        self.socket.sendall("getLinkedHistograms " + controlId + "\n")
        linkedHistograms = self.__getListFromSocket()
        return linkedHistograms

    def getLinkedStatistics(self, controlId):
        self.socket.sendall("getLinkedStatistics " + controlId + "\n")
        linkedStatistics = self.__getListFromSocket()
        return linkedStatistics

    def setAnalysisLayout(self):
        self.socket.sendall("setAnalysisLayout" + "\n")

    def setCustomLayout(self, rows, cols):
        self.socket.sendall("setCustomLayout " + str(rows) + " "
                            + str(cols) + "\n")

    def setImageLayout(self):
        self.socket.sendall("setImageLayout" + "\n")

    def setColorMap(self, colormapId, colormap):
        self.socket.sendall("setColorMap " + colormapId + " " + colormap + "\n")

    def reverseColorMap(self, colormapId, trueOrFalse):
        self.socket.sendall("reverseColorMap " + colormapId + " " + trueOrFalse + "\n")

    def invertColorMap(self, colormapId, trueOrFalse):
        self.socket.sendall("invertColorMap " + colormapId + " " + trueOrFalse + "\n")

    def setColorMix(self, colormapId, redPercent, greenPercent, bluePercent):
        self.socket.sendall("setColorMix " + colormapId + " " + redPercent + " " + greenPercent + " " + bluePercent + "\n")

    def setGamma(self, colormapId, gamma):
        self.socket.sendall("setGamma " + colormapId + " " + gamma + "\n")

    def setPlugins(self, pluginList):
        pluginString = ' '.join(pluginList)
        self.socket.sendall("setPlugins " + pluginString + "\n")

    def linkAdd(self, sourceId, destId):
        self.socket.sendall("linkAdd " + sourceId + " " + destId + "\n")

    def setFrame(self, animatorId, index):
        self.socket.sendall("setFrame " + animatorId + " " + index + "\n")

    def setClipValue(self, imageViewId, index):
        self.socket.sendall("setClipValue " + imageViewId + " " + index + "\n")

    def saveState(self, saveName):
        self.socket.sendall("saveState " + saveName + "\n")

# Commands below this point are possibly broken or just not implemented yet.

    def setResolution(self, xres, yres):
        print "Setting resolution to (" + str(xres) + ", " + str(yres) + ")"

    def applyAutoClip(self, percentage):
        print "Applying autoclip of " + str(percentage) + "%"
        self.socket.sendall("applyAutoClip " + str(percentage) + "\n")

    def setCoordinates(self, coordinateSystem):
        print "Setting coordinates to " + coordinateSystem + " system"
        self.socket.sendall("setCoordinates " + coordinateSystem + "\n")

    def showCoordinates(self):
        self.socket.sendall("showcoordinates" + "\n")

    def hideCoordinates(self):
        self.socket.sendall("hidecoordinates" + "\n")

    def toggleCoordinates(self):
        self.socket.sendall("togglecoordinates" + "\n")

    def saveScreenshot(self, fileName):
        print "Saving screenshot as " + fileName

def start(
        executable = "/home/jeff/scratch/build/2015-02-10_new_master_version/cpp/desktop/desktop", 
        configFile = "/home/jeff/.cartavis/config.json", 
        port = 9999, 
        htmlFile = "/home/jeff/dev/CARTAvis/Skeleton3/VFS/DesktopDevel/desktop/desktopIndex.html", 
        imageFile = "/home/jeff/Dropbox/Astronomy/m31_cropped.fits"
    ):
    print "Starting with arguments:\n"
    print "    executable: " + executable + "\n"
    print "    configFile: " + configFile + "\n"
    print "    port: " + str(port) + "\n"
    print "    htmlFile: " + htmlFile + "\n"
    print "    imageFile: " + imageFile + "\n"
    return Application(executable, configFile, port, htmlFile, imageFile)
