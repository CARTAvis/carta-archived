#!/usr/bin/env python
# -*- coding: utf-8 -*-

from statistics import Statistics
from cartaview import CartaView
from histogram import Histogram
from animator import Animator
from colormap import Colormap

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
            linkedColormapView = Colormap(colomap, self.con)
            linkedColormapViews.append(linkedColormapView)
        return linkedColormapViews

    def getLinkedAnimators(self):
        linkedAnimatorViewsList = self.con.cmdTagList("getLinkedAnimators", imageView=self.getId())
        linkedAnimatorViews = []
        for animator in linkedAnimatorViewsList:
            linkedAnimatorView = Animator(animator, self.con)
            linkedAnimatorViews.append(linkedAnimatorView)
        return linkedAnimatorViews

    def getLinkedHistograms(self):
        linkedHistogramViewsList = self.con.cmdTagList("getLinkedHistograms", imageView=self.getId())
        linkedHistogramViews = []
        for histogram in linkedHistogramViewsList:
            linkedHistogramView = Histogram(histogram, self.con)
            linkedHistogramViews.append(linkedHistogramView)
        return linkedHistogramViews

    def getLinkedStatistics(self):
        linkedStatisticsViewsList = self.con.cmdTagList("getLinkedStatistics", imageView=self.getId())
        linkedStatisticsViews = []
        for statistics in linkedStatisticsViewsList:
            linkedStatisticsView = Statistics(statistics, self.con)
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

    def saveImage(self, dest):
        result = self.con.cmdTagList("saveImage", imageView=self.getId(), filename=dest)
        return result

    def saveFullImage(self, dest):
        result = self.con.cmdTagList("saveFullImage", imageView=self.getId(), filename=dest)
        return result
