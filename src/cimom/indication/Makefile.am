include $(top_srcdir)/Makefile-shared-lib.inc
include $(top_srcdir)/Makefile-shared-lib-inst.inc

noinst_LIBRARIES = libowindicationserver.a libowindicationreplayer.a
SHAREDLIBADD = -L$(top_builddir)/src/wql/common -lowwqlcommon

OW_SHARED_LIBS = libowindicationserver.so libowindicationreplayer.so
thelibdir = $(libdir)/openwbem

libowindicationserver_a_SOURCES = \
OW_IndicationServer.hpp \
OW_IndicationServerImpl.hpp \
OW_IndicationServerImpl.cpp \
OW_LifecycleIndicationPoller.hpp \
OW_LifecycleIndicationPoller.cpp

libowindicationreplayer_a_SOURCES = \
OW_IndicationRepLayer.hpp \
OW_IndicationRepLayerImpl.hpp \
OW_IndicationRepLayerImpl.cpp

INCLUDES = \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/cimom \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/server \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/wql/common


