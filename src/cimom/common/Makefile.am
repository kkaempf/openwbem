include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


noinst_LIBRARIES = libowcimomcommon.a
SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/cimom/server -lowserver \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/repositories/hdb -lowrepositoryhdb \
-L$(top_builddir)/db/db -lowdb \
-ldl \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc


INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/cimom/server \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/repositories/hdb \
-I$(top_srcdir)/src/providerifcs/cpp



OW_SHARED_LIBS = libowcimomcommon.so
thelibdir = $(libdir)



libowcimomcommon_a_SOURCES =	\
OW_CIMOMEnvironment.cpp \
OW_CIMOMEnvironment.hpp \
OW_CIMServer.cpp \
OW_CIMServer.hpp \
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
OW_IndicationRepLayerMediator.cpp \
OW_IndicationRepLayerMediator.hpp \
OW_ProviderManager.cpp \
OW_ProviderManager.hpp