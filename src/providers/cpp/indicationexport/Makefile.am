MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

EXPORT_PREFIX=createProvider

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs

noinst_LIBRARIES = libcppindicationexportxmlhttpprovider.a

OW_SHARED_LIBS = libcppindicationexportxmlhttpprovider.$(LIB_EXT)
SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc \
-L$(top_builddir)/src/xml -lowxml 

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = 4

thelibdir = $(pkglibdir)/c++providers

libcppindicationexportxmlhttpprovider_a_SOURCES = \
OW_CppIndicationExportXMLHTTPProvider.cpp \
OW_CppIndicationExportXMLHTTPProvider.hpp \
OW_IndicationExporter.hpp \
OW_IndicationExporter.cpp


INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/http/common \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/provider

