all :  doxy

DOXYGEN   ?= `which doxygen`
RM        = rm -f
CP        = cp
MKDIR_P   = mkdir -p
HAVE_DOT  = /usr/bin/dot

.PHONY: doxy

doxy :
	$(DOXYGEN) Doxyfile
