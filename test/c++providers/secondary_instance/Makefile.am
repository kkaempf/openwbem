MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs


noinst_LIBRARIES = \
libsecondaryinstancetest1.a \
libsecondaryinstancetest2.a


OW_SHARED_LIBS = \
libsecondaryinstancetest1.$(LIB_EXT) \
libsecondaryinstancetest2.$(LIB_EXT)

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

libsecondaryinstancetest1_a_SOURCES = \
SecondaryInstanceTest1.cpp

libsecondaryinstancetest2_a_SOURCES = \
SecondaryInstanceTest2.cpp

EXTRA_DIST = \
SecondaryInstanceTest.mof

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/ifcs

SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem $(BLOCXX_LIBS) \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc

