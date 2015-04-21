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
        result = self.con.cmdTagList("centerOnPixel", imageView=self.getId(), xval=x, yval=y)
        animator = self.getLinkedAnimators()[0]
        animator.setChannel(z)
        return result

    def centerWithRadius(self, x, y, radius, dim='width'):
        """
        A convenience function:
            something that takes a centre and a radius and computes a ZoomLevel
            to put the centre at the centre of the viewer window and have the
            distance to the edge of the window be the "radius".
        Note that this function is defined entirely in terms of other, lower
        level Python functions.
        """
        self.centerOnPixel(x, y)
        viewerDim = self.getOutputSize()
        if dim == 'width':
            dimNumber = 0
        elif dim == 'height':
            dimNumber = 1
        zoom = (float(viewerDim[dimNumber]/2)) / radius
        self.setZoomLevel(zoom)

    def fitToViewer(self):
        iDim = self.getImageDimensions()
        oDim = self.getOutputSize()
        if (oDim[0] < oDim[1]):
            self.centerWithRadius(iDim[0]/2,iDim[1]/2,iDim[0]/2,'width')
        else:
            self.centerWithRadius(iDim[0]/2,iDim[1]/2,iDim[1]/2,'height')

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

    def getChannelCount(self):
        """
        A convenience function.
        Returns the number of channels in the image.
        """
        result = 1
        dimensions = self.getImageDimensions()
        if (len(dimensions) > 2):
            result = dimensions[2]
        return result

    def getOutputSize(self):
        result = self.con.cmdTagList("getOutputSize", imageView=self.getId())
        result = [int(i) for i in result]
        return result
