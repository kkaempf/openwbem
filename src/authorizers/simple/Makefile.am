MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

EXPORT_PREFIX=createAuthorizer

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


OW_SHARED_LIBS = libowsimpleauthorizer.$(LIB_EXT)

libowsimpleauthorizer_PROJECT_LIBS = \
-L$(top_builddir)/src/common -lopenwbem

libowsimpleauthorizer_EXTERNAL_LIBS = \
$(BLOCXX_LIBS)

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

thelibdir = $(libdir)/openwbem

noinst_LIBRARIES = libowsimpleauthorizer.a

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/cim

libowsimpleauthorizer_a_SOURCES = \
OW_SimpleAuthorizer.hpp \
OW_SimpleAuthorizer.cpp

