include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs


noinst_LIBRARIES = \
libsampleprovider.a 

#libindprovider.a

OW_SHARED_LIBS = \
libsampleprovider.so 

#libindprovider.so

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

