SUBDIRS = indication

cimombin_PROGRAMS = owcimomd
cimombindir = $(sbindir)

owcimomd_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-rdynamic -o owcimomd

owcimomd_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/server -lowserver \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc \
-ldl \
-L$(top_builddir)/src/repositories/tdbcr -lowrepositorytdbcr \
-L$(top_builddir)/src/provider -lowprovider

# OpenServer may need this one day, but Linux doesn't like it
#owcimomd_LDFLAGS = "-Wl,-Bexport"

owcimomd_SOURCES = \
owcimomd_main.cpp \
OW_CIMOMEnvironment.cpp \
OW_CIMOMEnvironment.hpp \
OW_CIMServer.cpp \
OW_CIMServer.hpp \
OW_IndicationRepLayerMediator.cpp \
OW_IndicationRepLayerMediator.hpp \
OW_LocalCIMOMHandle.cpp \
OW_LocalCIMOMHandle.hpp \
OW_PollingManager.cpp \
OW_PollingManager.hpp \
OW_ProviderIFCLoader.cpp \
OW_ProviderIFCLoader.hpp \
OW_ProviderManager.cpp \
OW_ProviderManager.hpp \
OW_SharedLibraryRepository.cpp \
OW_SharedLibraryRepository.hpp \
OW_WQLFilterRep.cpp \
OW_WQLFilterRep.hpp

INCLUDES = \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cimom/indication \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/server \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/repositories/tdbcr

#use this rule to create the data dir
install-exec-local:
	${INSTALL} -d ${DESTDIR}/@localstatedir@/openwbem

