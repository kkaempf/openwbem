MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

EXPORT_PREFIX=createProvider

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs

noinst_LIBRARIES = libowprovinstOpenWBEM_ObjectManager.a

thelibdir = $(pkglibdir)/c++providers

OW_SHARED_LIBS = libowprovinstOpenWBEM_ObjectManager.$(LIB_EXT)

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

libowprovinstOpenWBEM_ObjectManager_a_SOURCES = \
OW_provinstOpenWBEM_ObjectManager.cpp

libowprovinstOpenWBEM_ObjectManager_PROJECT_LIBS = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc

libowprovinstOpenWBEM_ObjectManager_EXTERNAL_LIBS = \
$(BLOCXX_LIBS)

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/cpp


