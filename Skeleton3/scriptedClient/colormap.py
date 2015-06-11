#!/usr/bin/env python
# -*- coding: utf-8 -*-

from cartaview import CartaView

class Colormap(CartaView):
    """
    Represents a colormap view.
    """

    def setColormap(self, colormap):
        """
        Set the color map that is being displayed.

        Parameters
        ----------
        colormap: string
            The name of the colormap.

        Returns
        -------
        list
            Error information if there was a problem setting the colormap.
        """
        result = self.con.cmdTagList("setColormap", colormapId=self.getId(),
                                     colormapName=colormap)
        return result

    def reverseColormap(self, reverseStr='toggle'):
        """
        Reverse the colormap.

        Parameters
        ----------
        reverseStr: string
            Can take  value of 'true', 'false', or 'toggle'.
            Can also take a boolean object (i.e. True or False).
            The default value is 'toggle', which causes the reversed state of
            the colormap to be toggled.

        Returns
        -------
        list
            Error information if there was a problem reversing the colormap.
        """
        result = self.con.cmdTagList("reverseColormap",
                                     colormapId=self.getId(),
                                     reverseString=str(reverseStr))
        return result

    def setCacheColormap(self, cacheStr='toggle'):
        """
        This command has been deprecated.
        """
        result = self.con.cmdTagList("setCacheColormap",
                                     colormapId=self.getId(),
                                     cacheString=str(cacheStr))
        return result

    def setCacheSize(self, cacheSize):
        """
        This command has been deprecated.
        """
        result = self.con.cmdTagList("setCacheSize", colormapId=self.getId(),
                                     size=str(cacheSize))
        return result

    def setInterpolatedColormap(self, interpolatedStr='toggle'):
        """
        This command has been deprecated.
        """
        result = self.con.cmdTagList("setInterpolatedColormap",
                                     colormapId=self.getId(),
                                     interpolatedString=str(interpolatedStr))
        return result

    def invertColormap(self, invertStr='toggle'):
        """
        Invert the colormap.

        Parameters
        ----------
        invertStr: string
            Can take  value of 'true', 'false', or 'toggle'.
            Can also take a boolean object (i.e. True or False).
            The default value is 'toggle', which causes the inverted state of
            the colormap to be toggled.

        Returns
        -------
        list
            Error information if there was a problem inverting the colormap.
        """
        result = self.con.cmdTagList("invertColormap",
                                     colormapId=self.getId(),
                                     invertString=str(invertStr))
        return result

    def setColorMix(self, red, green, blue):
        """
        Set a color mix.

        Parameters
        ----------
        red: double
            The amount of red in the mix [0,1].
        green: double
            The amount of green in the mix [0,1].
        blue: double
            The amount of blue in the mix [0,1].

        Returns
        -------
        list
            Error information if the color mix was not successfully set.
        """
        result = self.con.cmdTagList("setColorMix", colormapId=self.getId(),
                                     red=red, green=green, blue=blue)
        return result

    def setGamma(self, gamma):
        """
        Set the gamma color map parameter.

        Parameters
        ----------
        gamma: double
            A parameter for color mapping.

        Returns
        -------
        list
            Error information if gamma could not be set.
        """
        result = self.con.cmdTagList("setGamma", colormapId=self.getId(),
                                     gammaValue=gamma)
        return result

    def setDataTransform(self, transformStr):
        """
        Set the name of the data transform.

        Parameters
        ----------
        transformStr: string
            A unique identifier for a data transform.

        Returns
        -------
        list
            Error information if the data transform could not be set.
            [NOTE: should we provide a list of valid transforms here?]
        """
        result = self.con.cmdTagList("setDataTransform",
                                     colormapId=self.getId(),
                                     transform=transformStr)
        return result
