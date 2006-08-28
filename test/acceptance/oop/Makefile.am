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
libooptest.a


OW_SHARED_LIBS = \
libooptest.$(LIB_EXT)

LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

libooptest_a_SOURCES = \
testOOPProv.cpp

CLEANFILES = oopTest.mof

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/provider

SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc
