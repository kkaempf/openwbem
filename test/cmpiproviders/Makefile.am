include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs


noinst_LIBRARIES = \
libcmpiTestProvider.a 

OW_SHARED_LIBS = \
libcmpiTestProvider.so 

thelibdir = $(pkglibdir)/cmpiproviders

libcmpiTestProvider_a_SOURCES = \
cmpiTestProvider.c

EXTRA_DIST = \
cmpiProviderTest.mof

INCLUDES = \
-I$(top_srcdir)/src/providerifcs/cmpi/common \
-I$(top_srcdir)/src/common 

