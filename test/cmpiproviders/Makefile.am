MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs


noinst_LIBRARIES = \
libcmpiTestProvider.a 

OW_SHARED_LIBS = \
libcmpiTestProvider.$(LIB_EXT) 

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = 4

thelibdir = $(pkglibdir)/cmpiproviders

libcmpiTestProvider_a_SOURCES = \
cmpiTestProvider.c

EXTRA_DIST = \
cmpiProviderTest.mof

INCLUDES = \
-I$(top_srcdir)/src/providerifcs/cmpi/common \
-I$(top_srcdir)/src/common 

