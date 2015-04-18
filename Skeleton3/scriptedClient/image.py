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

    def centerOnPixel(self, x, y, z=0):
        """
        Centers the viewer on the pixel at (x, y).
        z is used to select the dimension in a data cube.
        """
        result = self.con.cmdTagList("centerOnPixel", imageView=self.getId(), xval=x, yval=y, zval=z)
        animator = self.getLinkedAnimators()[0]
        animator.setChannel(z)
        return result

    def centerWithRadius(self, x, y, r, dim='width'):
        result = self.com.cmdTagList("centerWithRadius", imageView=self.getId(), xval=x, yval=y, radius=r, dimension=dim)
        return result

    def setZoomLevel(self, zoom):
        result = self.con.cmdTagList("setZoomLevel", imageView=self.getId(), zoomLevel=zoom)
        return result

    def getZoomLevel(self):
        result = self.con.cmdTagList("getZoomLevel", imageView=self.getId())
        return float(result[0])

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

    def saveFullImage(self, dest, saveScale=1):
        result = self.con.cmdTagList("saveFullImage", imageView=self.getId(), filename=dest, scale=saveScale)
        return result

    def getImageDimensions(self):
        result = self.con.cmdTagList("getImageDimensions", imageView=self.getId())
        result = [int(i) for i in result]
        return result

    def getOutputSize(self):
        result = self.con.cmdTagList("getOutputSize", imageView=self.getId())
        result = [int(i) for i in result]
        return result
