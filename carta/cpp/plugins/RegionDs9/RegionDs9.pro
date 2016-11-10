! include(../../common.pri) {
  error( "Could not find the common.pri file!" )
}

QT       += core gui
TARGET = plugin
TEMPLATE = lib
CONFIG += plugin

FLEXSOURCES = ds9lex.L 
   
BISONSOURCES = ds9parse.y

SOURCES += \
    RegionDs9.cpp \
    ParserDs9.cpp \
    ContextDs9.cpp

HEADERS += \
    RegionDs9.h \
    ParserDs9.h \
    ContextDs9.h \
    ds9FlexLexer.h \
    ds9lex.h
    

casacoreLIBS += -L$${CASACOREDIR}/lib
casacoreLIBS += -lcasa_lattices -lcasa_tables -lcasa_scimath -lcasa_scimath_f -lcasa_mirlib
casacoreLIBS += -lcasa_casa -llapack -lblas -ldl
casacoreLIBS += -lcasa_images -lcasa_coordinates -lcasa_fits -lcasa_measures

LIBS += $${casacoreLIBS}
LIBS += -L$${WCSLIBDIR}/lib -lwcs
LIBS += -L$${CFITSIODIR}/lib -lcfitsio
LIBS += -L$$OUT_PWD/../../core/ -lcore
LIBS += -L$$OUT_PWD/../../CartaLib/ -lCartaLib
unix:macx{
    LIBS += -L$${FLEXANDBISONDIR}/lib -lfl -ly
    INCLUDEPATH += $${FLEXANDBISONDIR}/include
} else {
    LIBS += -lfl -ly
}

INCLUDEPATH += $${CASACOREDIR}/include
INCLUDEPATH += $${CASACOREDIR}/include/casacore
INCLUDEPATH += $${WCSLIBDIR}/include
INCLUDEPATH += $${CFITSIODIR}/include
#INCLUDEPATH += $$PWD/../../core
DEPENDPATH += $$PWD/../../core

OTHER_FILES += \
    plugin.json \
    $$FLEXSOURCES \
    $$BISONSOURCES

# copy json to build directory
MYFILES = plugin.json
MYFILES += $$FLEXSOURCES
MYFILES += $$BISONSOURCES

! include($$top_srcdir/cpp/copy_files.pri) {
  error( "Could not include $$top_srcdir/cpp/copy_files.pri file!" )
}

flexsource.input = FLEXSOURCES
flexsource.output = ${QMAKE_FILE_BASE}.cpp
flexsource.commands = flex -o ${QMAKE_FILE_BASE}.cpp ${QMAKE_FILE_IN}
flexsource.variable_out = SOURCES
flexsource.name = Flex Sources ${QMAKE_FILE_IN}
flexsource.CONFIG = += target_predeps
QMAKE_EXTRA_COMPILERS += flexsource

bisonsource.input = BISONSOURCES
bisonsource.output = ${QMAKE_FILE_BASE}.cpp
bisonsource.commands = bison -d --defines=${QMAKE_FILE_BASE}.hpp -o ${QMAKE_FILE_BASE}.cpp ${QMAKE_FILE_IN}
bisonsource.variable_out = SOURCES
bisonsource.name = Bison Sources ${QMAKE_FILE_IN}
bisonsource.CONFIG += target_predeps
QMAKE_EXTRA_COMPILERS += bisonsource

bisonheader.input = BISONSOURCES
bisonheader.output = ${QMAKE_FILE_BASE}.hpp
bisonheader.commands = @true
bisonheader.variable_out = HEADERS
bisonheader.name = Bison Headers ${QMAKE_FILE_IN}
bisonheader.CONFIG += target_predeps no_link
QMAKE_EXTRA_COMPILERS += bisonheader




unix:macx {
    PRE_TARGETDEPS += $$OUT_PWD/../../core/libcore.dylib
    QMAKE_LFLAGS += -undefined dynamic_lookup
}
else{
    PRE_TARGETDEPS += $$OUT_PWD/../../core/libcore.so
}


