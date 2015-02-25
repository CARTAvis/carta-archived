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

class Colormap:
    """Represents a colormap view"""

    def __init__(self, colormapId, socket):
        self.__colormapId = colormapId
        self.__socket = socket
        return

    def getId(self):
        """This is mainly for testing/debugging/sanity purposes"""
        return self.__colormapId

    def setColormap(self, colormap):
        """Set the specified colormap"""
        self.__socket.sendall("setColormap " + self.__colormapId + " " + colormap + "\n")

    def reverseColormap(self, trueOrFalse):
        self.__socket.sendall("reverseColormap " + self.__colormapId + " " + trueOrFalse + "\n")

    def setCacheColormap(self, cacheStr):
        self.__socket.sendall("setCacheColormap " + self.__colormapId + " " + cacheStr + "\n")
        result = getListFromSocket(self.__socket)
        return result

    def setCacheSize(self, cacheSize):
        self.__socket.sendall("setCacheSize " + self.__colormapId + " " + cacheSize + "\n")
        result = getListFromSocket(self.__socket)
        return result

    def setInterpolatedColormap(self, trueOrFalse):
        self.__socket.sendall("setInterpolatedColormap " + self.__colormapId + " " + trueOrFalse + "\n")
        result = getListFromSocket(self.__socket)
        return result

    def invertColormap(self, trueOrFalse):
        self.__socket.sendall("invertColormap " + self.__colormapId + " " + trueOrFalse + "\n")

    def setColorMix(self, redPercent, greenPercent, bluePercent):
        self.__socket.sendall("setColorMix " + self.__colormapId + " " + str(redPercent) + " " + str(greenPercent) + " " + str(bluePercent) + "\n")

    def setGamma(self, gamma):
        self.__socket.sendall("setGamma " + self.__colormapId + " " + str(gamma) + "\n")

    def setDataTransform(self, transformString):
        self.__socket.sendall("setDataTransform " + self.__colormapId + " " + transformString + "\n")
        result = getListFromSocket(self.__socket)
        return result

class Image:
    """Represents an image view"""

    def __init__(self, imageViewId, socket):
        self.__imageViewId = imageViewId
        self.__socket = socket
        return

    def getId(self):
        """This is mainly for testing/debugging/sanity purposes"""
        return self.__imageViewId

    def loadFile(self, fileName):
        self.__socket.sendall("loadFile " + self.__imageViewId + " " + fileName + "\n")

    def loadLocalFile(self, fileName):
        self.__socket.sendall("loadLocalFile " + self.__imageViewId + " " + fileName + "\n")

    def getLinkedColormaps(self):
        self.__socket.sendall("getLinkedColormaps " + self.__imageViewId + "\n")
        linkedColormapViewsList = getListFromSocket(self.__socket)
        linkedColormapViews = []
        for colomap in linkedColormapViewsList:
            linkedColormapView = makeColormap(colomap, self.__socket)
            linkedColormapViews.append(linkedColormapView)
        return linkedColormapViews

    def getLinkedAnimators(self):
        self.__socket.sendall("getLinkedAnimators " + self.__imageViewId + "\n")
        linkedAnimatorViewsList = getListFromSocket(self.__socket)
        linkedAnimatorViews = []
        for animator in linkedAnimatorViewsList:
            linkedAnimatorView = makeAnimator(animator, self.__socket)
            linkedAnimatorViews.append(linkedAnimatorView)
        return linkedAnimatorViews

    def getLinkedHistograms(self):
        self.__socket.sendall("getLinkedHistograms " + self.__imageViewId + "\n")
        linkedHistogramViewsList = getListFromSocket(self.__socket)
        linkedHistogramViews = []
        for histogram in linkedHistogramViewsList:
            linkedHistogramView = makeHistogram(histogram, self.__socket)
            linkedHistogramViews.append(linkedHistogramView)
        return linkedHistogramViews

    def getLinkedStatistics(self):
        self.__socket.sendall("getLinkedStatistics " + self.__imageViewId + "\n")
        linkedStatisticsViewsList = getListFromSocket(self.__socket)
        linkedStatisticsViews = []
        for statistics in linkedStatisticsViewsList:
            linkedStatisticsView = makeStatistics(statistics, self.__socket)
            linkedStatisticsViews.append(linkedStatisticsView)
        return linkedStatisticsViews

    def setClipValue(self, index):
        self.__socket.sendall("setClipValue " + self.__imageViewId + " " + index + "\n")

class Animator:
    """Represents an animator view"""

    def __init__(self, animatorId, socket):
        self.__animatorId = animatorId
        self.__socket = socket
        return

    def getId(self):
        """This is mainly for testing/debugging/sanity purposes"""
        return self.__animatorId

    def setFrame(self, index):
        self.__socket.sendall("setFrame " + self.__animatorId + " " + str(index) + "\n")

class Statistics:
    """Represents a statistics view"""

    def __init__(self, statisticsId, socket):
        self.__statisticsId = statisticsId
        self.__socket = socket
        return

    def getId(self):
        """This is mainly for testing/debugging/sanity purposes"""
        return self.__statisticsId

class Histogram:
    """Represents a histogram view"""

    def __init__(self, histogramId, socket):
        self.__histogramId = histogramId
        self.__socket = socket
        return

    def getId(self):
        """This is mainly for testing/debugging/sanity purposes"""
        return self.__histogramId

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

    def getColormaps(self, substring=""):
        self.socket.sendall("getColormaps " + substring + "\n")
        colormapsList = getListFromSocket(self.socket)
        return colormapsList

    def getFileList(self, substring=""):
        self.socket.sendall("getFileList " + substring + "\n")
        fileList = getListFromSocket(self.socket)
        return fileList

    def getImageViews(self):
        self.socket.sendall("getImageViews" + "\n")
        imageViewsList = getListFromSocket(self.socket)
        imageViews = []
        for iv in imageViewsList:
            imageView = makeImage(iv, self.socket)
            imageViews.append(imageView)
        return imageViews

    def getColormapViews(self):
        self.socket.sendall("getColormapViews" + "\n")
        colormapViewsList = getListFromSocket(self.socket)
        colormapViews = []
        for cmv in colormapViewsList:
            colormapView = makeColormap(cmv, self.socket)
            colormapViews.append(colormapView)
        return colormapViews

    def getAnimatorViews(self):
        self.socket.sendall("getAnimatorViews" + "\n")
        animatorViewsList = getListFromSocket(self.socket)
        animatorViews = []
        for av in animatorViewsList:
            animatorView = makeAnimator(av, self.socket)
            animatorViews.append(animatorView)
        return animatorViews

    def getHistogramViews(self):
        self.socket.sendall("getHistogramViews" + "\n")
        histogramViewsList = getListFromSocket(self.socket)
        histogramViews = []
        for hv in histogramViewsList:
            histogramView = makeHistogram(hv, self.socket)
            histogramViews.append(histogramView)
        return histogramViews

    def getStatisticsViews(self):
        self.socket.sendall("getStatisticsViews" + "\n")
        statisticsViewsList = getListFromSocket(self.socket)
        statisticsViews = []
        for sv in statisticsViewsList:
            statisticsView = makeStatistics(sv, self.socket)
            statisticsViews.append(statisticsView)
        return statisticsViews

    def setAnalysisLayout(self):
        self.socket.sendall("setAnalysisLayout" + "\n")

    def setCustomLayout(self, rows, cols):
        self.socket.sendall("setCustomLayout " + str(rows) + " "
                            + str(cols) + "\n")

    def setImageLayout(self):
        self.socket.sendall("setImageLayout" + "\n")

    def setPlugins(self, pluginList):
        pluginString = ' '.join(pluginList)
        self.socket.sendall("setPlugins " + pluginString + "\n")

    def linkAdd(self, source, dest):
        self.socket.sendall("linkAdd " + source.getId() + " " + dest.getId() + "\n")

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
        executable = "/home/jeff/scratch/build/cpp/desktop/desktop", 
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

def makeImage(imageViewId, socket):
    image = Image(imageViewId, socket)
    return image

def makeColormap(colormapId, socket):
    colormap = Colormap(colormapId, socket)
    return colormap

def makeStatistics(statisticsId, socket):
    statistics = Statistics(statisticsId, socket)
    return statistics

def makeAnimator(animatorId, socket):
    animator = Animator(animatorId, socket)
    return animator

def makeHistogram(histogramId, socket):
    histogram = Histogram(histogramId, socket)
    return histogram

def getListFromSocket(socket):
    # Get a small amount of data from the socket
    bufferSize = 10
    stringData = socket.recv(bufferSize)
    # Figure out its length
    # (The separator character needs to be the same as 
    # ScriptedCommandListener::SIZE_DELIMITER)
    lengthStr, ignored, stringData = stringData.partition(':')
    goodDataLength = int(lengthStr)
    lengthSoFar = len(stringData)
    # If more data is needed, grab it before returning
    if (goodDataLength > bufferSize):
        stringData = stringData + socket.recv(goodDataLength - lengthSoFar)
    listData = stringData.splitlines()
    return listData

