MAKEDYLIB=1
MAKEBUNDLE=1

include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

EXPORT_PREFIX=createWQL

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


OW_SHARED_LIBS = libowwql.$(LIB_EXT)
SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem -lblocxx

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

thelibdir = $(libdir)
owstatlibdir = $(thelibdir)

owstatlib_LIBRARIES = libowwql.a

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/client

pkginclude_HEADERS = \
OW_WQLImpl.hpp \
OW_WQLScanUtils.hpp \
OW_WQLAst.hpp \
OW_WQLVisitor.hpp 

libowwql_a_SOURCES = \
OW_WQLParser.yy \
OW_WQLLexer.ll \
OW_WQLAst.cpp \
OW_WQLScanUtils.cpp \
OW_WQLProcessor.hpp \
OW_WQLProcessor.cpp \
OW_WQLImpl.cpp \
OW_WQLSelectStatementGen.hpp \
OW_WQLSelectStatementGen.cpp

EXTRA_DIST = \
$(top_builddir)/src/wql/OW_WQLParser.cc \
$(top_builddir)/src/wql/OW_WQLLexer.cc \
$(top_builddir)/src/wql/OW_WQLParser.h

BUILT_SOURCES = OW_WQLParser.h

#debug versions
#AM_YFLAGS = -d -t -v -powwql
#AM_LFLAGS = -d -i -Powwql


AM_YFLAGS = -d -powwql
AM_LFLAGS = -i -Cfe -Powwql -olex.yy.c

