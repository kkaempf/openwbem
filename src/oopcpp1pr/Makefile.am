SUBDIRS = . bin

MAKEDYLIB=1
MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


owstatlib_LIBRARIES = libowoopcpp1pr.a
owstatlibdir = $(thelibdir)

OW_SHARED_LIBS = libowoopcpp1pr.$(LIB_EXT)

libowoopcpp1pr_PROJECT_LIBS = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/requesthandlers/binary -lowrequesthandlerbinary

libowoopcpp1pr_EXTERNAL_LIBS = \
$(BLOCXX_LIBS)

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

thelibdir = $(libdir)

libowoopcpp1pr_a_SOURCES = \
OW_OOPCpp1PR.cpp

pkginclude_HEADERS = \
OW_OOPCpp1PR.hpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/http/common \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/requesthandlers/binary \
-I$(top_srcdir)/src/monitor

