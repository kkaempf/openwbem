include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libowmofc.a 
thelibdir = $(libdir)

OW_SHARED_LIBS = libowmofc.so
SHAREDLIBADD =

all-local: $(OW_SHARED_LIBS)

install-exec-local:
	@INSTALL@ -d $(DESTDIR)$(thelibdir)
	for i in $(OW_SHARED_LIBS); do \
		@INSTALL@  $$i.$(SO_EXT) $(DESTDIR)$(thelibdir); \
		ln -sf $$i.$(SO_EXT) $(DESTDIR)$(thelibdir)/$$i; \
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


