MAKEDYLIB=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


owstatlib_LIBRARIES = libowcimomcommon.a

libowcimomcommon_PROJECT_LIBS = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/cimom/server -lowserver \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/repositories/hdb -lowrepositoryhdb \
-L$(top_builddir)/db/db -lowdb \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc

libowcimomcommon_EXTERNAL_LIBS = \
$(BLOCXX_LIBS)

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/cimom/server \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/repositories/hdb \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/monitor

OW_SHARED_LIBS = libowcimomcommon.$(LIB_EXT)
# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)
thelibdir = $(libdir)
owstatlibdir = $(thelibdir)

libowcimomcommon_a_SOURCES =	\
OW_CIMOMEnvironment.cpp \
OW_CIMOMEnvironment.hpp \
OW_CIMServer.cpp \
OW_CIMServer.hpp \
OW_CIMServerProviderEnvironment.cpp \
OW_CIMServerProviderEnvironment.hpp \
OW_LocalCIMOMHandle.cpp \
OW_LocalCIMOMHandle.hpp \
OW_PollingManager.cpp \
OW_PollingManager.hpp \
OW_ProviderIFCLoader.cpp \
OW_ProviderIFCLoader.hpp \
OW_SharedLibraryRepository.cpp \
OW_SharedLibraryRepository.hpp \
OW_WQLFilterRep.cpp \
OW_WQLFilterRep.hpp \
OW_IndicationRepLayer.hpp \
OW_IndicationRepLayerMediator.cpp \
OW_IndicationRepLayerMediator.hpp \
OW_IndicationServer.cpp \
OW_IndicationServer.hpp \
OW_AuthorizerManager.cpp \
OW_AuthorizerManager.hpp \
OW_ProviderManager.cpp \
OW_ProviderManager.hpp

pkginclude_HEADERS = \
OW_CimomCommonFwd.hpp

