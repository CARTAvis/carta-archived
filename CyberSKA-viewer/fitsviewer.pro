TEMPLATE = subdirs

#SUBDIRS += \
#    FitsViewerLib \
#    CoordinatesLib \
#    DesktopFitsViewer \
#    Testing \
#    Docs \
#    Fits2Image \
#    RaiViewerModel \
#    RaiViewerTester

SUBDIRS += FitsViewerLib
SUBDIRS += CoordinatesLib
SUBDIRS += DesktopFitsViewer
SUBDIRS += Testing
SUBDIRS += Docs
SUBDIRS += Fits2Image
#SUBDIRS += RaiViewerModel
#SUBDIRS += RaiViewerTester

CONFIG += ordered

#doc_builder.name = dox
#doc_builder.input = Doxyfile
#doc_builder.output =
#doc_builder.commands = doxygen Doxyfile
#doc_builder.clean =
#QMAKE_EXTRA_COMPILERS += doc_builder

