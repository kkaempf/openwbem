include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


noinst_LIBRARIES = libowprovinstOW_NameSpace.a

thelibdir = $(pkglibdir)/c++providers

OW_SHARED_LIBS = libowprovinstOW_NameSpace.so

libowprovinstOW_NameSpace_a_SOURCES = \
OW_NameSpaceProvider.hpp \
OW_NameSpaceProvider.cpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/cpp


