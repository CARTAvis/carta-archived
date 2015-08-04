TEMPLATE = subdirs

# to make the file visible in qt creator
# OTHER_FILES += common.pri

SUBDIRS = \
    CartaLib \
    core \
    desktop \
    plugins \
    Tests

isEmpty(NOSERVER) {
	SUBDIRS +=server
}

# explicit dependencies, to make sure parallel make works (i.e. make -j4...)
common.depends = CartaLib
desktop.depends = core
server.depends = core
plugins.depends = core
isEmpty(NOSERVER) {
        Tests.depends = core desktop server plugins
}
else{
        Tests.depends = core desktop plugins
}

# ... or ...
# build directories in order, or make sure to update dependencies manually, or make -j4 won't work
# ordered is the slowest option but most reliable and requires no maintenance :)
#CONFIG += ordered
