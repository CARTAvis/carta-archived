import sys
import os
import inspect
import string
import time
from taskinit import *
import viewertool
import carta.cartavis as cartavis
import json
###
### if numpy is not available, make float64 and ndarray redundant checks...
###
try:
    from numpy import float64 as float64
    from numpy import ndarray as ndarray
except:
    float64 = float
    ndarray = list

class __cartaview_class(object):
    "cartaview() task with local state for created viewer tool"

    def __init__( self ):
        self.local_vi = None
        self.local_ving = None
        self.__dirstack = [ ]
        self.__colorwedge_queue = [ ]

    def __call__(self, raster, configFile):
        """
        The cartaview task will display an image in the CARTA viewer.

        examples of usage:

        cartaview
        cartaview "myimage.fits"
    
        Executing cartaview() will bring up a display panel
        window, which can be resized.
        """
        a=inspect.stack()
        stacklevel=0
        for k in range(len(a)):
            if (a[k][1] == "<string>" or
                (string.find(a[k][1], 'ipython console') > 0 or
                string.find(a[k][1],"casapy.py") > 0)):
                stacklevel=k

        myf=sys._getframe(stacklevel).f_globals

        casalog.origin('cartaview')

        configJson = open(configFile).read()
        data = json.loads(configJson)
        executable = data["executable"]
        htmlFile = data["htmlFile"]
        port = data["port"]

        v = cartavis.Cartavis(executable, configFile, int(port), htmlFile,
                              raster)
        return v

cartaview = __cartaview_class( )
