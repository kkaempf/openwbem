MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


noinst_LIBRARIES = libowindicationserver.a libowindicationreplayer.a
SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/cimom/common -lowcimomcommon \
-L$(top_builddir)/src/cimom/server -lowserver \
-L$(top_builddir)/src/wql/common -lowwqlcommon \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc \
-L$(top_builddir)/src/repositories/hdb -lowrepositoryhdb \
-L$(top_builddir)/db/db -lowdb

OW_SHARED_LIBS = libowindicationserver.$(LIB_EXT) libowindicationreplayer.$(LIB_EXT)
thelibdir = $(libdir)/openwbem

libowindicationserver_a_SOURCES = \
OW_IndicationServerImpl.hpp \
OW_IndicationServerImpl.cpp \
OW_LifecycleIndicationPoller.hpp \
OW_LifecycleIndicationPoller.cpp

libowindicationreplayer_a_SOURCES = \
OW_IndicationRepLayerImpl.hpp \
OW_IndicationRepLayerImpl.cpp

INCLUDES = \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cimom/server \
-I$(top_srcdir)/src/cimom/common \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/wql/common


