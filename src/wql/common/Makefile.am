include $(top_srcdir)/Makefile-shared-lib.inc

OW_SHARED_LIBS = libowwqlcommon.so
SHAREDLIBADD =
thelibdir = $(libdir)

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

noinst_LIBRARIES = libowwqlcommon.a

INCLUDES = \
-I$(top_srcdir)/src/common

libowwqlcommon_a_SOURCES = \
OW_WQLCompile.cpp \
OW_WQLOperand.cpp \
OW_WQLPropertySource.cpp \
OW_WQLSelectStatement.cpp

pkginclude_HEADERS = \
OW_WQLCompile.hpp \
OW_WQLOperand.hpp \
OW_WQLOperation.hpp \
OW_WQLPropertySource.hpp \
OW_WQLSelectStatement.hpp

