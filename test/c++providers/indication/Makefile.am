MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs


noinst_LIBRARIES = libindicationtest1.a libindicationtest2.a libindicationtest3.a

OW_SHARED_LIBS = libindicationtest1.$(LIB_EXT) libindicationtest2.$(LIB_EXT) libindicationtest3.$(LIB_EXT)

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

libindicationtest1_a_SOURCES = \
OW_IndicationProviderTest1.cpp

libindicationtest2_a_SOURCES = \
OW_IndicationProviderTest2.cpp

libindicationtest3_a_SOURCES = \
OW_IndicationProviderTest3.cpp


INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/wql/common \
-I$(top_srcdir)/src/cimom/common \
-I$(top_srcdir)/src/cimom/server

SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/wql/common -lowwqlcommon \
-L$(top_builddir)/src/cimom/common -lowcimomcommon \
-L$(top_builddir)/src/cimom/server -lowserver \
-L$(top_builddir)/src/repositories/hdb -lowrepositoryhdb \
-L$(top_builddir)/db/db -lowdb


