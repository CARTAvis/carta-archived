defineTest(dbg) {
#    log( "dbg: $$1$$escape_expand(\\n)")
    message( $$1)
}
dbg( "===-------- Original compiler settings: --------====")
dbg( "QMAKE_CXX $$QMAKE_CXX")
dbg( "QMAKE_CXXFLAGS_DEBUG $$QMAKE_CXXFLAGS_DEBUG")
dbg( "QMAKE_CXXFLAGS_RELEASE $$QMAKE_CXXFLAGS_RELEASE")
dbg( "QMAKE_CXXFLAGS $$QMAKE_CXXFLAGS")
dbg( "QMAKE_CFLAGS $$QMAKE_CFLAGS")
dbg( "QMAKE_CFLAGS_DEBUG $$QMAKE_CFLAGS_DEBUG")
dbg( "QMAKE_CFLAGS_RELEASE $$QMAKE_CFLAGS_RELEASE")
dbg( "===---------------------------------------------====")

TEMPLATE = subdirs
SUBDIRS += cpp
SUBDIRS += scriptedClient
SUBDIRS += scripts

OTHER_FILES += readme.txt uncrustify.cfg


# make sure user did not specify debug version...
# it's probably harmless, but that's probably not what the user wanted
CONFIG(debug,debug|release) {
    error( "Please don't specify debug version like this!")
}

CONFIG(release,debug|release) {
    message( "All good, you specified release version!")
}

!include("cpp/common.pri") {
    error("Cannot include cpp/common.pri file")
}

dbg( "===----------Adjusted compiler settings---------====")
dbg( "QMAKE_CXX $$QMAKE_CXX")
dbg( "QMAKE_CXXFLAGS_DEBUG $$QMAKE_CXXFLAGS_DEBUG")
dbg( "QMAKE_CXXFLAGS_RELEASE $$QMAKE_CXXFLAGS_RELEASE")
dbg( "QMAKE_CXXFLAGS $$QMAKE_CXXFLAGS")
dbg( "QMAKE_CFLAGS $$QMAKE_CFLAGS")
dbg( "QMAKE_CFLAGS_DEBUG $$QMAKE_CFLAGS_DEBUG")
dbg( "QMAKE_CFLAGS_RELEASE $$QMAKE_CFLAGS_RELEASE")
dbg( "===---------------------------------------------====")

#for(var, $$list($$enumerate_vars())) {
#    x=$$eval($$var)
#    dbg( "$${var} = $${x}")
#}

#mytarget.target = buildfile.txt
#mytarget.commands = touch $$mytarget.target
#mytarget.depends = mytarget2 mytarget3

#mytarget2.commands = @echo Building $$mytarget.target
#mytarget3.commands = @echo top src/build: $$top_srcdir $$top_builddir

#QMAKE_EXTRA_TARGETS += mytarget mytarget2 mytarget3


