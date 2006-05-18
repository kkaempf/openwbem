MAKEDYLIB=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


owstatlib_LIBRARIES = libowembeddedcimom.a
SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/cimom/server -lowserver \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/repositories/hdb -lowrepositoryhdb \
-L$(top_builddir)/db/db -lowdb \
-L$(top_builddir)/src/cimom/common -lowcimomcommon \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc


INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/cimom/server \
-I$(top_srcdir)/src/cimom/common \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/repositories/hdb \
-I$(top_srcdir)/src/providerifcs/cpp

OW_SHARED_LIBS = libowembeddedcimom.$(LIB_EXT)
# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)
thelibdir = $(libdir)
owstatlibdir = $(thelibdir)

libowembeddedcimom_a_SOURCES =	\
OW_EmbeddedCIMOMEnvironment.cpp \
OW_LocalEmbeddedCIMOMHandle.cpp 

pkginclude_HEADERS = \
OW_EmbeddedCIMOMEnvironment.hpp \
OW_LocalEmbeddedCIMOMHandle.hpp

