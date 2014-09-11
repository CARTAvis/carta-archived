# invoke like this:
# python setup.py build_ext --inplace

from distutils.core import setup
from Cython.Build import cythonize

setup(
    name = 'Hello world app',
    ext_modules = cythonize(
        "pluginBridge.pyx",                # source
#        language="c++",
        include_dirs = [ "/scratch/cy" ],
        ),
)
