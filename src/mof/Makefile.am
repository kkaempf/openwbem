include $(top_srcdir)/Makefile-shared-lib.inc

SUBDIRS = testfiles

noinst_LIBRARIES = libowmofc.a 
thelibdir = $(libdir)

OW_SHARED_LIBS = libowmofc.so

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

bin_PROGRAMS = owmofc



INCLUDES = \
-I$(top_srcdir)/src \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/http \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/net/socket \
-I$(top_srcdir)/src/thread

libowmofc_a_SOURCES = \
Visitor.cpp \
mofparser.cpp \
moflexer.cpp \
mofparser.ypp \
moflexer.lpp \
MofCompiler.cpp

pkginclude_HEADERS = \
Grammar.h \
Visitor.h \
OW_MofParserErrorHandlerIFC.hpp \
MofCompiler.hpp \
MofParserDecls.hpp \
LineInfo.hpp

#libowmofc_a_LDFLAGS = -L$(top_builddir)/src/common/.libs

#libowmofc_a_LIBADD = -lopenwbem

owmofc_SOURCES = \
CIMOMVisitor.hpp \
CIMOMVisitor.cpp \
main.cpp

owmofc_DEPENDENCIES = $(OW_SHARED_LIBS)

owmofc_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owmofc

owmofc_LDADD = \
-L../common -lopenwbem \
-L. -lowmofc

BUILT_SOURCES = mofparser.cpp moflexer.cpp mofparser.tab.h
EXTRA_DIST = $(BUILT_SOURCES) \
DebugVisitor.cpp \
DebugVisitor.h


$(srcdir)/mofparser.tab.h $(srcdir)/mofparser.cpp: $(srcdir)/mofparser.ypp
	bison -d -v $(srcdir)/mofparser.ypp && mv $(srcdir)/mofparser.ypp.tab.c $(srcdir)/mofparser.cpp && mv $(srcdir)/mofparser.ypp.tab.h $(srcdir)/mofparser.tab.h

$(srcdir)/moflexer.cpp: $(srcdir)/moflexer.lpp
	flex -i -Cfe $(srcdir)/moflexer.lpp && mv lex.yy.c $(srcdir)/moflexer.cpp

