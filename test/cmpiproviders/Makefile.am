include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = \
libcmpiTestProvider.a 

OW_SHARED_LIBS = \
libcmpiTestProvider.so 

thelibdir = $(pkglibdir)/cmpiproviders

all-local: $(OW_SHARED_LIBS)

clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done


libcmpiTestProvider_a_SOURCES = \
cmpiTestProvider.c

EXTRA_DIST = \
cmpiProviderTest.mof

INCLUDES = \
-I$(top_srcdir)/src/providerifcs/cmpi/common \
-I$(top_srcdir)/src/common 

