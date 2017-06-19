#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function
import os.path

basename = os.path.basename(__file__)
oldprint = print

def print(*args, **kwargs):
    global basename, oldprint
    return oldprint( basename, ':', *args, **kwargs)

# old code:
#
#import scipy
#import numpy as np
#from random import randrange
#
#def no_preRenderHook(w, h, data):
#    print("preRenderHook", w, h, len(data))
#    myShape = data
#    for y in range(0,h):
#        if y % 20 < 2:
#            myShape[y,...] = 0


## colormap test

import matplotlib.cm as cm

class CMap(object):
    def __init__(self, name):
        self.m_cmap = cm.get_cmap(name)
        self.m_name = name
    def convert(self, x):
        return self.m_cmap(float(x))
    def name(self):
        return self.m_name

def colormapScalarHook():
    print("colormapScalarHook");
    maps=[m for m in cm.datad if not m.endswith("_r")]
    print("available colormaps from matplotlib: ", maps)

    # filter out unwanted colormaps borrowed from matplotlib
    maps_whiteList = ['Spectral','coolwarm','Set1','gnuplot2',
                      'jet','Reds','gist_stern','Blues',
                      'terrain','gnuplot','hot','Greens',
                      'nipy_spectral','viridis','inferno','RdBu']
    print("including the following colormaps only: ", maps_whiteList)
    maps_filtered = []
    for x in maps:
        if x in maps_whiteList:
            maps_filtered.append(x)

    result = []
    for i, m in enumerate(maps_filtered):
        result.append(CMap(m))
    return result

