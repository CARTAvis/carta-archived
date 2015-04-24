#!/usr/bin/env python
# -*- coding: utf-8 -*-

from cartaview import CartaView

class Colormap(CartaView):
    """Represents a colormap view"""

    def setColormap(self, colormap):
        """Set the specified colormap"""
        result = self.con.cmdTagList("setColormap", colormapId=self.getId(),
                                     colormapName=colormap)
        return result

    def reverseColormap(self, reverseStr='toggle'):
        result = self.con.cmdTagList("reverseColormap",
                                     colormapId=self.getId(),
                                     reverseString=str(reverseStr))
        return result

    def setCacheColormap(self, cacheStr='toggle'):
        result = self.con.cmdTagList("setCacheColormap",
                                     colormapId=self.getId(),
                                     cacheString=str(cacheStr))
        return result

    def setCacheSize(self, cacheSize):
        result = self.con.cmdTagList("setCacheSize", colormapId=self.getId(),
                                     size=str(cacheSize))
        return result

    def setInterpolatedColormap(self, interpolatedStr='toggle'):
        result = self.con.cmdTagList("setInterpolatedColormap",
                                     colormapId=self.getId(),
                                     interpolatedString=str(interpolatedStr))
        return result

    def invertColormap(self, invertStr='toggle'):
        result = self.con.cmdTagList("invertColormap",
                                     colormapId=self.getId(),
                                     invertString=str(invertStr))
        return result

    def setColorMix(self, redPercent, greenPercent, bluePercent):
        result = self.con.cmdTagList("setColorMix", colormapId=self.getId(),
                                     red=str(redPercent),
                                     green=str(greenPercent),
                                     blue=str(bluePercent))
        return result

    def setGamma(self, gamma):
        result = self.con.cmdTagList("setGamma", colormapId=self.getId(),
                                     gammaValue=gamma)
        return result

    def setDataTransform(self, transformStr):
        result = self.con.cmdTagList("setDataTransform",
                                     colormapId=self.getId(),
                                     transform=transformStr)
        return result
