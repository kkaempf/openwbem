MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

EXPORT_PREFIX=createProvider

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs

noinst_LIBRARIES = libowprovCIM_IndicationSubscription.a

thelibdir = $(pkglibdir)/c++providers

OW_SHARED_LIBS = libowprovCIM_IndicationSubscription.$(LIB_EXT)

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

libowprovCIM_IndicationSubscription_a_SOURCES = \
OW_provCIM_IndicationSubscription.cpp

SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/cimom/server -lowserver \
-L$(top_builddir)/src/cimom/common -lowcimomcommon \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc \
-L$(top_builddir)/src/repositories/hdb -lowrepositoryhdb \
-L$(top_builddir)/db/db -lowdb

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/cimom/server \
-I$(top_srcdir)/src/cimom/common \
-I$(top_srcdir)/src/cimom/indication \
-I$(top_srcdir)/src/monitor


