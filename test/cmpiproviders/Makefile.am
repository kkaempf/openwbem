include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = \
libcmpiTestProvider.a 

#libindprovider.a

OW_SHARED_LIBS = \
libcmpiTestProvider.so 

#libindprovider.so

all-local: $(OW_SHARED_LIBS)
	
clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done


#libindprovider_a_SOURCES = \
#IndProvider.c

libcmpiTestProvider_a_SOURCES = \
cmpiTestProvider.c

EXTRA_DIST = \
cmpiProviderTest.mof

INCLUDES = \
-I$(top_srcdir)/src/providerifcs/cmpi/common 

