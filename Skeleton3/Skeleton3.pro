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

#isEmpty( CARTA_BUILD_TYPE) {
#    warning( "You did not specify CARTA_BUILD_TYPE to qmake...")
#    warning( "supported builds are: release dev bughunter")
#    warning( "please rerun qmake with an argument 'CARTA_BUILD_TYPE=dev'")
#}

#dbg( "root            PWD=$$PWD")
#dbg( "root         IN_PWD=$$IN_PWD")
#dbg( "root _PRO_FILE_PWD_=$$_PRO_FILE_PWD_")
#dbg( "root        OUT_PWD=$$OUT_PWD")

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



