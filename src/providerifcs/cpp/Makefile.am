include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


noinst_LIBRARIES = libowcppprovifc.a
OW_SHARED_LIBS = libowcppprovifc.so
SHAREDLIBADD =

thelibdir = $(libdir)


libowcppprovifc_a_SOURCES = \
OW_CppProviderIFC.cpp \
OW_CppProxyProvider.cpp \
OW_CppProxyProvider.hpp \
OW_CppProviderIFC.hpp \
OW_CppIndicationProviderIFC.cpp \
OW_CppSimpleInstanceProviderIFC.cpp \
OW_CppSimpleAssociatorProviderIFC.cpp \
OW_CppReadOnlyInstanceProviderIFC.cpp

if DISABLE_ASSOCIATION_TRAVERSAL
ASSOCIATION_H=
else
ASSOCIATION_H=\
OW_CppAssociatorProviderIFC.hpp
endif

pkginclude_HEADERS = \
OW_CppProviderBaseIFC.hpp \
$(ASSOCIATION_H) \
OW_CppIndicationExportProviderIFC.hpp \
OW_CppInstanceProviderIFC.hpp \
OW_CppMethodProviderIFC.hpp \
OW_CppPolledProviderIFC.hpp \
OW_CppIndicationProviderIFC.hpp \
OW_CppSimpleInstanceProviderIFC.hpp \
OW_CppSimpleAssociatorProviderIFC.hpp \
OW_CppReadOnlyInstanceProviderIFC.hpp

#libOW_CppIFC_a_LDFLAGS = -L$(top_builddir)/src/common/
#libOW_CppIFC_a_LIBADD = -lOpenWBEM

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/server

