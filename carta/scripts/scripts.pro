TEMPLATE = aux

OTHER_FILES += \
    fabfile.py \
    fabfile2.py

MYFILES = $$files($${PWD}/*.py)
copy_files.name = copy project files
copy_files.input = MYFILES
copy_files.output = ${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
copy_files.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy_files.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += copy_files

DISTFILES += \
    deploy-viz2-release.py
