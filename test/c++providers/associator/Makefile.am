include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs


noinst_LIBRARIES = libassociatortest.a 
SHAREDLIBADD =

OW_SHARED_LIBS = libassociatortest.so

libassociatortest_a_SOURCES = \
OW_AssociatorTest.cpp

EXTRA_DIST = OW_AssociatorTest.mof

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/provider

