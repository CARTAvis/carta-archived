! include(../../common.pri) {
  error( "Could not find the common.pri file!" )
}

INCLUDEPATH += /usr/include/python2.7
unix:macx{
 	INCLUDEPATH += /System/Library/Frameworks/Python.framework/Versions/2.7/Extras/lib/python/numpy/core/include
}

LIBS += -lpython2.7
INCLUDEPATH += $$PROJECT_ROOT
DEPENDPATH += $$PROJECT_ROOT

QT       += core gui

TARGET = plugin
TEMPLATE = lib
CONFIG += plugin

SOURCES += \
    Python273Plugin.cpp \
    PyCppPlugin.cpp

HEADERS += \
     Python273Plugin.h \
    PyCppPlugin.h \
    pragmaHack.h

OTHER_FILES += \
    plugin.json \
    pluginBridge.pyx \
    setup.py

# experiment with
#relative_path(filePath[, base])
#Returns the path to filePath relative to base. If base is not specified,
#it is the current project directory. This function is a wrapper around QDir::relativeFilePath.


# copy json to build directory
MYFILES = plugin.json
MYFILES += $$files($${PWD}/*.py)
MYFILES += $$files($${PWD}/*.pyx)
copy_files.name = copy large files
copy_files.input = MYFILES
copy_files.output = ${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_files.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_files.CONFIG += no_link target_predeps
copy_files.clean = $${copy_files.output}
QMAKE_EXTRA_COMPILERS += copy_files


# ---------------------------------------------------------
# experiment with cython
# ---------------------------------------------------------
CYTHONSOURCES = pluginBridge.pyx
cythoncpp.input = CYTHONSOURCES
cythoncpp.output = ${QMAKE_FILE_BASE}.cpp
#cythoncpp.commands = python $$_PRO_FILE_PWD_/setup.py build_ext --inplace
#cythoncpp.commands = python setup.py build_ext --inplace
cythoncpp.commands = python -m cython --cplus -f pluginBridge.pyx
cythoncpp.variable_out = SOURCES
cythoncpp.name = cython Sources ${QMAKE_FILE_IN}
cythoncpp.CONFIG += target_predeps

QMAKE_EXTRA_COMPILERS += cythoncpp

cythonheader.input = CYTHONSOURCES
cythonheader.output = ${QMAKE_FILE_BASE}.h
cythonheader.commands = @true
cythonheader.variable_out = HEADERS
cythonheader.name = cython Headers ${QMAKE_FILE_IN}
cythonheader.CONFIG += target_predeps no_link
cythonheader.depends += ${QMAKE_FILE_BASE}.cpp

QMAKE_EXTRA_COMPILERS += cythonheader
