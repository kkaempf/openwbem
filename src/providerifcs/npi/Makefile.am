MAKEBUNDLE=1
SUBDIRS = common
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

EXPORT_PREFIX=createProviderIFC

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


noinst_LIBRARIES = libnpiprovifc.a
OW_SHARED_LIBS = libnpiprovifc.$(LIB_EXT)

libnpiprovifc_PROJECT_LIBS = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/providerifcs/npi/common -lnpisupport

libnpiprovifc_EXTERNAL_LIBS = \
$(BLOCXX_LIBS)

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

thelibdir = $(pkglibdir)/provifcs

if DISABLE_ASSOCIATION_TRAVERSAL
ASSOCIATION_S=
else
ASSOCIATION_S= \
OW_NPIAssociatorProviderProxy.cpp \
OW_NPIAssociatorProviderProxy.hpp
endif

libnpiprovifc_a_SOURCES = \
NPIProvider.hpp \
$(ASSOCIATION_S) \
OW_NPIInstanceProviderProxy.cpp \
OW_NPIInstanceProviderProxy.hpp \
OW_NPIMethodProviderProxy.cpp \
OW_NPIMethodProviderProxy.hpp \
OW_NPIPolledProviderProxy.cpp \
OW_NPIPolledProviderProxy.hpp \
OW_NPIProviderIFC.cpp \
OW_NPIProviderIFC.hpp \
OW_NPIProviderIFCUtils.hpp \
NPIProvider.hpp \
OW_NPIIndicationProviderProxy.cpp \
OW_NPIIndicationProviderProxy.hpp \
OW_FTABLERef.hpp

#libOW_CppIFC_a_LDFLAGS = -L$(top_builddir)/src/common/
#libOW_CppIFC_a_LIBADD = -lOpenWBEM

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/npi/common \
-I$(top_srcdir)/src/cimom/server

