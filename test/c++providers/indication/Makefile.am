include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libindicationtest1.a libindicationtest2.a
SHAREDLIBADD =


OW_SHARED_LIBS = libindicationtest1.so libindicationtest2.so

all-local: $(OW_SHARED_LIBS)
	
clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done


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



