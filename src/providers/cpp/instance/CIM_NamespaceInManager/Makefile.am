MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

EXPORT_PREFIX=createProvider

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs

noinst_LIBRARIES = libowprovinstCIM_NamespaceInManager.a

thelibdir = $(pkglibdir)/c++providers

OW_SHARED_LIBS = libowprovinstCIM_NamespaceInManager.$(LIB_EXT)

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

libowprovinstCIM_NamespaceInManager_a_SOURCES = \
OW_provinstCIM_NamespaceInManager.cpp

libowprovinstCIM_NamespaceInManager_PROJECT_LIBS = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc

libowprovinstCIM_NamespaceInManager_EXTERNAL_LIBS = \
$(BLOCXX_LIBS)

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/cpp


