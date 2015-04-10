#!/usr/bin/env python
# -*- coding: utf-8 -*-

import subprocess
import time

from tagconnector import TagConnector
from statistics import Statistics
from histogram import Histogram
from animator import Animator
from colormap import Colormap
from image import Image

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
            imageView = Image(iv, self.con)
            imageViews.append(imageView)
        return imageViews

    def getColormapViews(self):
        commandStr = "getColormapViews"
        colormapViewsList = self.con.cmdTagList(commandStr)
        colormapViews = []
        for cmv in colormapViewsList:
            colormapView = Colormap(cmv, self.con)
            colormapViews.append(colormapView)
        return colormapViews

    def getAnimatorViews(self):
        commandStr = "getAnimatorViews"
        animatorViewsList = self.con.cmdTagList(commandStr)
        animatorViews = []
        for av in animatorViewsList:
            animatorView = Animator(av, self.con)
            animatorViews.append(animatorView)
        return animatorViews

    def getHistogramViews(self):
        commandStr = "getHistogramViews"
        histogramViewsList = self.con.cmdTagList(commandStr)
        histogramViews = []
        for hv in histogramViewsList:
            histogramView = Histogram(hv, self.con)
            histogramViews.append(histogramView)
        return histogramViews

    def getStatisticsViews(self):
        commandStr = "getStatisticsViews"
        statisticsViewsList = self.con.cmdTagList(commandStr)
        statisticsViews = []
        for sv in statisticsViewsList:
            statisticsView = Statistics(sv, self.con)
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
        result = self.con.cmdTagList("fakeCommand", data=f.read())
        print "Finish time: " + time.asctime()
        return result

    def uc(self):
        """
        A command that is not implemented on the C++ side.
        For testing purposes only.
        """
        result = self.con.cmdTagList("unknownCommand")
        return result
