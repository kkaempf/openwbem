MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

EXPORT_PREFIX=createProvider

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs

noinst_LIBRARIES = libowprovinstCIM_ObjectManagerCommunicationMechanism.a

thelibdir = $(pkglibdir)/c++providers

OW_SHARED_LIBS = libowprovinstCIM_ObjectManagerCommunicationMechanism.$(LIB_EXT)

libowprovinstCIM_ObjectManagerCommunicationMechanism_a_SOURCES = \
OW_provinstCIM_ObjectManagerCommunicationMechanism.cpp

SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/cimom/server -lowserver \
-L$(top_builddir)/src/cimom/common -lowcimomcommon \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc \
-L$(top_builddir)/src/repositories/hdb -lowrepositoryhdb \
-L$(top_builddir)/db/db -lowdb

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/cimom/common \
-I$(top_srcdir)/src/cimom/server


