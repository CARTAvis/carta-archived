#!/usr/bin/env python
# -*- coding: utf-8 -*-

from astropy.coordinates import SkyCoord

from cartaview import CartaView
from histogram import Histogram
from animator import Animator
from colormap import Colormap

class Image(CartaView):
    """
    Represents an image view.
    """

    def loadFile(self, fileName):
        """
        Load a file into the image view.

        Parameters
        ----------
        filename: string
            A path identifying the file to be loaded.

        Returns
        -------
        list
            An error message if there was a problem loading the file,
            and a list containing  the server-side
            id of object holding the image if it was successfully loaded.
        """
        result = self.con.cmdTagList("loadFile", imageView=self.getId(),
                                     fname=fileName)
        return result

    def getLinkedColormaps(self):
        """
        Get the colormaps that are linked to this image view.

        Returns
        -------
        list
            A list of Colormap objects.
        """
        resultList = self.con.cmdTagList("getLinkedColormaps",
                                         imageView=self.getId())
        linkedColormapViews = []
        if (resultList[0] != ""):
            for colormap in resultList:
                linkedColormapView = Colormap(colormap, self.con)
                linkedColormapViews.append(linkedColormapView)
        return linkedColormapViews

    def getLinkedAnimators(self):
        """
        Get the animators that are linked to this image view.

        Returns
        -------
        list
            A list of Animator objects.
        """
        resultList = self.con.cmdTagList("getLinkedAnimators",
                                         imageView=self.getId())
        linkedAnimatorViews = []
        if (resultList[0] != ""):
            for animator in resultList:
                linkedAnimatorView = Animator(animator, self.con)
                linkedAnimatorViews.append(linkedAnimatorView)
        return linkedAnimatorViews

    def getLinkedHistograms(self):
        """
        Get the histograms that are linked to this image view.

        Returns
        -------
        list
            A list of Histogram objects.
        """
        resultList = self.con.cmdTagList("getLinkedHistograms",
                                         imageView=self.getId())
        linkedHistogramViews = []
        if (resultList[0] != ""):
            for histogram in resultList:
                linkedHistogramView = Histogram(histogram, self.con)
                linkedHistogramViews.append(linkedHistogramView)
        return linkedHistogramViews

    def setClipValue(self, index):
        """
        Set the histogram to show the specified percentage of the data.

        Parameters
        ----------
        index: float
            The percentage of data to be shown.

        Returns
        -------
        list
            Error information if the clip value could not be set.
        """
        result = self.con.cmdTagList("setClipValue", imageView=self.getId(),
                                     clipValue=index)
        return result

    def centerOnPixel(self, x, y, z=0):
        """
        Centers the viewer on a pixel.

        Parameters
        ----------
        x: float
            The x value of the pixel to center on.
        y: float
            The y value of the pixel to center on.
        z: integer
            The z value of the pixel to center on.
            The default value is 0, which causes the parameter to be
            ignored.

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
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

            something that takes a centre and a radius and computes a
            ZoomLevel to put the centre at the centre of the viewer
            window and have the distance to the edge of the window be
            the "radius".

        Note that this function is defined entirely in terms of other,
        lower level Python functions.

        Parameters
        ----------
        x: float
            The x value of the pixel to center on.
        y: float
            The y value of the pixel to center on.
        radius: float
            The desired distance, in pixels, from the center pixel to
            the edge of the window.
        dim: string
            The dimension of the image viewer to use for the radius.
            Can be equal to either 'width' or 'height'.
            The default value is 'width'.

        Returns
        -------
        list
            Error message if an error occurred; nothing otherwise.
        """
        viewerDim = self.getOutputSize()
        if (viewerDim[0] == "error"):
            return viewerDim
        self.centerOnPixel(x, y)
        if dim == 'width':
            dimNumber = 0
        elif dim == 'height':
            dimNumber = 1
        zoom = (float(viewerDim[dimNumber]/2)) / radius
        self.setZoomLevel(zoom)

    def fitToViewer(self):
        """
        Centers the image and sets the zoom level so that the entire
        image fits within the viewer.
        A convenience function that is actually a special case of
        centerWithRadius().

        Returns
        -------
        list
            Error message if an error occurred; nothing otherwise.
        """
        oDim = self.getOutputSize()
        iDim = self.getImageDimensions()
        if (iDim[0] != "error" and oDim[0] != "error"):
            if (oDim[0] < oDim[1]):
                self.centerWithRadius(iDim[0]/2 - 0.5,
                                      iDim[1]/2 - 0.5,
                                      iDim[0]/2,
                                      'width')
            else:
                self.centerWithRadius(iDim[0]/2 - 0.5,
                                      iDim[1]/2-0.5,
                                      iDim[1]/2,
                                      'height')
            return []
        else:
            return ["Could not fit image to viewer."]

    def zoomToPixel(self, x, y):
        """
        Sets the zoom level so that a single pixel fills the entire
        image viewer.
        A convenience function that is actually a special case of
        centerWithRadius().

        Parameters
        ----------
        x: float
            The x value of the pixel to center on.
        y: float
            The y value of the pixel to center on.

        Returns
        -------
        list
            Error message if an error occurred; nothing otherwise.
        """
        iDim = self.getImageDimensions()
        oDim = self.getOutputSize()
        if (iDim[0] != "error" and oDim[0] != "error"):
            if (oDim[0] < oDim[1]):
                self.centerWithRadius(x, y, 0.5, 'width')
            else:
                self.centerWithRadius(x, y, 0.5, 'height')
            return []
        else:
            return ["Could not zoom to pixel ("
                    + str(x) + "," + str(y) + ")"]

    def setZoomLevel(self, zoom):
        """
        Sets the zoom level, which is defined as the rendered size of a
        single data pixel in the image on the screen.

        e.g. a zoom level of 1e-9 means that a single data pixel in the
        image would occupy 1e-9 of a screen pixel, i.e. it would allow
        you to zoom out an image with dimensions of 1,000,000,000 x
        1,000,000,000 pixels so that it would fit into a single pixel on
        the screen. A zoom level of 1e+9 would expand a single data
        pixel to a billion by a billion screen pixels.

        Parameters
        ----------
        zoom: float
            The desired zoom level.

        Returns
        -------
        list
            Error message if an error occurred; nothing otherwise.
        """
        result = self.con.cmdTagList("setZoomLevel", imageView=self.getId(),
                                     zoomLevel=zoom)
        return result

    def getZoomLevel(self):
        """
        Get the current zoom level.

        Returns
        -------
        list
            The zoom level, or error information if it could not be
            obtained.
        """
        result = self.con.cmdTagList("getZoomLevel", imageView=self.getId())
        if (result[0] != "error"):
            result = float(result[0])
        else:
            result = result[1]
        return result

    def resetZoom(self):
        """
        Reset the zoom to its original value.

        Returns
        -------
        list
            Error message if an error occurred; nothing otherwise.
        """
        result = self.con.cmdTagList("resetZoom", imageView=self.getId())
        return result

    def centerImage(self):
        """
        Center the image.

        Returns
        -------
        list
            Error message if an error occurred; nothing otherwise.
        """
        result = self.con.cmdTagList("centerImage", imageView=self.getId())
        return result

    def getCenterPixel(self):
        """
        Get the image pixel that is currently centered.

        Returns
        -------
        list
            Two floating point values representing the currently
            centered x- and y-values, respectively, of the image.
            Error information if the center pixel could not be obtained
            (e.g. if there is no image currently loaded).
        """
        result = self.con.cmdTagList("getCenterPixel", imageView=self.getId())
        if (result[0] != "error"):
            result = [float(x) for x in result]
        return result

    def addLink(self, dest):
        """
        Establish a link between this image viewer and a destination
        object.
        The parameters are CartaView objects that are obtained by
        commands such as getHistogramViews() and getColormapViews(). For
        example, the following sequence of commands will obtain the
        image views and histogram views, then add a link between the
        first image view and the first histogram view:

            i = v.getImageViews()
            h = v.getHistogramViews()
            i[0].addLink(h[0])

        Parameters
        ----------
        dest: CartaView object
            The object to link to this image viewer.

        Returns
        -------
        list
            An error message if the link does not succeed, and nothing
            otherwise.
        """
        # Note that this method needs to override the base class method
        # because the source and destination are flipped.
        result = self.con.cmdTagList("addLink", sourceView=dest.getId(),
                                     destView=self.getId())
        return result

    def removeLink(self, dest):
        """
        Remove a link between this image viewer and a destination
        object.
        The parameters are CartaView objects that are obtained by
        commands such as getHistogramViews() and getColormapViews(). For
        example, the following sequence of commands will obtain the
        image views and histogram views, then remove the link between
        the first image view and the first histogram view:

            i = v.getImageViews()
            h = v.getHistogramViews()
            i[0].removeLink(h[0])

        Parameters
        ----------
        dest: CartaView object
            The object to remove the link from.

        Returns
        -------
        list
            An error message if the link removal does not succeed, and
            nothing otherwise.
        """
        # Note that this method needs to override the base class method
        # because the source and destination are flipped.
        result = self.con.cmdTagList("removeLink", sourceView=dest.getId(),
                                     destView=self.getId())
        return result



    def saveImage(self, dest, width=-1, height=-1,
                      aspectRatioMode='ignore', fullImage=False):
        """
        Save a copy of the entire image (not just what is visible in the
        image viewer).

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
        aspectRatioMode: string
            Can be one of three possible values: 'ignore', 'keep', or
            'expand'. See
            http://doc.qt.io/qt-5/qt.html#AspectRatioMode-enum for an
            explanation of these options.
            The default value is 'ignore'.
        fullImage : bool
            True means that the full image will be saved; false means that the
            current view of the image will be saved.

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
        """
        if (width < 0 or height < 0):
            outputSize = self.getOutputSize()
            width = outputSize.width
            height = outputSize.height
        result = self.con.cmdAsyncList("saveImage", imageView=self.getId(),
                                     filename=dest, width=width, height=height,
                                     aspectRatioMode=aspectRatioMode,
                                     fullImage = fullImage)
        return result

    def getImageDimensions(self):
        """
        Get the dimensions of the image.

        Returns
        -------
        list
            A list of integers representing the x, y, and z dimensions
            of the image, or error information if the dimensions could
            not be obtained.
        """
        result = self.con.cmdTagList("getImageDimensions",
                                     imageView=self.getId())
        if (result[0] != "error"):
            result = [int(i) for i in result]
        return result

    def getChannelCount(self):
        """
        Get the number of channels in the image.

        Returns
        -------
        integer
            The number of channels in the image.
            Error information if the number of channels could not be
            obtained.
        """
        result = self.con.cmdTagList("getChannelCount", imageView=self.getId())
        if (result[0] != "error"):
            result = int(result[0])
        else:
            result = result[1]
        return result

    def getOutputSize(self):
        """
        Get the dimensions of the image viewer (window size).

        Returns
        -------
        list
            A list of integers representing the x and y dimensions of
            the image viewer.
            Error information if the image viewer dimensions could not
            be obtained.
        """
        result = self.con.cmdTagList("getOutputSize", imageView=self.getId())
        if (result[0] != "error"):
            result = [int(i) for i in result]
        return result

    def getIntensity(self, frameLow, frameHigh, percentile):
        """
        Returns the intensity corresponding to a given percentile.

        Parameters
        ----------
        frameLow: integer
            A lower bound for the image channels.
        frameHigh: integer
            An upper bound for the image channels.
        percentile: double
            A number [0,1] for which an intensity is desired.

        Returns
        -------
        list
            The intensity value or an error message if the intensity
            could not be obtained.
        """
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
        Centers the image on an Astropy SkyCoord object.

        Astropy needs to be installed for this command to work. See
        http://www.astropy.org for more information about Astropy and
        how to install it.

        For example, the following sequence of commands will focus the
        image viewer on the coordinates of M33 (regardless of whether
        or not M33 is contained within the current image).

            from astropy.coordinates import SkyCoord
            i = v.getImageViews()
            i[0].centerOnCoordinate(SkyCoord.from_name("M33"))

        Parameters
        ----------
        skyCoord: Astropy SkyCoord object
            The object to center the image on.

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
        """
        result = self.con.cmdTagList("getPixelCoordinates",
                                     imageView=self.getId(),
                                     ra=skyCoord.ra.radian,
                                     dec=skyCoord.dec.radian)
        if (result[0] == "error" or result[0] == ''):
            result = ["Could not obtain pixel coordinates for " + str(skyCoord)]
        else:
            self.centerOnPixel(float(result[0]), float(result[1]))
        return result
        
    def getPixelValue(self, x, y):
        """
        Get the value of a pixel.

        Parameters
        ----------
        x: float
            The x value of the desired pixel.
        y: float
            The y value of the desired pixel.

        Returns
        -------
        list
            The value of the pixel at (x, y), or an empty string if
            there is no valid value at (x, y).
        """
        result = self.con.cmdTagList("getPixelValue", imageView=self.getId(),
                                     x=x, y=y)
        return result

    def getPixelUnits(self):
        """
        Get the units of the pixels in the currently loaded image.

        Returns
        -------
        list
            The units of the pixels, or an empty string if no units are
            defined.
        """
        result = self.con.cmdTagList("getPixelUnits", imageView=self.getId())
        return result

    def getCoordinates(self, x, y, system):
        """
        Get the coordinates at a given pixel in a given coordinate
        system.

        Parameters
        ----------
        x: float
            The x value of the desired pixel.
        y: float
            The y value of the desired pixel.
        system: string
            The desired coordinate system.
            Acceptable systems are:

                - J2000
                - B1950
                - ICRS
                - Galactic
                - Ecliptic

        Returns
        -------
        list
            The x- and y-coordinates in the desired coordinate system.
        """
        result = self.con.cmdTagList("getCoordinates", imageView=self.getId(),
                                     x=x, y=y, system=system)
        return result

    def getImageNames(self):
        """
        Returns a list of images open in this image view.

        Returns
        -------
        list
            The names of the images that are currently open in this
            image view.
        """
        result = self.con.cmdTagList("getImageNames", imageView=self.getId())
        imageNames = []
        if ( len(result)> 0):
            if (result[0] != ""):
                imageNames.extend( result)
        return imageNames

    def closeImage(self, imageName):
        """
        Close the specified image.

        Parameters
        ----------
        imageName: string
            The filename of the image to close.

        Returns
        -------
        list
            Error information if the specified image could not be
            closed.
        """
        result = self.con.cmdTagList("closeImage", imageView=self.getId(),
                                     imageName=imageName)
        return result

    def setGridAxesColor(self, red, green, blue):
        """
        Set the color of the grid axes.

        Parameters
        ----------
        red: integer
            An integer in [0, 255] indicating the amount of red.
        green: integer
            An integer in [0, 255] indicating the amount of green.
        blue: integer
            An integer in [0, 255] indicating the amount of blue.

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
        """
        result = self.con.cmdTagList("setGridAxesColor",
                                     imageView=self.getId(),
                                     red=red, green=green, blue=blue)
        return result

    def setGridAxesThickness(self, thickness):
        """
        Set grid axis thickness.

        Parameters
        ----------
        thickness: integer
            A positive integer.

        Returns
        -------
        list
            An error message if the thickness could not be set or an
            empty string if the thickness was successfully set.
        """
        result = self.con.cmdTagList("setGridAxesThickness",
                                     imageView=self.getId(),
                                     thickness=thickness)
        return result

    def setGridAxesTransparency(self, transparency):
        """
        Parameters
        ----------
        transparency: integer
            A nonnegative integer between 0 and 255, with 255 opaque.

        Returns
        -------
        list
            An error message if the transparency could not be set or an
            empty string if it was successfully set.
        """
        result = self.con.cmdTagList("setGridAxesTransparency",
                                     imageView=self.getId(),
                                     transparency=transparency)
        return result

    def setGridApplyAll(self, applyAll):
        """
        Set whether or not grid control settings should apply to all
        images on the set.

        Parameters
        ----------
        applyAll: boolean
            True if the settings apply to all images on the stack;
            False otherwise.

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
        """
        result = self.con.cmdTagList("setGridApplyAll",
                                     imageView=self.getId(),
                                     applyAll=applyAll)
        return result

    def setGridCoordinateSystem(self, coordSystem):
        """
        Set the grid coordinate system.

        Parameters
        ----------
        coordSystem: string
            An identifier for a grid coordinate system.

        Returns
        -------
        list
            An error message if there was a problem setting the
            coordinate system; an empty string otherwise.
        """
        result = self.con.cmdTagList("setGridCoordinateSystem",
                                     imageView=self.getId(),
                                     coordSystem=coordSystem)
        return result

    def setGridFontFamily(self, fontFamily):
        """
        Set the font family used for grid labels.

        Parameters
        ----------
        fontFamily: string
            An identifier for a font family.

        Returns
        -------
        list
            An error message if there was a problem setting the font
            family; an empty string otherwise.
        """
        result = self.con.cmdTagList("setGridFontFamily",
                                     imageView=self.getId(),
                                     fontFamily=fontFamily)
        return result

    def setGridFontSize(self, fontSize):
        """
        Set the font size used for grid labels.

        Parameters
        ----------
        fontSize: integer
            An identifier for a font point size.

        Returns
        -------
        list
            An error message if there was a problem setting the font
            point size; an empty string otherwise.
        """
        result = self.con.cmdTagList("setGridFontSize",
                                     imageView=self.getId(),
                                     fontSize=fontSize)
        return result

    def setGridColor(self, redAmount, greenAmount, blueAmount):
        """
        Set the grid color.

        Parameters
        ----------
        redAmount: integer
            An integer in [0, 255] indicating the amount of red.
        greenAmount: integer
            An integer in [0,255] indicating the amount of green.
        blueAmount: integer
            An integer in [0,255] indicating the amount of blue.

        Returns
        -------
        list
            A list of errors or an empty list if the color was
            successfully set.
        """
        result = self.con.cmdTagList("setGridColor",
                                     imageView=self.getId(),
                                     redAmount=redAmount,
                                     greenAmount=greenAmount,
                                     blueAmount=blueAmount)
        return result

    def setGridSpacing(self, spacing):
        """
        Set the spacing between grid lines.

        Parameters
        ----------
        spacing: float
            The grid spacing in [0,1] with 1 having the least amount of
            spacing.

        Returns
        -------
        list
            An error message if there was a problem setting the grid
            spacing; an empty string otherwise.
        """
        result = self.con.cmdTagList("setGridSpacing",
                                     imageView=self.getId(),
                                     spacing=spacing)
        return result

    def setGridThickness(self, thickness):
        """
        Set the thickness of the grid lines.

        Parameters
        ----------
        thickness: integer
            A positive integer.

        Returns
        -------
        list
            An error message if there was a problem setting the grid
            line thickness; an empty string otherwise.
        """
        result = self.con.cmdTagList("setGridThickness",
                                     imageView=self.getId(),
                                     thickness=thickness)
        return result

    def setGridTransparency(self, transparency):
        """
        Set the transparency of the grid.

        Parameters
        ----------
        transparency: integer
            The amount of transparency in [0,255] with 255 completely
            opaque.

        Returns
        -------
        list
            An error message if there was a problem setting the
            transparency; an empty string otherwise.
        """
        result = self.con.cmdTagList("setGridTransparency",
                                     imageView=self.getId(),
                                     transparency=transparency)
        return result

    def setGridLabelColor(self, redAmount, greenAmount, blueAmount):
        """
        Set the color of grid labels.

        Parameters
        ----------
        redAmount: integer
            An integer in [0, 255] indicating the amount of red.
        greenAmount: integer
            An integer in [0,255] indicating the amount of green.
        blueAmount: integer
            An integer in [0,255] indicating the amount of blue.

        Returns
        -------
        list
            A list of errors or an empty list if the color was
            successfully set.
        """
        result = self.con.cmdTagList("setGridLabelColor",
                                     imageView=self.getId(),
                                     redAmount=redAmount,
                                     greenAmount=greenAmount,
                                     blueAmount=blueAmount)
        return result

    def setShowGridAxis(self, showAxis):
        """
        Set whether or not the grid axes should be shown.

        Parameters
        ----------
        showAxis: boolean
            True if the axes should be shown; False otherwise.

        Returns
        -------
        list
            an error message if there was a problem changing the
            visibility of the axes; an empty string otherwise.
        """
        result = self.con.cmdTagList("setShowGridAxis",
                                     imageView=self.getId(),
                                     showAxis=showAxis)
        return result

    def setShowGridCoordinateSystem(self, showCoordinateSystem):
        """
        Set whether or not the grid coordinate system should be visible.

        Parameters
        ----------
        showCoordinateSystem: boolean
            True if the coordinate system should be shown; False
            otherwise.

        Returns
        -------
        list
            An error message if there was a problem setting the
            coordinate system; an empty string otherwise.
        """
        result = self.con.cmdTagList("setShowGridCoordinateSystem",
                                     imageView=self.getId(),
                                     showCoordinateSystem=showCoordinateSystem)
        return result

    def setShowGridLines(self, showGridLines=True):
        """
        Set whether or not to show grid lines.

        Parameters
        ----------
        showGridLines: boolean
            The default value is True, which caues the grid lines to be
            shown.

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
        """
        result = self.con.cmdTagList("setShowGridLines",
                                     imageView=self.getId(),
                                     showGridLines=showGridLines)
        return result

    def setShowGridInternalLabels(self, showInternalLabels):
        """
        Set whether or not the grid axis should be internal or external.

        Parameters
        ----------
        showInternalLabels: boolean
            True if the axes should be internal; False otherwise.

        Returns
        -------
        list
            An error message if there was a problem setting the axes
            internal/external; false otherwise.
        """
        result = self.con.cmdTagList("setShowGridInternalLabels",
                                     imageView=self.getId(),
                                     showInternalLabels=showInternalLabels)
        return result

    def setShowGridStatistics(self,  showStatistics):
        """
        Sets whether or not cursor position image statistics should be
        shown.

        Parameters
        ----------
        showStatistics: boolean
            True if cursor statistics should be shown; False otherwise.

        Returns
        -------
        list
            An error message if there was a problem hiding/showing
            cursor statistics; an empty string otherwise.
        """
        result = self.con.cmdTagList("setShowGridStatistics",
                                     imageView=self.getId(),
                                     showStatistics=showStatistics)
        return result

    def setShowGridTicks(self, showTicks):
        """
        Set whether or not to show grid axis ticks.

        Parameters
        ----------
            showTicks: boolean
                True if the grid axis ticks should be shown; False
                otherwise.

        Returns
        -------
        list
            An error message if there was a problem setting the
            visibility of grid axis ticks; an empty string otherwise.
        """
        result = self.con.cmdTagList("setShowGridTicks",
                                     imageView=self.getId(),
                                     showTicks=showTicks)
        return result

    def setGridTickColor(self, redAmount, greenAmount, blueAmount):
        """
        Set the color of the grid tick marks.

        Parameters
        ----------
        redAmount: integer
            A nonnegative integer in [0,255].
        greenAmount: integer
            A nonnegative integer in [0,255].
        blueAmount: integer
            A nonnegative integer in [0,255].

        Returns
        -------
        list
            A list of error message(s) if there was a problem setting
            the tick color; an empty list otherwise.
        """
        result = self.con.cmdTagList("setGridTickColor",
                                     imageView=self.getId(),
                                     redAmount=redAmount,
                                     greenAmount=greenAmount,
                                     blueAmount=blueAmount)
        return result

    def setGridTickThickness(self, tickThickness):
        """
        Set the length of the grid ticks.

        Parameters
        ----------
        tickThickness: integer
            A positive integer.

        Returns
        -------
        list
            Return an error message if the tick thickness was not
            successfully set; an empty string otherwise.
        """
        result = self.con.cmdTagList("setGridTickThickness",
                                     imageView=self.getId(),
                                     tickThickness=tickThickness)
        return result

    def setGridTickTransparency(self, transparency):
        """
        Set the transparency of the grid tick marks.

        Parameters
        ----------
        transparency: integer
            A nonnegative integer between 0 and 255 with 255 being
            opaque.

        Returns
        -------
        list
            An error message if the transparency was not successfully
            set; an empty string otherwise.
        """
        result = self.con.cmdTagList("setGridTickTransparency",
                                     imageView=self.getId(),
                                     transparency=transparency)
        return result

    def setGridTheme(self, theme):
        """
        Set the canvas theme.

        Parameters
        ----------
        theme: string
            An identifier for a canvas theme.

        Returns
        -------
        list
            An error message if the theme was not successfully set; an
            empty string otherwise.
        """
        result = self.con.cmdTagList("setGridTheme", imageView=self.getId(),
                                     theme=theme)
        return result

    def deleteContourSet(self, name):
        """
        Delete the contour set with the indicated name.

        Parameters
        ----------
        name: string
            A unique identifier for a contour set.

        Returns
        -------
        list
            An error message if the contour set could not be deleted; an
            empty string otherwise.
        """
        result = self.con.cmdTagList("deleteContourSet",
                                     imageView=self.getId(), name=name)
        return result

    def generateContourSet(self, name):
        """
        Generate a set of contours with the given name.

        Parameters
        ----------
        name: string
            The name of the contour set to generate.

        Returns
        -------
        list
            An error message if there was a problem generating the
            contour set; an empty string otherwise.
        """
        result = self.con.cmdTagList("generateContourSet",
                                     imageView=self.getId(), name=name)
        return result

    def selectContourSet(self, name):
        """
        Select a specific contour set.

        Parameters
        ----------
        name: string
            A name for a contour set.

        Returns
        -------
        list
            An error message if the contour set could not be selected;
            an empty string otherwise.
        """
        result = self.con.cmdTagList("selectContourSet",
                                     imageView=self.getId(), name=name)
        return result

    def setContourAlpha(self, contourName, levels, transparency):
        """
        Set the transparency level of the contours within the specified
        set.

        Parameters
        ----------
        contourName: string
            An identifier for a contour set.
        levels: list of floats
            The contours levels within the set whose transparency should
            be changed.
        transparency: integer
            The transparency between 0 and 255.

        Returns
        -------
        list
            An error message if the transparency could not be set;
            otherwise, an empty string.
        """
        result = self.con.cmdTagList("setContourAlpha",
                                     imageView=self.getId(),
                                     contourName=contourName, levels=levels,
                                     transparency=transparency)
        return result

    def setContourColor(self, contourName, levels, red, green, blue):
        """
        Set the color of the contours within the specified set.

        Parameters
        ----------
        contourName: string
            An identifier for a contour set.
        levels: list of floats
            The contours levels within the set affected by the change.
        red: int
            The amount of red between 0 and 255.
        green: int
            The amount of green between 0 and 255.
        blue: int
            The amount of blue between 0 and 255.

        Returns
        -------
        list
            A list of errors if the color could not be set; otherwise,
            an empty list.
        """
        result = self.con.cmdTagList("setContourColors",
                                     imageView=self.getId(),
                                     contourName=contourName, levels=levels,
                                     red=red, green=green, blue=blue)
        return result

    def setContourDashedNegative(self, useDash):
        """
        Set whether or not negative contours should be dashed.

        Parameters
        ----------
        useDash: boolean
            True if negative contours should be dashed; false if they
            should be solid lines.

        Returns
        -------
        list
            An error message if the dashed negative status could not be
            set; an empty string otherwise.
        """
        result = self.con.cmdTagList("setContourDashedNegative",
                                     imageView=self.getId(), useDash=useDash)
        return result

    def setContourGenerateMethod(self, method):
        """
        Set the method used to generate contour levels within the set.

        Parameters
        ----------
        method: string
            An identifier for a method used to generate contour levels.

        Returns
        -------
        list
            An error message if there was a problem setting the method
            used to generate contour levels; an empty string otherwise.
        """
        result = self.con.cmdTagList("setContourGenerateMethod",
                                     imageView=self.getId(), method=method)
        return result

    def setContourSpacing(self, method):
        """
        Set the type of spacing to use between contour levels
        (linear, logarithmic, etc).

        Parameters
        ----------
        method: string
            An identifier for the spacing to use between contour levels.

        Returns
        -------
        list
            An error message if there was a problem setting the spacing;
            an empty string otherwise.
        """
        result = self.con.cmdTagList("setContourSpacing",
                                     imageView=self.getId(), method=method)
        return result

    def setContourLevelCount(self, count):
        """
        Set the number of contour levels in the set.

        Parameters
        ----------
        count: int
            The number of contour levels.

        Returns
        -------
        list
            An error message if the count could not be set; an empty
            string otherwise.
        """
        result = self.con.cmdTagList("setContourLevelCount",
                                     imageView=self.getId(), count=count)
        return result

    def setContourLevelMax(self, value):
        """
        Set the largest contour level.

        Parameters
        ----------
        value: double
            The intensity or percentage of the largest contour level.

        Returns
        -------
        list
            An error message if the largest contour level could not be
            set; an empty string otherwise.
        """
        result = self.con.cmdTagList("setContourLevelMax",
                                     imageView=self.getId(), value=value)
        return result

    def setContourLevelMin(self, value):
        """
        Set the smallest contour level.

        Parameters
        ----------
        value: double
            The intensity or percentage of the smallest contour level.

        Returns
        -------
        list
            An error message if the smallest contour level could not be
            set; an empty string otherwise.
        """
        result = self.con.cmdTagList("setContourLevelMin",
                                     imageView=self.getId(), value=value)
        return result

    def  setContourLevels(self, contourName, levels):
        """
        Update the contour levels within the given contour set.

        Parameters
        ----------
        contourName: string
            The name of a contour set.
        levels: list
            An updated list of contour levels.

        Returns
        -------
        list
            An error message if the contour levels could not be updated;
            otherwise, an empty string.
        """
        result = self.con.cmdTagList("setContourLevels",
                                     imageView=self.getId(),
                                     contourName=contourName, levels=levels)
        return result

    def showImage(self, imageName):
        """
        Show the image.

        Returns
        -------
        list
            Error message if an error occurred; nothing otherwise.
        """
        result = self.con.cmdTagList("showImage", imageName, imageView=self.getId())
        return result

    def hideImage(self, imageName):
        """
        Hide the image.

        Returns
        -------
        list
            Error message if an error occurred; nothing otherwise.
        """
        result = self.con.cmdTagList("hideImage",  imageName, imageView=self.getId())
        return result

    def setStackSelectAuto(self, stackSelectFlag = 'true'):
        """
        Set whether or not selection of layers in the stack should be based on the
        current layer or whether the user wants to make a manual selection.

        Returns
        -------
        list
            Error message if an error occurred; nothing otherwise.
        """
        result = self.con.cmdTagList("setStackSelectAuto", imageView=self.getId(), stackSelectFlag=stackSelectFlag)
        return result

    def setPanZoomAll(self, setPanZoomAllFlag = 'true'):
        """
        Set whether or not a pan/zoom operation should affect all layers in the stack
        or just the top layer.

        Returns
        -------
        list
            Error message if an error occurred; nothing otherwise.
        """
        result = self.con.cmdTagList("setPanZoomAll", imageView=self.getId(), setPanZoomAllFlag=setPanZoomAllFlag)
        return result

    def setMaskAlpha(self, imageName, alphaAmount = 0):
        """
        Set the transparency of the layer.
        alphaAmount - the transparency level in [0,255] with 255 being opaque.

        Returns
        -------
        list
            Error message if an error occurred; nothing otherwise.
        """
        result = self.con.cmdTagList("setMaskAlpha", imageView=self.getId(), imageName=imageName, alphaAmount=alphaAmount)
        return result

    def setMaskColor(self, imageName, redAmount = 0, greenAmount = 0, blueAmount = 0):
        """
        Set the color to use for the mask.
        redAmount - the amount of red in [0,255].
        greenAmount - the amount of green in [0,255].
        blueAmount - the amount of blue in [0,255].

        Returns
        -------
        list
            Error message if an error occurred; nothing otherwise.
        """
        result = self.con.cmdTagList("setMaskAlpha", imageView=self.getId(), imageName=imageName, redAmount=redAmount, greenAmount=greenAmount, blueAmount=blueAmount)
        return result

    def setCompositionMode(self, imageName ):
        """
        Set whether or not to apply a composition mode to the image

        Returns
        -------
        list
            Error message if an error occurred; nothing otherwise.
        """
	    result = self.con.cmdTagList("setCompositionMode", imageView=self.getId(), imageName=imageName)
        return result

    def isEmpty(self):
        """
        Checks whether or not the image view is empty.

        Returns
        -------
        boolean
            True if the image view is empty.
            False if there is at least one image loaded in the image
            view.
        """
        images = self.getImageNames()
        if (images):
            return False
        else:
            return True
