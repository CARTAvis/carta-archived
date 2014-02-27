CONFIG += build_all

# ensure one "debug_and_release" in CONFIG, for clarity...
debug_and_release {
    CONFIG -= debug_and_release
    CONFIG += debug_and_release
}
# ensure one "debug" or "release" in CONFIG so they can be used as
#   conditionals instead of writing "CONFIG(debug, debug|release)"...
CONFIG(debug, debug|release) {
    CONFIG -= debug release
    CONFIG += debug
}
CONFIG(release, debug|release) {
    CONFIG -= debug release
    CONFIG += release
}

# use c++11 with gcc 4.8.1
#QMAKE_CXX = g++-4.8
#QMAKE_CXXFLAGS = -std=c++11
#QMAKE_LFLAGS = -std=c++11
#QMAKE_CXXFLAGS += -fsanitize=address  -fno-omit-frame-pointer
#QMAKE_LFLAGS += -fsanitize=address  -fno-omit-frame-pointer
#QMAKE_CXXFLAGS += -fsanitize=thread
#QMAKE_LFLAGS += -fsanitize=thread

# use c++11 with gcc 4.6.3
QMAKE_CXXFLAGS += -std=c++0x
QMAKE_LFLAGS += -std=c++0x

#QMAKE_CXXFLAGS -= -O2
QMAKE_CXXFLAGS_RELEASE -= -Os
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O0
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE -= -O3
QMAKE_CXXFLAGS_RELEASE += -O3

QMAKE_CXXFLAGS_DEBUG -= -Os
QMAKE_CXXFLAGS_DEBUG -= -O
QMAKE_CXXFLAGS_DEBUG -= -O0
QMAKE_CXXFLAGS_DEBUG -= -O1
QMAKE_CXXFLAGS_DEBUG -= -O2
QMAKE_CXXFLAGS_DEBUG -= -O3
QMAKE_CXXFLAGS_DEBUG += -O0

# add openmp support
QMAKE_CXXFLAGS += -fopenmp -D_GLIBCXX_PARALLEL
QMAKE_LFLAGS += -fopenmp

# casacore 1.5.0 - old
CASACOREDIR=$$(HOME)/Software/casacore-1.5.0-static
WCSLIBDIR=$$(HOME)/Software/wcslib-4.15
CFITSIODIR=$$(HOME)/Software/cfitsio-3.310

PUREWEBSDKDIR=$$(HOME)/Work/Software/PureWeb/4.0.0-ga/SDK


#ASTLIBDIR = $$(HOME)/Software/ast-7.0.6
# version 7.3.2 currently has a bug with carlin=1 being fixed...
#ASTLIBDIR = $$(HOME)/Software/ast-7.3.2
ASTLIBDIR = $$(HOME)/Software/ast-7.3.3

casacoreLIBS += -L$${CASACOREDIR}/lib
casacoreLIBS += -lcasa_images -lcasa_components -lcasa_coordinates -lcasa_fits -lcasa_measures
casacoreLIBS += -lcasa_lattices -lcasa_tables -lcasa_scimath -lcasa_scimath_f -lcasa_mirlib
casacoreLIBS += -lcasa_casa -llapack -lblas -lgfortran -ldl
casacoreLIBS += -L$${WCSLIBDIR}/lib -lwcs
casacoreLIBS += -L$${CFITSIODIR}/lib -lcfitsio

astlibLIBS += $${ASTLIBDIR}/lib/libast.a
astlibLIBS += $${ASTLIBDIR}/lib/libast_pal.a
astlibLIBS += $${ASTLIBDIR}/lib/libast_grf3d.a
astlibLIBS += $${ASTLIBDIR}/lib/libast_pass2.a
astlibLIBS += $${ASTLIBDIR}/lib/libast_err.a

gslLIBS += /usr/lib/libgsl.a -lblas


