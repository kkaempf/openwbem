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
Visitor.cpp \
mofparser.cc \
moflexer.cc \
mofparser.yy \
moflexer.ll \
CIMOMVisitor.cpp \
MofCompiler.cpp \
OW_MofParserErrorHandlerIFC.cpp \
OW_MofParseFatalErrorException.cpp

pkginclude_HEADERS = \
Grammar.h \
Visitor.h \
OW_MofParserErrorHandlerIFC.hpp \
CIMOMVisitor.hpp \
MofCompiler.hpp \
MofParserDecls.hpp \
LineInfo.hpp

#libowmofc_a_LDFLAGS = -L$(top_builddir)/src/common/.libs

#libowmofc_a_LIBADD = -lopenwbem


EXTRA_DIST = \
$(top_builddir)/src/mof/mofparser.cc \
$(top_builddir)/src/mof/moflexer.cc \
$(top_builddir)/src/mof/mofparser.h \
DebugVisitor.cpp \
DebugVisitor.h

#debug versions
#AM_YFLAGS = -d -t -v
#AM_LFLAGS = -d -i


AM_YFLAGS = -d
AM_LFLAGS = -i -Cfe


