include $(top_srcdir)/Makefile-shared-lib.inc

OW_SHARED_LIBS = libowwql.so
SHAREDLIBADD = -lpthread
thelibdir = $(libdir)

all-local: $(OW_SHARED_LIBS)

install-exec-local:
	@INSTALL@ -d $(DESTDIR)$(thelibdir)
	for i in $(OW_SHARED_LIBS); do \
		@INSTALL@  $$i.$(SO_EXT) $(DESTDIR)$(thelibdir); \
		ln -s $$i.$(SO_EXT) $(DESTDIR)$(thelibdir)/$$i; \
	done
	
clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done



noinst_LIBRARIES = libowwql.a

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/client

pkginclude_HEADERS = \
OW_WQLImpl.hpp \
OW_WQLScanUtils.hpp \
OW_WQLAst.hpp \
OW_WQLProcessor.hpp \
OW_WQLVisitor.hpp 

libowwql_a_SOURCES = \
OW_WQLParser.yy \
OW_WQLLexer.ll \
OW_WQLScanUtils.cpp \
OW_WQLProcessor.cpp \
OW_WQLImpl.cpp

EXTRA_DIST = \
$(top_builddir)/src/wql/OW_WQLParser.cc \
$(top_builddir)/src/wql/OW_WQLLexer.cc \
$(top_builddir)/src/wql/OW_WQLParser.h

#debug versions
#AM_YFLAGS = -d -t -v
#AM_LFLAGS = -d -i


AM_YFLAGS = -d
AM_LFLAGS = -i -Cfe

