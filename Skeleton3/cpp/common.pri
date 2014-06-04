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

#QMAKE_CXXFLAGS += -fsanitize=thread
#QMAKE_LFLAGS += -fsanitize=thread

# add a little speedup with ccache
linux-*:exists(/usr/bin/ccache):QMAKE_CXX=ccache $${COMPILER}
linux-*:exists(/usr/bin/ccache):QMAKE_LINK=ccache $${COMPILER}

message( "pri            PWD=$$PWD")
message( "pri         IN_PWD=$$IN_PWD")
message( "pri _PRO_FILE_PWD_=$$_PRO_FILE_PWD_")
message( "pri        OUT_PWD=$$OUT_PWD")

# the rest of the project files may use this
PROJECT_ROOT = $$IN_PWD

# add include/depend path to start from root
INCLUDEPATH += $$PROJECT_ROOT
DEPENDPATH += $$PROJECT_ROOT

