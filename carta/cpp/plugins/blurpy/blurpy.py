#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function

print("hello from blurpy.py")

from scipy import ndimage
import scipy
import numpy as np
from random import randrange

def no_preRenderHook(w, h, data):
    print("preRenderHook from blurpy.py", w, h, len(data))

    myShape = data
    print("py: stride is:", myShape.strides)
    sigma = 2
    red = myShape[:,:,0]
#    red[...] = ndimage.gaussian_filter( red, sigma=sigma)
    ndimage.gaussian_filter( red, sigma=sigma, output=red)
    green = myShape[:,:,1]
#    green[...] = ndimage.gaussian_filter( green, sigma=sigma)
    ndimage.gaussian_filter( green, sigma=sigma, output=green)
    blue = myShape[:,:,2]
#    blue[...] = ndimage.gaussian_filter( blue, sigma=sigma)
    ndimage.gaussian_filter( blue, sigma=sigma, output=blue)

# grayscale blur:
#    myShape[...] = ndimage.gaussian_filter( myShape, sigma=5)

print("end of blurpy.py")
