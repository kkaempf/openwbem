include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs


noinst_LIBRARIES = libindicationtest1.a libindicationtest2.a libindicationtest3.a
SHAREDLIBADD =


OW_SHARED_LIBS = libindicationtest1.so libindicationtest2.so libindicationtest3.so

libindicationtest1_a_SOURCES = \
OW_IndicationProviderTest1.cpp

libindicationtest2_a_SOURCES = \
OW_IndicationProviderTest2.cpp

libindicationtest3_a_SOURCES = \
OW_IndicationProviderTest3.cpp


INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/wql/common \
-I$(top_srcdir)/src/cimom



