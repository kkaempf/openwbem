include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libcpiprovifc.a
OW_SHARED_LIBS = libcpiprovifc.so
SHAREDLIBADD = -lpthread

thelibdir = $(pkglibdir)/provifcs

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

libcpiprovifc_a_SOURCES = \
OW_CPI.hpp \
OW_CPI.cpp \
owcpiprov.h \
owcpicls.cpp \
owcpiinst.cpp \
owcpimeth.cpp \
owcpins.cpp \
owcpivect.cpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/server

