! include(../../common.pri) {
  error( "Could not find the common.pri file!" )
}

TEMPLATE = aux

OTHER_FILES += \
    plugin.json \
    ColormapsPy.py

# list files to copy to compile output in MYFILES

# Masks are available with $$files functions but
# if your set of files changes (files added or removed)
# your have to re-run qmake after that explicitly, not just make
#MYFILES = $$files($${PWD}/files/*.*)
MYFILES  = plugin.json
MYFILES += $$files($${PWD}/*.py)
! include($$top_srcdir/cpp/copy_files.pri) {
  error( "Could not include $$top_srcdir/cpp/copy_files.pri file!" )
}

LIBSTOCOPY += $$files($${PWD}/libs/*.so)
message( "libstocopy = $${LIBSTOCOPY}" )
copy_libs.name = copy shared libraries
copy_libs.input = LIBSTOCOPY
copy_libs.output = $${OUT_PWD}/libs/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_libs.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_libs.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_libs


