#!/usr/bin/env python
# -*- coding: utf-8 -*-

import subprocess
import socket
import time
import json

from layer2 import TagMessage, TagMessageSocket
from layer3 import JsonMessage

class TagConnector:
    """
    Can connect to a port, then exposes the cmd() method to send commands
    and retrieve results. The commands and results are in JsonMessage format.
    Uses TagMessageSocket, which gives us freedom to later send/receive raw
    tag messages
    """
    def __init__(self,port):
        self.port = port
        self.socket = socket.socket( socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect(("localhost", self.port))
        self.tagMessageSocket = TagMessageSocket( self.socket)

    def cmd( self, cmd, ** kwargs):
        self.tagMessageSocket.send( JsonMessage.fromKW( cmd=cmd, args=kwargs).toTagMessage())
        tm = self.tagMessageSocket.receive()
        result = JsonMessage.fromTagMessage(tm)
        return result.jsonString

    def cmdTagList( self, cmd, ** kwargs):
        """
        Send a tag messge, return a list
        """
        self.tagMessageSocket.send( JsonMessage.fromKW( cmd=cmd, args=kwargs).toTagMessage())
        tm = self.tagMessageSocket.receive()
        result = JsonMessage.fromTagMessage(tm)
        j = json.loads(str(result.jsonString))
        return j['result']

class CartaView:
    """Base class for Carta objects"""
    def __init__(self, idStr, connection):
        self.__id = idStr
        self.con = connection
        return

    def getId(self):
        """This is mainly for testing/debugging/sanity purposes"""
        return self.__id

    def addLink(self, imageView):
        result = self.con.cmdTagList("addLink", sourceView=self.getId(), destView=imageView.getId())
        return result

    def removeLink(self, imageView):
        result = self.con.cmdTagList("removeLink", sourceView=self.getId(), destView=imageView.getId())
        return result

class Colormap(CartaView):
    """Represents a colormap view"""

    def setColormap(self, colormap):
        """Set the specified colormap"""
        result = self.con.cmdTagList("setColormap", colormapId=self.getId(), colormapName=colormap)
        return result

    def reverseColormap(self, reverseStr='toggle'):
        result = self.con.cmdTagList("reverseColormap", colormapId=self.getId(), reverseString=reverseStr)
        return result

    def setCacheColormap(self, cacheStr='toggle'):
        result = self.con.cmdTagList("setCacheColormap", colormapId=self.getId(), cacheString=cacheStr)
        return result

    def setCacheSize(self, cacheSize):
        result = self.con.cmdTagList("setCacheSize", colormapId=self.getId(), size=str(cacheSize))
        return result

    def setInterpolatedColormap(self, interpolatedStr='toggle'):
        result = self.con.cmdTagList("setInterpolatedColormap", colormapId=self.getId(), interpolatedString=interpolatedStr)
        return result

    def invertColormap(self, invertStr='toggle'):
        result = self.con.cmdTagList("invertColormap", colormapId=self.getId(), invertString=invertStr)
        return result

    def setColorMix(self, redPercent, greenPercent, bluePercent):
        result = self.con.cmdTagList("setColorMix", colormapId=self.getId(), red=str(redPercent), green=str(greenPercent), blue=str(bluePercent))
        return result

    def setGamma(self, gamma):
        result = self.con.cmdTagList("setGamma", colormapId=self.getId(), gammaValue=gamma)
        return result

    def setDataTransform(self, transformStr):
        result = self.con.cmdTagList("setDataTransform", colormapId=self.getId(), transform=transformStr)
        return result

class Image(CartaView):
    """Represents an image view"""

    def loadFile(self, fileName):
        result = self.con.cmdTagList("loadFile", imageView=self.getId(), fname="/RootDirectory/"+fileName)
        return result

    def loadLocalFile(self, fileName):
        result = self.con.cmdTagList("loadLocalFile", imageView=self.getId(), fname=fileName)
        return result

    def getLinkedColormaps(self):
        linkedColormapViewsList = self.con.cmdTagList("getLinkedColormaps", imageView=self.getId())
        linkedColormapViews = []
        for colomap in linkedColormapViewsList:
            linkedColormapView = makeColormap(colomap, self.con)
            linkedColormapViews.append(linkedColormapView)
        return linkedColormapViews

    def getLinkedAnimators(self):
        linkedAnimatorViewsList = self.con.cmdTagList("getLinkedAnimators", imageView=self.getId())
        linkedAnimatorViews = []
        for animator in linkedAnimatorViewsList:
            linkedAnimatorView = makeAnimator(animator, self.con)
            linkedAnimatorViews.append(linkedAnimatorView)
        return linkedAnimatorViews

    def getLinkedHistograms(self):
        linkedHistogramViewsList = self.con.cmdTagList("getLinkedHistograms", imageView=self.getId())
        linkedHistogramViews = []
        for histogram in linkedHistogramViewsList:
            linkedHistogramView = makeHistogram(histogram, self.con)
            linkedHistogramViews.append(linkedHistogramView)
        return linkedHistogramViews

    def getLinkedStatistics(self):
        linkedStatisticsViewsList = self.con.cmdTagList("getLinkedStatistics", imageView=self.getId())
        linkedStatisticsViews = []
        for statistics in linkedStatisticsViewsList:
            linkedStatisticsView = makeStatistics(statistics, self.con)
            linkedStatisticsViews.append(linkedStatisticsView)
        return linkedStatisticsViews

    def setClipValue(self, index):
        result = self.con.cmdTagList("setClipValue", imageView=self.getId(), clipValue=str(index))
        return result

    def updatePan(self, x, y):
        result = self.con.cmdTagList("updatePan", imageView=self.getId(), xval=x, yval=y)
        return result

    def updateZoom(self, x, y, z):
        result = self.con.cmdTagList("updateZoom", imageView=self.getId(), xval=x, yval=y)
        return result

    def addLink(self, dest):
        """ Note that this method needs to override the base class method
            because the source and destination are flipped."""
        result = self.con.cmdTagList("addLink", sourceView=dest.getId(), destView=self.getId())
        return result

    def removeLink(self, dest):
        """ Note that this method needs to override the base class method
            because the source and destination are flipped."""
        result = self.con.cmdTagList("removeLink", sourceView=dest.getId(), destView=self.getId())
        return result

class Animator(CartaView):
    """Represents an animator view"""

    def setChannel(self, index):
        result = self.con.cmdTagList("setChannel", animatorView=self.getId(), channel=index)
        return result

    def setImage(self, index):
        result = self.con.cmdTagList("setImage", animatorView=self.getId(), image=index)
        return result

    def showImageAnimator(self):
        result = self.con.cmdTagList("showImageAnimator", animatorView=self.getId())
        return result

class Statistics(CartaView):
    """Represents a statistics view"""

class Histogram(CartaView):
    """Represents a histogram view"""

class Application:
    """Represents an application"""

    def __init__(self, executable, configFile, port, htmlFile, imageFile):
        args = [executable, "--scriptPort", 
                str(port), "--html", htmlFile, imageFile]
        print args
        self.popen = subprocess.Popen(args)
        print "Started process with pid=", self.popen.pid
        time.sleep(3)
        self.visible = False
        self.con = TagConnector(port)
        return

    def setGuiVisible(self,flag):
        self.visible = flag
        return

    def isGuiVisible(self):
        return self.visible

    def kill(self):
        self.popen.kill()

    def quit(self):
        self.kill()

    def getColormaps(self):
        commandStr = "getColormaps"
        colormapsList = self.con.cmdTagList(commandStr)
        return colormapsList

    def getFileList(self):
        commandStr = "getFileList"
        fileListJson = self.con.cmdTagList(commandStr)
        j = json.loads(fileListJson)
        # Start parsing from the first occurrence of 'dir'
        fileList = parseDirectory(j['dir'], "")
        return fileList

    def getImageViews(self):
        commandStr = "getImageViews"
        imageViewsList = self.con.cmdTagList(commandStr)
        imageViews = []
        for iv in imageViewsList:
            imageView = makeImage(iv, self.con)
            imageViews.append(imageView)
        return imageViews

    def getColormapViews(self):
        commandStr = "getColormapViews"
        colormapViewsList = self.con.cmdTagList(commandStr)
        colormapViews = []
        for cmv in colormapViewsList:
            colormapView = makeColormap(cmv, self.con)
            colormapViews.append(colormapView)
        return colormapViews

    def getAnimatorViews(self):
        commandStr = "getAnimatorViews"
        animatorViewsList = self.con.cmdTagList(commandStr)
        animatorViews = []
        for av in animatorViewsList:
            animatorView = makeAnimator(av, self.con)
            animatorViews.append(animatorView)
        return animatorViews

    def getHistogramViews(self):
        commandStr = "getHistogramViews"
        histogramViewsList = self.con.cmdTagList(commandStr)
        histogramViews = []
        for hv in histogramViewsList:
            histogramView = makeHistogram(hv, self.con)
            histogramViews.append(histogramView)
        return histogramViews

    def getStatisticsViews(self):
        commandStr = "getStatisticsViews"
        statisticsViewsList = self.con.cmdTagList(commandStr)
        statisticsViews = []
        for sv in statisticsViewsList:
            statisticsView = makeStatistics(sv, self.con)
            statisticsViews.append(statisticsView)
        return statisticsViews

    def setAnalysisLayout(self):
        commandStr = "setAnalysisLayout"
        result = self.con.cmdTagList(commandStr)
        return result

    def setImageLayout(self):
        commandStr = "setImageLayout"
        result = self.con.cmdTagList(commandStr)
        return result

    def setCustomLayout(self, rows, cols):
        result = self.con.cmdTagList("setCustomLayout", nrows=rows, ncols=cols)
        return result

    def setPlugins(self, pluginList):
        pluginString = ' '.join(pluginList)
        result = self.con.cmdTagList("setPlugins", plugins=pluginString)
        return result

    def addLink(self, source, dest):
        result = self.con.cmdTagList("addLink", sourceView=source.getId(), destView=dest.getId())
        return result

    def removeLink(self, source, dest):
        result = self.con.cmdTagList("removeLink", sourceView=source.getId(), destView=dest.getId())
        return result

    def saveState(self, saveName):
        result = self.con.cmdTagList("saveState", name=saveName)
        return result

    def fakeCommand(self, infile):
        """Purely for the purpose of testing what happens when an arbitrarily
        large command is sent."""
        f = open(infile, 'r')
        print "Start time: " + time.asctime()
        result = self.con.cmdTagList("fakeCommand", inFile=f.read())
        print "Finish time: " + time.asctime()
        return result

def start(
        executable = "/home/jeff/scratch/build/cpp/desktop/desktop", 
        configFile = "/home/jeff/.cartavis/config.json", 
        port = 9999, 
        htmlFile = "/home/jeff/dev/CARTAvis/Skeleton3/VFS/DesktopDevel/desktop/desktopIndex.html", 
        imageFile = "/scratch/Images/m42_40min_ir.fits"
    ):
    return Application(executable, configFile, port, htmlFile, imageFile)

def startPavol(
        executable = "/scratch/builds/Skeleton3/Dev/cpp/desktop/desktop", 
        configFile = "/home/pfederl/.cartavis/config.json", 
        port = 9999, 
        htmlFile = "/home/pfederl/Work/ALMAvis/Skeleton3/VFS/DesktopDevel/desktop/desktopIndex.html", 
        imageFile = "/scratch/Images/smallcube.fits"
    ):
    return Application(executable, configFile, port, htmlFile, imageFile)

def makeImage(imageViewId, connection):
    image = Image(imageViewId, connection)
    return image

def makeColormap(colormapId, connection):
    colormap = Colormap(colormapId, connection)
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

def parseDirectory(directory, prefix):
    """ This is not working the way I want it to yet.
        Since it is only used for getFileList(), which in turn only lists the
        files in /scratch/Images, perhaps getFileList() can just be scrapped?
        The scripted client can load files from anywhere using
        loadLocalFile().
    """
    print "prefix = " + prefix
    fileList = []
    for i in range (0, len(directory)):
        print "directory[" + str(i) + "] = " + str(directory[i])
        name = directory[i]['name']
        print "name = " + name
        try:
            if directory[i]['dir'] != "":
                print "     This should be a subdirectory"
                subdirectory = directory[i]['dir']
                print "subdirectory = " + str(subdirectory)
                subFileList = parseDirectory(subdirectory, prefix + "/" + name)
                print "subFileList = " + str(subFileList)
                fileList.append(subFileList)
                print "fileList = " + str(fileList)
        except (KeyError):
            "KeyError"
            if prefix != "":
                name = prefix + "/" + name
            print "Appending " + name
            fileList.append(name)
            print "fileList = " + str(fileList)
    return fileList
