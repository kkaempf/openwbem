include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


noinst_LIBRARIES = libowprovinstCIM_IndicationFilter.a

thelibdir = $(pkglibdir)/c++providers

OW_SHARED_LIBS = libowprovinstCIM_IndicationFilter.so

libowprovinstCIM_IndicationFilter_a_SOURCES = \
OW_provinstCIM_IndicationFilter.cpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/cimom \
-I$(top_srcdir)/src/cimom/indication


