#!/usr/bin/env python
# -*- coding: utf-8 -*-

import subprocess
import socket
import time
import os.path
import struct
import binascii

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

class CartaView:
    """Base class for Carta objects"""
    def __init__(self, idStr, socket):
        self.__id = idStr
        self.socket = socket
        return

    def getId(self):
        """This is mainly for testing/debugging/sanity purposes"""
        return self.__id

    def addLink(self, imageView):
        commandStr = "addLink " + self.getId() + " " + imageView.getId()
        result = sendCommand(self.socket, commandStr)
        return result

    def removeLink(self, imageView):
        commandStr = "removeLink " + self.getId() + " " + imageView.getId()
        result = sendCommand(self.socket, commandStr)
        return result

class Colormap(CartaView):
    """Represents a colormap view"""

    def setColormap(self, colormap):
        """Set the specified colormap"""
        commandStr = "setColormap " + self.getId() + " " + colormap
        result = sendCommand(self.socket, commandStr)
        return result

    def reverseColormap(self, trueOrFalse):
        commandStr = "reverseColormap " + self.getId() + " " + trueOrFalse
        result = sendCommand(self.socket, commandStr)
        return result

    def setCacheColormap(self, cacheStr):
        commandStr = "setCacheColormap " + self.getId() + " " + cacheStr
        result = sendCommand(self.socket, commandStr)
        return result

    def setCacheSize(self, cacheSize):
        commandStr = "setCacheSize " + self.getId() + " " + str(cacheSize)
        result = sendCommand(self.socket, commandStr)
        return result

    def setInterpolatedColormap(self, trueOrFalse):
        commandStr = "setInterpolatedColormap " + self.getId() + " " + trueOrFalse
        result = sendCommand(self.socket, commandStr)
        return result

    def invertColormap(self, trueOrFalse):
        commandStr = "invertColormap " + self.getId() + " " + trueOrFalse
        result = sendCommand(self.socket, commandStr)
        return result

    def setColorMix(self, redPercent, greenPercent, bluePercent):
        commandStr = "setColorMix " + self.getId() + " " + str(redPercent) + " " + str(greenPercent) + " " + str(bluePercent)
        result = sendCommand(self.socket, commandStr)
        return result

    def setGamma(self, gamma):
        commandStr = "setGamma " + self.getId() + " " + str(gamma)
        result = sendCommand(self.socket, commandStr)
        return result

    def setDataTransform(self, transformString):
        commandStr = "setDataTransform " + self.getId() + " " + transformString
        result = sendCommand(self.socket, commandStr)
        return result

class Image(CartaView):
    """Represents an image view"""

    def loadFile(self, fileName):
        commandStr = "loadFile " + self.getId() + " " + fileName
        result = sendCommand(self.socket, commandStr)
        return result

    def loadLocalFile(self, fileName):
        commandStr = "loadLocalFile " + self.getId() + " " + fileName
        result = sendCommand(self.socket, commandStr)
        return result

    def getLinkedColormaps(self):
        commandStr = "getLinkedColormaps " + self.getId()
        linkedColormapViewsList = sendCommand(self.socket, commandStr)
        linkedColormapViews = []
        for colomap in linkedColormapViewsList:
            linkedColormapView = makeColormap(colomap, self.socket)
            linkedColormapViews.append(linkedColormapView)
        return linkedColormapViews

    def getLinkedAnimators(self):
        commandStr = "getLinkedAnimators " + self.getId()
        linkedAnimatorViewsList = sendCommand(self.socket, commandStr)
        linkedAnimatorViews = []
        for animator in linkedAnimatorViewsList:
            linkedAnimatorView = makeAnimator(animator, self.socket)
            linkedAnimatorViews.append(linkedAnimatorView)
        return linkedAnimatorViews

    def getLinkedHistograms(self):
        commandStr = "getLinkedHistograms " + self.getId()
        linkedHistogramViewsList = sendCommand(self.socket, commandStr)
        linkedHistogramViews = []
        for histogram in linkedHistogramViewsList:
            linkedHistogramView = makeHistogram(histogram, self.socket)
            linkedHistogramViews.append(linkedHistogramView)
        return linkedHistogramViews

    def getLinkedStatistics(self):
        commandStr = "getLinkedStatistics " + self.getId()
        linkedStatisticsViewsList = sendCommand(self.socket, commandStr)
        linkedStatisticsViews = []
        for statistics in linkedStatisticsViewsList:
            linkedStatisticsView = makeStatistics(statistics, self.socket)
            linkedStatisticsViews.append(linkedStatisticsView)
        return linkedStatisticsViews

    def setClipValue(self, index):
        commandStr = "setClipValue " + self.getId() + " " + str(index)
        result = sendCommand(self.socket, commandStr)
        return result

    def addLink(self, destView):
        """ Note that this method needs to override the base class method
            because the source and destination are flipped."""
        commandStr = "addLink " + destView.getId() + " " + self.getId()
        result = sendCommand(self.socket, commandStr)
        return result

    def removeLink(self, destView):
        """ Note that this method needs to override the base class method
            because the source and destination are flipped."""
        commandStr = "removeLink " + destView.getId() + " " + self.getId()
        result = sendCommand(self.socket, commandStr)
        return result

class Animator(CartaView):
    """Represents an animator view"""

    def setFrame(self, index):
        commandStr = "setFrame " + self.getId() + " " + str(index)
        result = sendCommand(self.socket, commandStr)
        return result

class Statistics(CartaView):
    """Represents a statistics view"""

class Histogram(CartaView):
    """Represents a histogram view"""

class Application:
    """Represents an application"""

    def __init__(self, executable, configFile, port, htmlFile, imageFile):
        global lastPort
        print "lastPort = ", lastPort
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

    def fakeCommand(self):
        commandStr = """fake command to send a huge amount of text"""
        result = sendCommand(self.socket, commandStr)
        return result;

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
    sendTypedMessage(socket, commandStr, 1)
    data = []
    print "sendCommand data (before) = " + str(data)
    output = receiveTypedMessage(socket, 1, data)
    print "sendCommand data (after) = " + str(data)
    #return output
    return data

def sendNBytes(socket, message, n):
    """the sendNBytes() method"""
    """returns a boolean value, either:"""
    """     nothing else to read"""
    """     all bytes"""
    """needs to loop until all bytes have been read"""
    print "sendNBytes"
    print "Sending message: " + str(message)
    result = socket.sendall(message)
    print "sendall result = " + str(result)

def receiveNBytes(socket, n, data):
    """the receiveNBytes() method"""
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
    for d in listData:
        data.append(d)
    print "receiveNBytes data = " + str(data)
    return True
    #return listData

def sendMessage(socket, message):
    """the sendMessage() method"""
    packed_len = struct.pack('>i', len(message))
    print "message length = " + str(len(message))
    print "len(packed_len) = " + str(len(packed_len))
    print "total length = " + str(len(message+packed_len))
    print 'sending "%s"' % binascii.hexlify(packed_len)
    for i in range(0,4):
        print 'packed_len[%s] = "%s"' % (i, binascii.hexlify(packed_len[i]))
    sendNBytes(socket, packed_len + message, 100000)

def receiveMessage(socket, data):
    """the receiveMessage() method"""
    """format: 4, 6, or 8 bytes: the size of the following message"""
    """after receiving this, enter a loop to receive this number of bytes"""
    #output = receiveNBytes(socket, 100000)
    print "receiveMessage data (before) = " + str(data)
    output = receiveNBytes(socket, 100000, data)
    print "receiveMessage data (after) = " + str(data)
    return output

def sendTypedMessage(socket, message, messageType):
    """the sendTypedMessage() method"""
    sendMessage(socket, message)

def receiveTypedMessage(socket, messageType, data):
    """the receiveTypedMessage() method"""
    #output = receiveMessage(socket)
    print "receiveTypedMessage data (before) = " + str(data)
    output = receiveMessage(socket, data)
    print "receiveTypedMessage data (after) = " + str(data)
    #return output
    return data
