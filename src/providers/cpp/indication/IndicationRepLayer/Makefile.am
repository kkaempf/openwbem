include $(top_srcdir)/Makefile-shared-lib.inc
include $(top_srcdir)/Makefile-shared-lib-inst.inc

noinst_LIBRARIES = libowprovindIndicationRepLayer.a 
SHAREDLIBADD =

thelibdir = $(pkglibdir)/c++providers

OW_SHARED_LIBS = libowprovindIndicationRepLayer.so

libowprovindIndicationRepLayer_a_SOURCES = \
OW_provindIndicationRepLayer.cpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/wql/common \
-I$(top_srcdir)/src/cimom



