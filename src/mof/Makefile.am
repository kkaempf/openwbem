include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


noinst_LIBRARIES = libowmofc.a 
thelibdir = $(libdir)

OW_SHARED_LIBS = libowmofc.so
SHAREDLIBADD =

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim

libowmofc_a_SOURCES = \
OW_MOFVisitor.cpp \
OW_MOFParser.cc \
OW_MOFLexer.cc \
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

#libowmofc_a_LDFLAGS = -L$(top_builddir)/src/common/.libs

#libowmofc_a_LIBADD = -lopenwbem


EXTRA_DIST = \
$(top_builddir)/src/mof/OW_MOFParser.cc \
$(top_builddir)/src/mof/OW_MOFLexer.cc \
$(top_builddir)/src/mof/OW_MOFParser.h

#debug versions
#AM_YFLAGS = -d -t -v
#AM_LFLAGS = -d -i


AM_YFLAGS = -d
AM_LFLAGS = -i -Cfe


