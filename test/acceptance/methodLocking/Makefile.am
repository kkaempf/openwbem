EXTRA_DIST = \
methodLockingTest.mof.in

BUILT_SOURCES = \
methodLockingTest.mof


include ${top_srcdir}/Makefile-replacement.inc

global_substitutions = \
-e 's!@provdir@!${libdir}/openwbem/oopproviders!g'


MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

noinst_LIBRARIES = \
libmethodLockingTest.a

OW_SHARED_LIBS = \
libmethodLockingTest.$(LIB_EXT)

LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

libmethodLockingTest_a_SOURCES = \
testMethodLockingProv.cpp \
testMethodLocking.cpp

noinst_PROGRAMS = \
testMethodLocking

testMethodLocking_SOURCES = \
testMethodLocking_main.cpp \
testMethodLocking.cpp

testMethodLocking_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/wql -lowwql \
-L$(top_builddir)/src/common -lopenwbem $(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

testMethodLocking_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o testMethodLocking$(EXEEXT)

CLEANFILES = methodLockingTest.mof

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/provider

SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem $(BLOCXX_LIBS) \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc

