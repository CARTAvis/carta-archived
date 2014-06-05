TEMPLATE = subdirs

# to make the file visible in qt creator
# OTHER_FILES += common.pri

SUBDIRS = \
    common \
    desktop \
    server \
    plugins \
    Tests

# explicit dependencies, much better for make -j4...
desktop.depends = common
server.depends = common
plugins.depends = common
Tests.depends = common desktop server plugins

# ... or ...
# build directories in order, or make sure to update dependencies manually, or make -j4 won't work
# ordered is the slowest option but most reliable and requires no maintenance :)
#CONFIG += ordered
