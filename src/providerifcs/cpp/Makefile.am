include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libowcppprovifc.a
OW_SHARED_LIBS = libowcppprovifc.so
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




libowcppprovifc_a_SOURCES = \
OW_CppAssociatorProviderIFC.hpp \
OW_CppIndicationExportProviderIFC.hpp \
OW_CppInstanceProviderIFC.hpp \
OW_CppMethodProviderIFC.hpp \
OW_CppPolledProviderIFC.hpp \
OW_CppPropertyProviderIFC.hpp \
OW_CppProviderBaseIFC.hpp \
OW_CppProviderIFC.cpp \
OW_CppProviderIFC.hpp \
OW_CppProxyProvider.cpp \
OW_CppProxyProvider.hpp \
OW_CppIndicationProviderIFC.hpp \
OW_CppIndicationProviderIFC.cpp

#libOW_CppIFC_a_LDFLAGS = -L$(top_builddir)/src/common/
#libOW_CppIFC_a_LIBADD = -lOpenWBEM

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/server

