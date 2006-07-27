MAKEDYLIB=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


owstatlib_LIBRARIES = libowmofc.a 
thelibdir = $(libdir)
owstatlibdir = $(thelibdir)

OW_SHARED_LIBS = libowmofc.$(LIB_EXT)
SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem
# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim

libowmofc_a_SOURCES = \
OW_MOFVisitor.cpp \
OW_MOFParser.yy \
OW_MOFLexer.ll \
OW_MOFCIMOMVisitor.cpp \
OW_MOFCompiler.cpp \
OW_MOFParserErrorHandlerIFC.cpp \
OW_MOFParseFatalErrorException.cpp

pkginclude_HEADERS = \
OW_MOFGrammar.hpp \
OW_MOFVisitor.hpp \
OW_MOFParserErrorHandlerIFC.hpp \
OW_MOFCIMOMVisitor.hpp \
OW_MOFCompiler.hpp \
OW_MOFParserDecls.hpp \
OW_MOFLineInfo.hpp


EXTRA_DIST = \
$(top_builddir)/src/mof/OW_MOFParser.cc \
$(top_builddir)/src/mof/OW_MOFLexer.cc \
$(top_builddir)/src/mof/OW_MOFParser.h

# This is necessary in order for dependency tracking to work
BUILT_SOURCES = OW_MOFParser.h

#debug versions
#AM_YFLAGS = -d -t -v -powmof
#AM_LFLAGS = -d -i -Powmof


AM_YFLAGS = -d -powmof
AM_LFLAGS = -i -Cfe -Powmof -olex.yy.c


