MAKEDYLIB=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


owstatlib_LIBRARIES = libowcppprovifc.a
OW_SHARED_LIBS = libowcppprovifc.$(LIB_EXT)
SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/provider -lowprovider

thelibdir = $(libdir)
owstatlibdir = $(thelibdir)

if DISABLE_ASSOCIATION_TRAVERSAL
ASSOCIATION_S=
else
ASSOCIATION_S=\
OW_CppSimpleAssociatorProviderIFC.cpp \
OW_CppAssociatorProviderIFC.cpp
endif

libowcppprovifc_a_SOURCES = \
$(ASSOCIATION_S) \
OW_CppProviderBaseIFC.cpp \
OW_CppProviderIFC.cpp \
OW_CppProxyProvider.cpp \
OW_CppProxyProvider.hpp \
OW_CppProviderIFC.hpp \
OW_CppIndicationExportProviderIFC.cpp \
OW_CppIndicationProviderIFC.cpp \
OW_CppMethodProviderIFC.cpp \
OW_CppPolledProviderIFC.cpp \
OW_CppSimpleInstanceProviderIFC.cpp \
OW_CppInstanceProviderIFC.cpp \
OW_CppSecondaryInstanceProviderIFC.cpp \
OW_CppReadOnlyInstanceProviderIFC.cpp


if DISABLE_ASSOCIATION_TRAVERSAL
ASSOCIATION_H=
else
ASSOCIATION_H=\
OW_CppSimpleAssociatorProviderIFC.hpp \
OW_CppAssociatorProviderIFC.hpp
endif

pkginclude_HEADERS = \
OW_CppProviderBaseIFC.hpp \
$(ASSOCIATION_H) \
OW_CppIndicationExportProviderIFC.hpp \
OW_CppInstanceProviderIFC.hpp \
OW_CppSecondaryInstanceProviderIFC.hpp \
OW_CppMethodProviderIFC.hpp \
OW_CppPolledProviderIFC.hpp \
OW_CppIndicationProviderIFC.hpp \
OW_CppSimpleInstanceProviderIFC.hpp \
OW_CppReadOnlyInstanceProviderIFC.hpp \
OW_CppProviderIncludes.hpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/provider

