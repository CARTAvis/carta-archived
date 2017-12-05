# invoke like this:
# python setup.py build_ext --inplace

from distutils.core import setup
from Cython.Build import cythonize

setup(
    name = 'c++/python bridge',
    ext_modules = cythonize(
        "pluginBridge.pyx",                # source
        extra_compile_args=["-O3"]
#        language="c++",
        ),


)
