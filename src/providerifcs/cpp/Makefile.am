include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


noinst_LIBRARIES = libcppprovifc.a
OW_SHARED_LIBS = libcppprovifc.so
SHAREDLIBADD =

thelibdir = $(pkglibdir)/provifcs

pkginclude_HEADERS = \
OW_CppAssociatorProviderIFC.hpp \
OW_CppIndicationExportProviderIFC.hpp \
OW_CppInstanceProviderIFC.hpp \
OW_CppMethodProviderIFC.hpp \
OW_CppPolledProviderIFC.hpp \
OW_CppPropertyProviderIFC.hpp \
OW_CppProviderBaseIFC.hpp

libcppprovifc_a_SOURCES = \
OW_CppProviderIFC.cpp \
OW_CppProviderIFC.hpp \
OW_CppProxyProvider.cpp \
OW_CppProxyProvider.hpp

#libOW_CppIFC_a_LDFLAGS = -L$(top_builddir)/src/common/
#libOW_CppIFC_a_LIBADD = -lOpenWBEM

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/server

