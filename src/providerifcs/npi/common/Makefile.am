include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libnpisupport.a
OW_SHARED_LIBS = libnpisupport.so
SHAREDLIBADD =

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


libnpisupport_a_SOURCES = \
NPIExternal.cpp \
NPIExternal.hpp \
npicrash.c \
npicrash.h \
provider.h \
mlogsup.c \
mlogsup.h \
npi.h \
npi_import.h \
os_compile.h

#libOW_CppIFC_a_LDFLAGS = -L$(top_builddir)/src/common/
#libOW_CppIFC_a_LIBADD = -lOpenWBEM


INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/npi \
-I$(top_srcdir)/src/server 

