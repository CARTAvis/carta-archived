#!/usr/bin/env python
# -*- coding: utf-8 -*-

from cartaview import CartaView

class Histogram(CartaView):
    """
    Represents a histogram view.
    """

    def setClipBuffer(self, bufferAmount):
        """
        Set the amount of extra space on each side of the clip bounds.

        Parameters
        ----------
        bufferAmount: integer
            A percentage in [0,100) representing the amount of extra
            space.

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
        """
        result = self.con.cmdTagList("setClipBuffer",
                                     histogramView=self.getId(),
                                     bufferAmount=bufferAmount)
        return result

    def setUseClipBuffer(self, useBuffer='toggle'):
        """
        Set whether or not to show extra space on each side of the clip
        bounds.

        Parameters
        ----------
        useBuffer: string
            Can take a value of 'true', 'false', or 'toggle'.
            Can also take a boolean object (i.e. True or False).
            The default value is 'toggle', which causes the extra space
            setting to be toggled.

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
        """
        result = self.con.cmdTagList("setUseClipBuffer",
                                     histogramView=self.getId(),
                                     useBuffer=str(useBuffer))
        return result

    def setClipRange(self, minRange, maxRange):
        """
        Set the lower and upper bounds for the histogram horizontal
        axis.

        Parameters
        ----------
        minRange: float
            A lower bound for the histogram horizontal axis.
        maxRange: float
            An upper bound for the histogram horizontal axis.

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
        """
        result = self.con.cmdTagList("setClipRange",
                                     histogramView=self.getId(),
                                     minRange=minRange, maxRange=maxRange)
        return result

    def setClipRangePercent(self, minPercent, maxPercent):
        """
        Set the lower and upper bounds for the histogram as percentages
        of the entire range.

        Parameters
        ----------
        minPercent: float
            A number in [0,100) representing the amount to leave off on
            the left.
        maxPercent: float
            A number in [0,100) representing the amount to leave off on
            the right.

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
        """
        result = self.con.cmdTagList("setClipRangePercent",
                                     histogramView=self.getId(),
                                     minPercent=minPercent,
                                     maxPercent=maxPercent)
        return result

    def getClipRange(self):
        """
        Get the lower and upper bounds for the histogram horizontal
        axis.

        Returns
        -------
        list
            The lower and upper bounds for the histogram horizontal
            axis.
            Error message if an error occurred.
        """
        result = self.con.cmdTagList("getClipRange", histogramView=self.getId())
        if (result[0] != "error"):
            result = [float(i) for i in result]
        return result

    def applyClips(self):
        """
        Apply clips to the image.

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
        """
        result = self.con.cmdTagList("applyClips", histogramView=self.getId())
        return result

    def setBinCount(self, count):
        """
        Set the number of bins in the histogram.

        Parameters
        ----------
        count: integer
            the number of histogram bins.

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
        """
        result = self.con.cmdTagList("setBinCount",
                                     histogramView=self.getId(),
                                     binCount=count)
        return result

    def setBinWidth(self, width):
        """
        Set the width of the histogram bins.

        Parameters
        ----------
        width: float
            The histogram bin width.

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
        """
        result = self.con.cmdTagList("setBinWidth",
                                     histogramView=self.getId(),
                                     binWidth=width)
        return result

    def setPlaneMode(self, mode):
        """
        Set whether the histogram should be based on a single plane, a
        range of planes, or the entire cube.

        Parameters
        ----------
        mode: string
            A unique identifier for the 3D data range.
            [NOTE: should we provide a list of valid plane modes here?]

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
        """
        result = self.con.cmdTagList("setPlaneMode",
                                     histogramView=self.getId(),
                                     planeMode=mode)
        return result

    def setPlaneRange(self, minPlane, maxPlane):
        """
        Set the range of channels to include as data in generating the
        histogram.

        Parameters
        ----------
        minPlane: float
            The minimum frequency (GHz) to include.
        maxPlane: float
            The maximum frequency (GHz) to include.

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
        """
        result = self.con.cmdTagList("setPlaneRange",
                                     histogramView=self.getId(),
                                     minPlane=minPlane,
                                     maxPlane=maxPlane)
        return result

    def setChannelUnit(self, unit):
        """
        Set the unit used to specify a channel range, for example,
        "GHz".

        Parameters
        ----------
        unit: string
            The channel units used to specify a range.
            [NOTE: should we provide a list of valid units here?]

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
        """
        result = self.con.cmdTagList("setChannelUnit",
                                     histogramView=self.getId(),
                                     unit=unit)
        return result

    def setGraphStyle(self, style):
        """
        Set the drawing style for the histogram (outline, filled, etc).

        Parameters
        ----------
        style: string
            A unique identifier for a histogram drawing style.
            Valid values are 'line', 'outline', and 'fill'.

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
        """
        result = self.con.cmdTagList("setGraphStyle",
                                     histogramView=self.getId(),
                                     graphStyle=style)
        return result

    def setLogCount(self, logCount='toggle'):
        """
        Set whether or not the histogram's vertical axis should use a
        logarithmic scale.

        Parameters
        ----------
        logCount: string
            Can take a value of 'true', 'false', or 'toggle'.
            Can also take a boolean object (i.e. True or False).
            The default value is 'toggle', which causes the logarithmic
            scale setting to be toggled.

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
        """
        result = self.con.cmdTagList("setLogCount",
                                     histogramView=self.getId(),
                                     logCount=str(logCount))
        return result

    def setColored(self, colored='toggle'):
        """
        Set whether or not the histogram should be colored by
        intensity.

        Parameters
        ----------
        colored: string
            Can take a value of 'true', 'false', or 'toggle'.
            Can also take a boolean object (i.e. True or False).
            The default value is 'toggle', which causes the colored
            setting to be toggled.

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
        """
        result = self.con.cmdTagList("setColored",
                                     histogramView=self.getId(),
                                     colored=str(colored))
        return result

    def saveHistogram(self, filename, width=-1, height=-1,
                      aspectRatioMode='ignore'):
        """
        Save a copy of the histogram as an image.

        Parameters
        ----------
        filename: string
            The full path where the file is to be saved.
        width: integer
            The width, in pixels, of the saved image.
            The default value is -1, which causes the dimension
            parameters to be ignored; in this case, the resulting image
            will be an exact copy of the histogram in the GUI.
        height: integer
            The height, in pixels, of the saved image.
            The default value is -1, which causes the dimension
            parameters to be ignored; in this case, the resulting image
            will be an exact copy of the histogram in the GUI.
        aspectRatioMode: string
            Can be one of three possible values: 'ignore', 'keep', or
            'expand'. See
            http://doc.qt.io/qt-5/qt.html#AspectRatioMode-enum for an
            explanation of these options.
            The default value is 'ignore'.

        Returns
        -------
        list
            Error message if an error occurred; empty otherwise.
        """
        result = self.con.cmdTagList("saveHistogram",
                                     filename=filename,
                                     histogramView=self.getId(),
                                     width=width, height=height,
                                     aspectRatioMode=aspectRatioMode)
        return result
