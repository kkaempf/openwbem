EXTRA_DIST = \
oopTest.mof.in

BUILT_SOURCES = \
oopTest.mof


include ${top_srcdir}/Makefile-replacement.inc

global_substitutions = \
-e 's!@provdir@!${libdir}/openwbem/oopproviders!g'


MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

noinst_LIBRARIES = \
libooptest.a \
liboopassoctest.a

OW_SHARED_LIBS = \
libooptest.$(LIB_EXT) \
liboopassoctest.$(LIB_EXT)

LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

libooptest_a_SOURCES = \
testOOPProv.cpp

liboopassoctest_a_SOURCES = \
testOOPAssocProv.cpp

CLEANFILES = oopTest.mof

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/provider

LOCAL_LIBS = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc

EXTERNAL_LIBS = \
$(BLOCXX_LIBS)

libooptest_PROJECT_LIBS = $(LOCAL_LIBS)
libooptest_EXTERNAL_LIBS = $(EXTERNAL_LIBS)
liboopassoctest_PROJECT_LIBS = $(LOCAL_LIBS)
liboopassoctest_EXTERNAL_LIBS = $(EXTERNAL_LIBS)
