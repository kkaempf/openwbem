if HAVE_SLP
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


noinst_LIBRARIES = libowslpprovider.a

thelibdir = $(pkglibdir)/c++providers

OW_SHARED_LIBS = libowslpprovider.so

libowslpprovider_a_SOURCES = \
OW_SLPProvider.cpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/thread \
-I$(top_srcdir)/src/server \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/client

else

clean-local:

EXTRA_DIST = \
OW_SLPProvider.cpp

endif


