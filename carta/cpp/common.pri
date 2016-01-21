defineTest(dbg) {
    log( "dbg: $$1$$escape_expand(\\n)")
#    message( $$1)
}

! include(common_config.pri) {
  error( "Could not find the common_config.pri file!" )
}

equals( CARTA_BUILD_TYPE, release) {
    CARTA_CONFIG =
}
else:equals( CARTA_BUILD_TYPE, bughunter) {
    CARTA_CONFIG = noOpt gdb dbgout runtimeChecks #addrSanit
}
else {
    # assuming dev build...
    !equals( CARTA_BUILD_TYPE, dev) {
        !isEmpty(CARTA_BUILD_TYPE) {
            warning( "Incorrect CARTA_BUILD_TYPE: $$CARTA_BUILD_TYPE")
            error( "supported builds are: release dev bughunter")
        }
        warning( "You did not specify CARTA_BUILD_TYPE to qmake...")
        warning( "supported builds are: release dev bughunter")
        warning( "assuming you wanted dev build, i.e. 'qmake CARTA_BUILD_TYPE=dev'")
        $$prompt( "enter to continue, or ctrl-c to abort")
    }
    CARTA_CONFIG = dbgout runtimeChecks
}

message("CARTA_CONFIG is now: $$CARTA_CONFIG")

#remove known optimizations and debugger flags
QMAKE_CXXFLAGS -= -O -O0 -O1 -O2 -O3 -g
QMAKE_CXXFLAGS_DEBUG -= -O -O0 -O1 -O2 -O3 -g
QMAKE_CXXFLAGS_RELEASE -= -O -O0 -O1 -O2 -O3 -g
QMAKE_CFLAGS -= -O -O0 -O1 -O2 -O3 -g
QMAKE_CFLAGS_DEBUG -= -O -O0 -O1 -O2 -O3 -g
QMAKE_CFLAGS_RELEASE -= -O -O0 -O1 -O2 -O3 -g

contains( CARTA_CONFIG, gdb) {
    QMAKE_CXXFLAGS += -g
    QMAKE_CFLAGS += -g
    QMAKE_LFLAGS += -g
    message( "+ debugger support")
} else {
    message( "- NO debugger support")
}
contains( CARTA_CONFIG, dbgout) {
    message( "+ extra debug output")
} else {
    QMAKE_CXXFLAGS += -DQT_NO_DEBUG_OUTPUT -DQT_NO_WARNING_OUTPUT
    QMAKE_CFLAGS += -DQT_NO_DEBUG_OUTPUT -DQT_NO_WARNING_OUTPUT
    CONFIG -= debug
    message( "- NO extra debug output")
}
contains( CARTA_CONFIG, runtimeChecks) {
    QMAKE_CXXFLAGS += -DCARTA_RUNTIME_CHECKS=1
    QMAKE_CFLAGS += -DCARTA_RUNTIME_CHECKS=1
    CONFIG += debug
    message( "+ extra runtime checks")
} else {
    QMAKE_CXXFLAGS += -DCARTA_RUNTIME_CHECKS=0
    QMAKE_CFLAGS += -DCARTA_RUNTIME_CHECKS=0
    CONFIG -= debug
    message( "- NO extra runtime checks")
}
contains( CARTA_CONFIG, noOpt) {
    message( "- NO full optimization")
    QMAKE_CXXFLAGS += -O0
    QMAKE_CFLAGS += -O0
    QMAKE_LFLAGS += -O0
} else {
    message( "+ full optimization")
    QMAKE_CXXFLAGS += -O2
    QMAKE_CFLAGS += -O2
}
contains( CARTA_CONFIG, addrSanit) {
    QMAKE_CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
    QMAKE_CFLAGS += -fsanitize=address -fno-omit-frame-pointer
    QMAKE_LFLAGS += -fsanitize=address
    message( "+ address sanitization")
} else {
    message( "- no address sanitization")
}

# use gcc 4.8.1
CONFIG += gcc481

gcc481 {
    COMPILER = g++-4.8
} else {
    COMPILER = g++
}

QMAKE_CXX = $${COMPILER}
QMAKE_LINK = $${COMPILER}

# use c++11
CONFIG += c++11

# -Wall
CONFIG += warn_on

# comment out for some extra memory debugging (either this or valgrind, not both)
#QMAKE_CXXFLAGS += -fsanitize=address  -fno-omit-frame-pointer
#QMAKE_LFLAGS += -fsanitize=address  -fno-omit-frame-pointer


# add a little speedup with ccache
linux-*:exists(/usr/bin/ccache):QMAKE_CXX=ccache $${COMPILER}
linux-*:exists(/usr/bin/ccache):QMAKE_LINK=ccache $${COMPILER}

#message( "pri            PWD=$$PWD")
#message( "pri         IN_PWD=$$IN_PWD")
#message( "pri _PRO_FILE_PWD_=$$_PRO_FILE_PWD_")
#message( "pri        OUT_PWD=$$OUT_PWD")

# the rest of the project files may use this
PROJECT_ROOT = $$IN_PWD

# add include/depend path to start from root
INCLUDEPATH += $$PROJECT_ROOT
DEPENDPATH += $$PROJECT_ROOT

