include $(top_srcdir)/Makefile-shared-lib.inc

SUBDIRS = testfiles

noinst_LIBRARIES = libowmofc.a 
thelibdir = $(libdir)

OW_SHARED_LIBS = libowmofc.so
SHAREDLIBADD =

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


INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim

libowmofc_a_SOURCES = \
Visitor.cpp \
mofparser.cpp \
moflexer.cpp \
mofparser.ypp \
moflexer.lpp \
CIMOMVisitor.cpp \
MofCompiler.cpp

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

BUILT_SOURCES = mofparser.cpp moflexer.cpp mofparser.tab.h
EXTRA_DIST = $(BUILT_SOURCES) \
DebugVisitor.cpp \
DebugVisitor.h


$(srcdir)/mofparser.tab.h $(srcdir)/mofparser.cpp: $(srcdir)/mofparser.ypp
	bison -d -v $(srcdir)/mofparser.ypp && mv $(srcdir)/mofparser.ypp.tab.c $(srcdir)/mofparser.cpp && mv $(srcdir)/mofparser.ypp.tab.h $(srcdir)/mofparser.tab.h

$(srcdir)/moflexer.cpp: $(srcdir)/moflexer.lpp
	flex -i -Cfe $(srcdir)/moflexer.lpp && mv lex.yy.c $(srcdir)/moflexer.cpp

