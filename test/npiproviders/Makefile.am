include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = \
libsampleprovider.a 

#libindprovider.a

OW_SHARED_LIBS = \
libsampleprovider.so 

#libindprovider.so

all-local: $(OW_SHARED_LIBS)
	
clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done


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

