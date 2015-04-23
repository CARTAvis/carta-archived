#!/usr/bin/env python
# -*- coding: utf-8 -*-

from cartaview import CartaView

class Histogram(CartaView):
    """Represents a histogram view"""

    def applyClips(self, clipMin, clipMax, mode):
        result = self.con.cmdTagList("applyClips",
                                     histogramView=self.getId(),
                                     clipMinValue=clipMin,
                                     clipMaxValue=clipMax,
                                     modeStr=mode)
        return result

    def applyClipsByPercent(self, clipMin, clipMax):
        result = self.applyClips(clipMin, clipMax, "percent")
        return result

    def applyClipsByIntensity(self, clipMin, clipMax):
        result = self.applyClips(clipMin, clipMax, "intensity")
        return result
