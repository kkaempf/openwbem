include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libmethodtest.a 


OW_SHARED_LIBS = libmethodtest.so

all-local: $(OW_SHARED_LIBS)
	
clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done


libmethodtest_a_SOURCES = \
OW_MethodProviderTest.cpp

EXTRA_DIST = OW_MethodProviderTest.mof

INCLUDES = \
-I$(top_srcdir)/src/common -I$(top_srcdir)/src/thread -I$(top_srcdir)/src \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/server \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/http \
-I$(top_srcdir)/src/cimom


