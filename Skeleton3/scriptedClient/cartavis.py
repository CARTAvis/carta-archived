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
        commandStr = "setColormap " + self.__colormapId + " " + colormap
        result = sendCommand(self.__socket, commandStr)
        return result

    def reverseColormap(self, trueOrFalse):
        commandStr = "reverseColormap " + self.__colormapId + " " + trueOrFalse
        result = sendCommand(self.__socket, commandStr)
        return result

    def setCacheColormap(self, cacheStr):
        commandStr = "setCacheColormap " + self.__colormapId + " " + cacheStr
        result = sendCommand(self.__socket, commandStr)
        return result

    def setCacheSize(self, cacheSize):
        commandStr = "setCacheSize " + self.__colormapId + " " + str(cacheSize)
        result = sendCommand(self.__socket, commandStr)
        return result

    def setInterpolatedColormap(self, trueOrFalse):
        commandStr = "setInterpolatedColormap " + self.__colormapId + " " + trueOrFalse
        result = sendCommand(self.__socket, commandStr)
        return result

    def invertColormap(self, trueOrFalse):
        commandStr = "invertColormap " + self.__colormapId + " " + trueOrFalse
        result = sendCommand(self.__socket, commandStr)
        return result

    def setColorMix(self, redPercent, greenPercent, bluePercent):
        commandStr = "setColorMix " + self.__colormapId + " " + str(redPercent) + " " + str(greenPercent) + " " + str(bluePercent)
        result = sendCommand(self.__socket, commandStr)
        return result

    def setGamma(self, gamma):
        commandStr = "setGamma " + self.__colormapId + " " + str(gamma)
        result = sendCommand(self.__socket, commandStr)
        return result

    def setDataTransform(self, transformString):
        commandStr = "setDataTransform " + self.__colormapId + " " + transformString
        result = sendCommand(self.__socket, commandStr)
        return result

    def addLink(self, imageView):
        commandStr = "addLink " + self.getId() + " " + imageView.getId()
        result = sendCommand(self.__socket, commandStr)
        return result

    def removeLink(self, imageView):
        commandStr = "removeLink " + self.getId() + " " + imageView.getId()
        result = sendCommand(self.__socket, commandStr)
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
        commandStr = "loadFile " + self.__imageViewId + " " + fileName
        result = sendCommand(self.__socket, commandStr)
        return result

    def loadLocalFile(self, fileName):
        commandStr = "loadLocalFile " + self.__imageViewId + " " + fileName
        result = sendCommand(self.__socket, commandStr)
        return result

    def getLinkedColormaps(self):
        commandStr = "getLinkedColormaps " + self.__imageViewId
        linkedColormapViewsList = sendCommand(self.__socket, commandStr)
        linkedColormapViews = []
        for colomap in linkedColormapViewsList:
            linkedColormapView = makeColormap(colomap, self.__socket)
            linkedColormapViews.append(linkedColormapView)
        return linkedColormapViews

    def getLinkedAnimators(self):
        commandStr = "getLinkedAnimators " + self.__imageViewId
        linkedAnimatorViewsList = sendCommand(self.__socket, commandStr)
        linkedAnimatorViews = []
        for animator in linkedAnimatorViewsList:
            linkedAnimatorView = makeAnimator(animator, self.__socket)
            linkedAnimatorViews.append(linkedAnimatorView)
        return linkedAnimatorViews

    def getLinkedHistograms(self):
        commandStr = "getLinkedHistograms " + self.__imageViewId
        linkedHistogramViewsList = sendCommand(self.__socket, commandStr)
        linkedHistogramViews = []
        for histogram in linkedHistogramViewsList:
            linkedHistogramView = makeHistogram(histogram, self.__socket)
            linkedHistogramViews.append(linkedHistogramView)
        return linkedHistogramViews

    def getLinkedStatistics(self):
        commandStr = "getLinkedStatistics " + self.__imageViewId
        linkedStatisticsViewsList = sendCommand(self.__socket, commandStr)
        linkedStatisticsViews = []
        for statistics in linkedStatisticsViewsList:
            linkedStatisticsView = makeStatistics(statistics, self.__socket)
            linkedStatisticsViews.append(linkedStatisticsView)
        return linkedStatisticsViews

    def setClipValue(self, index):
        commandStr = "setClipValue " + self.__imageViewId + " " + str(index)
        result = sendCommand(self.__socket, commandStr)
        return result

    def addLink(self, destView):
        commandStr = "addLink " + destView.getId() + " " + self.getId()
        result = sendCommand(self.__socket, commandStr)
        return result

    def removeLink(self, destView):
        commandStr = "removeLink " + destView.getId() + " " + self.getId()
        result = sendCommand(self.__socket, commandStr)
        return result

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
        commandStr = "setFrame " + self.__animatorId + " " + str(index)
        result = sendCommand(self.__socket, commandStr)
        return result

    def addLink(self, imageView):
        commandStr = "addLink " + self.getId() + " " + imageView.getId()
        result = sendCommand(self.__socket, commandStr)
        return result

    def removeLink(self, imageView):
        commandStr = "removeLink " + self.getId() + " " + imageView.getId()
        result = sendCommand(self.__socket, commandStr)
        return result

class Statistics:
    """Represents a statistics view"""

    def __init__(self, statisticsId, socket):
        self.__statisticsId = statisticsId
        self.__socket = socket
        return

    def getId(self):
        """This is mainly for testing/debugging/sanity purposes"""
        return self.__statisticsId

    def addLink(self, imageView):
        commandStr = "addLink " + self.getId() + " " + imageView.getId()
        result = sendCommand(self.__socket, commandStr)
        return result

    def removeLink(self, imageView):
        commandStr = "removeLink " + self.getId() + " " + imageView.getId()
        result = sendCommand(self.__socket, commandStr)
        return result

class Histogram:
    """Represents a histogram view"""

    def __init__(self, histogramId, socket):
        self.__histogramId = histogramId
        self.__socket = socket
        return

    def getId(self):
        """This is mainly for testing/debugging/sanity purposes"""
        return self.__histogramId

    def addLink(self, imageView):
        commandStr = "addLink " + self.getId() + " " + imageView.getId()
        result = sendCommand(self.__socket, commandStr)
        return result

    def removeLink(self, imageView):
        commandStr = "removeLink " + self.getId() + " " + imageView.getId()
        result = sendCommand(self.__socket, commandStr)
        return result

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
        commandStr = "getColormaps " + substring
        colormapsList = sendCommand(self.socket, commandStr)
        return colormapsList

    def getFileList(self, substring=""):
        commandStr = "getFileList " + substring
        fileList = sendCommand(self.socket, commandStr)
        return fileList

    def getImageViews(self):
        commandStr = "getImageViews"
        imageViewsList = sendCommand(self.socket, commandStr)
        imageViews = []
        for iv in imageViewsList:
            imageView = makeImage(iv, self.socket)
            imageViews.append(imageView)
        return imageViews

    def getColormapViews(self):
        commandStr = "getColormapViews"
        colormapViewsList = sendCommand(self.socket, commandStr)
        colormapViews = []
        for cmv in colormapViewsList:
            colormapView = makeColormap(cmv, self.socket)
            colormapViews.append(colormapView)
        return colormapViews

    def getAnimatorViews(self):
        commandStr = "getAnimatorViews"
        animatorViewsList = sendCommand(self.socket, commandStr)
        animatorViews = []
        for av in animatorViewsList:
            animatorView = makeAnimator(av, self.socket)
            animatorViews.append(animatorView)
        return animatorViews

    def getHistogramViews(self):
        commandStr = "getHistogramViews"
        histogramViewsList = sendCommand(self.socket, commandStr)
        histogramViews = []
        for hv in histogramViewsList:
            histogramView = makeHistogram(hv, self.socket)
            histogramViews.append(histogramView)
        return histogramViews

    def getStatisticsViews(self):
        commandStr = "getStatisticsViews"
        statisticsViewsList = sendCommand(self.socket, commandStr)
        statisticsViews = []
        for sv in statisticsViewsList:
            statisticsView = makeStatistics(sv, self.socket)
            statisticsViews.append(statisticsView)
        return statisticsViews

    def setAnalysisLayout(self):
        commandStr = "setAnalysisLayout"
        result = sendCommand(self.socket, commandStr)
        return result

    def setCustomLayout(self, rows, cols):
        commandStr = "setCustomLayout " + str(rows) + " " + str(cols)
        result = sendCommand(self.socket, commandStr)
        return result

    def setImageLayout(self):
        commandStr = "setImageLayout"
        result = sendCommand(self.socket, commandStr)
        return result

    def setPlugins(self, pluginList):
        pluginString = ' '.join(pluginList)
        commandStr = "setPlugins " + pluginString
        result = sendCommand(self.socket, commandStr)
        return result

    def addLink(self, source, dest):
        commandStr = "addLink " + source.getId() + " " + dest.getId()
        result = sendCommand(self.socket, commandStr)
        return result

    def removeLink(self, source, dest):
        commandStr = "removeLink " + source.getId() + " " + dest.getId()
        result = sendCommand(self.socket, commandStr)
        return result

    def saveState(self, saveName):
        commandStr = "saveState " + saveName
        result = sendCommand(self.socket, commandStr)
        return result

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

def sendCommand(socket, commandStr):
    socket.sendall(commandStr + "\n")

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

