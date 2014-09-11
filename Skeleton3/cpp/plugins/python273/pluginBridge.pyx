# distutils: language = c++

from __future__ import division, absolute_import, print_function
from libcpp.string cimport string
from libcpp cimport bool
from libcpp.vector cimport vector
from libc.stdint cimport int8_t
import importlib
import imp


cdef public void foobar():
    return

# cdef int moddId
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

cdef public bool pb_hasPreRenderHook( int id):
    if not id in mods:
        return False
    return mods[id].hasPreRenderHook()

# from cython cimport view
# cimport numpy as np
#import numpy as np


# cdef public bool pb_callPreRenderHook( int id, int w, int h, const char * data):
#     print("pb_callPreRenderHook id=", id, w, h)
#     # numpy_array = np.asarray(<np.int8_t[:w, :h]> data)
#     cdef view.array my_array = view.array(shape=(w, h), itemsize=sizeof(char), format="i", mode="c", allocate_buffer=False)
#     my_array.data = <char *> data
#     print( "first bytes:", my_array[0,0])
#     return True

cdef public bool pb_callPreRenderHook( int id, int w, int h, vector[char] & data):
    print("pb_callPreRenderHook id=", id, w, h, data.size())
    data[0] = 123
    if not id in mods:
        print("!!! could not find mod", id)
        return False
    print("weird", hasattr( mods[id].loadedMod, 'preRenderHook'), mods[id], mods[id].loadedMod)
    newData = mods[id].loadedMod.preRenderHook(w, h, data)
    for i, x in enumerate(newData):
        data[i] = newData[i]

    return True

# # pyData = data
    # # my_array = np.asarray(<np.int8_t[:w, :h]> pyData)
    # my_array = np.array( data)
    #
    # print( "first bytes:", my_array[0,0])
    # return True


print( "Last line of pluginBridge.pyx...." )
