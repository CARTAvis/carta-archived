# distutils: language = c++

from __future__ import division, absolute_import, print_function
from libcpp.string cimport string
from libcpp cimport bool
from libcpp.vector cimport vector
from libc.stdint cimport int8_t
import importlib
import imp

#the following will tell cython to generate #include "pragmaHacks.h"
cdef extern from "pragmaHack.h":
    int pragma_hack_i_dont_exist

cdef public void foobar():
    return

# module-globals
moddId = 1
mods = {}

# list of mods

class Modd(object):
    def __init__(self, modName, fname):
        global moddId
        try:
            self.id = -1
            self.loadedMod = imp.load_source( modName, fname)
            self.modName = modName
            self.fname = fname
            self.id = moddId
            moddId = moddId + 1
            print("moddid = ", moddId)
        except:
            print("Failed to load mod")

        return

    def getId(self):
        return self.id

    def hasPreRenderHook(self):
        return hasattr(self.loadedMod, 'preRenderHook')


cdef public int  pb_loadModule( string fname, string modName):
    # mod = importlib.import_module( fname)
    # mod = imp.load_source( modName, fname)
    mod = Modd( modName, fname)
    print( "mod.id=", mod.getId())
    if( mod.getId() >= 0):
        print( "mod.mod=", mod.loadedMod)
        mods[ mod.getId()] = mod
    return mod.getId()

# check if the plugin has ColormapScalarHook
cdef public bool pb_hasColormapScalarHook( int id):
    if not id in mods:
        return False
    return hasattr(mods[id].loadedMod, 'colormapScalarHook')



# check to see if the python plugin has pre-render hook
cdef public bool pb_hasPreRenderHook( int id):
    if not id in mods:
        return False
    return mods[id].hasPreRenderHook()

# from cython cimport view
cimport numpy as np
import numpy as np

np.import_array()

# this calls the pre-render hook defined in the python plugin
cdef public bool pb_callPreRenderHook( int id, int w, int h, int stride, unsigned char * data):
    print("pb_callPreRenderHook id=", id, w, h)
    if not id in mods:
        print("!!! could not find mod", id)
        return False
    print("weird", hasattr( mods[id].loadedMod, 'preRenderHook'), mods[id], mods[id].loadedMod)
    cdef int len = w * h * 3
    # cdef unsigned char[::1] mv = <unsigned char[:len]> data
    cdef unsigned char[:,:,:] mv = <unsigned char[:h,:w,:3]> data
    mv.strides[0] = stride
    nv = np.asarray( mv)
    # nv.shape = (h,w,3)
    mods[id].loadedMod.preRenderHook(w, h, nv)

    return True

# just a debug statement
print( "Last line of pluginBridge.pyx...." )
