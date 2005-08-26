MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
check-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs


check_LIBRARIES = libindicationtest1.a libindicationtest2.a 

OW_SHARED_LIBS = libindicationtest1.$(LIB_EXT) libindicationtest2.$(LIB_EXT) 

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

libindicationtest1_a_SOURCES = \
OW_IndicationProviderTest1.cpp

libindicationtest2_a_SOURCES = \
OW_IndicationProviderTest2.cpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/wql/common

SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/wql/common -lowwqlcommon


