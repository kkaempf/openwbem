MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
check-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs


check_LIBRARIES = libassociatortest.a 
SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc


OW_SHARED_LIBS = libassociatortest.$(LIB_EXT)

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

libassociatortest_a_SOURCES = \
OW_AssociatorTest.cpp

EXTRA_DIST = OW_AssociatorTest.mof

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/provider

