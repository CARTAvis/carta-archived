#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function

print("hello from noisepy.py")


import scipy
import numpy as np
from random import randrange

def preRenderHook(w, h, data):
    print("preRenderHook from noisepy.py", w, h, len(data))
#    myShape = data.reshape(h,w,3)
    myShape = data
    for y in range(0,h):
        if y % 2 < 1:
            myShape[y,...] = 0
#    for i, x in enumerate(data):
#        if (i // (w*3)) % 10 < 5:
#           data[i] = 0
