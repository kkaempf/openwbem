MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs


noinst_LIBRARIES = \
libsampleprovider.a 

#libindprovider.a

OW_SHARED_LIBS = \
libsampleprovider.$(LIB_EXT) 

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

#libindprovider.$(LIB_EXT)

#libindprovider_a_SOURCES = \
#IndProvider.c

libsampleprovider_a_SOURCES = \
SampleProvider.c

#EXTRA_DIST = \
#IndicationTest.mof \
#testind.mof \
#SampleProvider.mof

INCLUDES = \
-I$(top_srcdir)/src/providerifcs/npi/common 


libsampleprovider_PROJECT_LIBS = \
-L$(top_builddir)/src/providerifcs/npi/common -lnpisupport
