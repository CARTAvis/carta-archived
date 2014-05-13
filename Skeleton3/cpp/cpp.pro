TEMPLATE = subdirs

SUBDIRS = \
    common \
    desktop \
    server \
    plugins

# build these in order
CONFIG += ordered

# explicit dependencies
desktop.depends = common
desktop2.depends = common
server.depends = common

# to make the file visible in qt creator
OTHER_FILES += common.pri
