#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function

print("hello from noisepy.py")


import scipy
import numpy as np
from random import randrange

def preRenderHook(w, h, data):
    print("preRenderHook from noisepy.py", w, h, len(data))

    for i, x in enumerate(data):
        if (i // (w*3)) % 2 < 1:
           data[i] = 0

    return data
