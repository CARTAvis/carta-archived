# COMPILER = g++
CONFIG += gcc481

gcc481 {
    COMPILER = g++-4.8
} else {
    COMPILER = g++
}

# use c++11 with gcc 4.8.1
QMAKE_CXX = $${COMPILER}
QMAKE_LINK = $${COMPILER}
#QMAKE_CXXFLAGS += -std=c++11
#QMAKE_LFLAGS += -std=c++11
CONFIG += c++11

#QMAKE_CXXFLAGS += -fsanitize=address  -fno-omit-frame-pointer
#QMAKE_LFLAGS += -fsanitize=address  -fno-omit-frame-pointer
#QMAKE_CXXFLAGS += -fsanitize=thread
#QMAKE_LFLAGS += -fsanitize=thread

# use c++11 with gcc 4.6.3
#QMAKE_CXXFLAGS += -std=c++0x
#QMAKE_LFLAGS += -std=c++0x

#QMAKE_CXXFLAGS += -Wall

# add a little speedup with ccache
linux-*:exists(/usr/bin/ccache):QMAKE_CXX=ccache $${COMPILER}
linux-*:exists(/usr/bin/ccache):QMAKE_LINK=ccache $${COMPILER}

HEADERS += \
    ../Nullable.h

