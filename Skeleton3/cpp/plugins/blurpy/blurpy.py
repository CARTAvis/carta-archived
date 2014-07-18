#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function

print("hello from blurpy.py")


import scipy
import numpy as np
from random import randrange

def preRenderHook(w, h, data):
    print("preRenderHook from blurpy.py", w, h, len(data))

    for i, x in enumerate(data):
        if randrange(0,10) < 5:
           data[i] = randrange(-127,127)

    return data
