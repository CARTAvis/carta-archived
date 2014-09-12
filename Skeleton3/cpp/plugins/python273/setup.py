# invoke like this:
# python setup.py build_ext --inplace

#from distutils.core import setup
#from Cython.Build import cythonize

#setup(
#    name = 'Python bridge',
#    ext_modules = cythonize(
#        "pluginBridge.pyx"                # source
#        language="c++"
##        include_dirs = [ "/scratch/cy" ],
#        )
#)

from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

extensions = [
    Extension("pluginBridge", ["pluginBridge.pyx"],
        language="c++"
#        extra_compile_args=['']
        )
]

setup(
name = 'Python bridge',
    ext_modules = cythonize(extensions),
)
