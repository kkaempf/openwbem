MAKEDYLIB=1
MAKEDBUNDLE=1
SUBDIRS = common

include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

EXPORT_PREFIX=createWQL

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
	if [ "$(LIB_EXT)" = "dylib" ]; then \
		if [ -f $(DESTDIR)$(thelibdir)/`echo $(OW_SHARED_LIBS)|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT).bundle ]; then \
			mv $(DESTDIR)$(thelibdir)/`echo $(OW_SHARED_LIBS)|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT) $(DESTDIR)$(thelibdir)/`echo $(OW_SHARED_LIBS)|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT).dso; \
			cp $(DESTDIR)$(thelibdir)/`echo $(OW_SHARED_LIBS)|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT).bundle $(DESTDIR)$(thelibdir)/`echo $(OW_SHARED_LIBS)|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT); \
			ln -sf $(DESTDIR)$(thelibdir)/`echo $(OW_SHARED_LIBS)|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT) $(DESTDIR)$(thelibdir)/$(OW_SHARED_LIBS); \
			fi; \
	fi;

uninstall-local : uninstall-ow-shared-libs


OW_SHARED_LIBS = libowwql.$(LIB_EXT)
SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/wql/common -lowwqlcommon

thelibdir = $(libdir)

noinst_LIBRARIES = libowwql.a

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/wql/common

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

#debug versions
#AM_YFLAGS = -d -t -v -powwql
#AM_LFLAGS = -d -i -Powwql


AM_YFLAGS = -d -powwql
AM_LFLAGS = -i -Cfe -Powwql -olex.yy.c

