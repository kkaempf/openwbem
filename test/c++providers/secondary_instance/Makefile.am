include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs


noinst_LIBRARIES = \
libsecondaryinstancetest.a


OW_SHARED_LIBS = \
libsecondaryinstancetest.$(LIB_EXT)

libsecondaryinstancetest_a_SOURCES = \
SecondaryInstanceTest.cpp

EXTRA_DIST = \
SecondaryInstanceTest.mof

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/ifcs

SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc

