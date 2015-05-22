#!/usr/bin/env python
# -*- coding: utf-8 -*-

from astropy.coordinates import SkyCoord

from statistics import Statistics
from cartaview import CartaView
from histogram import Histogram
from animator import Animator
from colormap import Colormap

class Image(CartaView):
    """Represents an image view"""

    def loadFile(self, fileName):
        result = self.con.cmdTagList("loadFile", imageView=self.getId(),
                                     fname="/RootDirectory/"+fileName)
        return result

    def loadLocalFile(self, fileName):
        result = self.con.cmdTagList("loadLocalFile", imageView=self.getId(),
                                     fname=fileName)
        return result

    def getLinkedColormaps(self):
        resultList = self.con.cmdTagList("getLinkedColormaps",
                                         imageView=self.getId())
        linkedColormapViews = []
        if (resultList[0] != ""):
            for colormap in resultList:
                linkedColormapView = Colormap(colormap, self.con)
                linkedColormapViews.append(linkedColormapView)
        return linkedColormapViews

    def getLinkedAnimators(self):
        resultList = self.con.cmdTagList("getLinkedAnimators",
                                         imageView=self.getId())
        linkedAnimatorViews = []
        if (resultList[0] != ""):
            for animator in resultList:
                linkedAnimatorView = Animator(animator, self.con)
                linkedAnimatorViews.append(linkedAnimatorView)
        return linkedAnimatorViews

    def getLinkedHistograms(self):
        resultList = self.con.cmdTagList("getLinkedHistograms",
                                         imageView=self.getId())
        linkedHistogramViews = []
        if (resultList[0] != ""):
            for histogram in resultList:
                linkedHistogramView = Histogram(histogram, self.con)
                linkedHistogramViews.append(linkedHistogramView)
        return linkedHistogramViews

    def getLinkedStatistics(self):
        resultList = self.con.cmdTagList("getLinkedStatistics",
                                         imageView=self.getId())
        linkedStatisticsViews = []
        if (resultList[0] != ""):
            for statistics in resultList:
                linkedStatisticsView = Statistics(statistics, self.con)
                linkedStatisticsViews.append(linkedStatisticsView)
        return linkedStatisticsViews

    def setClipValue(self, index):
        result = self.con.cmdTagList("setClipValue", imageView=self.getId(),
                                     clipValue=index)
        return result

    def centerOnPixel(self, x, y, z=0):
        """
        Centers the viewer on the pixel at (x, y).
        z is used to select the dimension in a data cube.
        """
        result = self.con.cmdTagList("centerOnPixel", imageView=self.getId(),
                                     xval=x, yval=y)
        if (result[0] != "error"):
            animators = self.getLinkedAnimators()
            if (len(animators) > 0):
                animators[0].setChannel(z)
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
        viewerDim = self.getOutputSize()
        if (len(viewerDim) != 2):
            #return "Could not center the image."
            return viewerDim
        self.centerOnPixel(x, y)
        if dim == 'width':
            dimNumber = 0
        elif dim == 'height':
            dimNumber = 1
        zoom = (float(viewerDim[dimNumber]/2)) / radius
        self.setZoomLevel(zoom)

    def fitToViewer(self):
        oDim = self.getOutputSize()
        iDim = self.getImageDimensions()
        if (iDim[0] != "error" and oDim[0] != "error"):
            if (oDim[0] < oDim[1]):
                self.centerWithRadius(iDim[0]/2,iDim[1]/2,iDim[0]/2,'width')
            else:
                self.centerWithRadius(iDim[0]/2,iDim[1]/2,iDim[1]/2,'height')
            return []
        else:
            return ["Could not fit image to viewer."]

    def zoomToPixel(self, x, y):
        iDim = self.getImageDimensions()
        oDim = self.getOutputSize()
        if (iDim[0] != "error" and oDim[0] != "error"):
            if (oDim[0] < oDim[1]):
                self.centerWithRadius(x+0.5, y+0.5, 0.5, 'width')
            else:
                self.centerWithRadius(x+0.5, y+0.5, 0.5, 'height')
            return []
        else:
            return ["Could not zoom to pixel ("
                    + str(x) + "," + str(y) + ")"]

    def setZoomLevel(self, zoom):
        result = self.con.cmdTagList("setZoomLevel", imageView=self.getId(),
                                     zoomLevel=zoom)
        return result

    def getZoomLevel(self):
        result = self.con.cmdTagList("getZoomLevel", imageView=self.getId())
        if (result[0] != "error"):
            result = float(result[0])
        else:
            result = result[1]
        return result

    def addLink(self, dest):
        """ Note that this method needs to override the base class method
            because the source and destination are flipped."""
        result = self.con.cmdTagList("addLink", sourceView=dest.getId(),
                                     destView=self.getId())
        return result

    def removeLink(self, dest):
        """ Note that this method needs to override the base class method
            because the source and destination are flipped."""
        result = self.con.cmdTagList("removeLink", sourceView=dest.getId(),
                                     destView=self.getId())
        return result

    def saveImage(self, dest):
        result = self.con.cmdTagList("saveImage", imageView=self.getId(),
                                     filename=dest)
        return result

    def saveFullImage(self, dest, width=-1, height=-1, scale=1,
                      aspectRatioMode='ignore'):
        """
        Save a copy of the entire image (not just what's in the image viewer).

        Parameters
        ----------
        dest: string
            The full path of the destination filename.
        width: integer
            The width of the saved image.
            The default value is -1, which causes the parameter to be
                ignored.
        height: integer
            The height of the saved image.
            The default value is -1, which causes the parameter to be
                ignored.
        scale: float
            The desired zoom level of the saved image.
            The default value is 1.
        aspectRatioMode: string
            Can be one of three possible values: 'ignore', 'keep', or
                'expand'. See
                http://doc.qt.io/qt-5/qt.html#AspectRatioMode-enum for an
                explanation of these options.
        """
        if (width < 0 or height < 0):
            currentDim = self.getImageDimensions()
            width = currentDim[0]
            height = currentDim[1]
        result = self.con.cmdAsyncList("saveFullImage", imageView=self.getId(),
                                     filename=dest, width=width, height=height,
                                     scale=scale,
                                     aspectRatioMode=aspectRatioMode)
        return result

    def getImageDimensions(self):
        result = self.con.cmdTagList("getImageDimensions",
                                     imageView=self.getId())
        if (result[0] != "error"):
            result = [int(i) for i in result]
        return result

    def getChannelCount(self):
        """
        A convenience function.
        Returns the number of channels in the image.
        """
        result = 1
        dimensions = self.getImageDimensions()
        if (dimensions[0] != "error"):
            result = dimensions[1]
        else:
            result = dimensions
        return result

    def getOutputSize(self):
        result = self.con.cmdTagList("getOutputSize", imageView=self.getId())
        if (result[0] != "error"):
            result = [int(i) for i in result]
        else:
            result = result[1]
        return result


    def getIntensity(self, frameLow, frameHigh, percentile):
        result = self.con.cmdTagList("getIntensity", imageView=self.getId(),
                                     frameLow=frameLow, frameHigh=frameHigh,
                                     percentile=percentile)
        if (result[0] == "error"):
            return result[1]
        elif (result[0] == False):
            return False
        else:
            return float(result[0])

    def centerOnCoordinate(self, skyCoord):
        """
        Centers the image on an astropy Sky Coordinate object.
        """
        result = self.con.cmdTagList("getPixelCoordinates",
                                     imageView=self.getId(),
                                     ra=skyCoord.ra.radian,
                                     dec=skyCoord.dec.radian)
        self.centerOnPixel( float(result[0]), float(result[1]) )
        return result
