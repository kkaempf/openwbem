SUBDIRS = indication

cimombin_PROGRAMS = owcimomd
cimombindir = $(sbindir)

owcimomd_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-rdynamic -o owcimomd

owcimomd_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/server -lowserver \
-L$(top_builddir)/db/db -lowdb \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc \
-ldl

# OpenServer may need this one day, but Linux doesn't like it
#owcimomd_LDFLAGS = "-Wl,-Bexport"

owcimomd_SOURCES = \
owcimomd_main.cpp \
OW_CIMOMEnvironment.cpp \
OW_CIMServer.cpp \
OW_GenericHDBRepository.cpp \
OW_IndexImpl.cpp \
OW_IndicationRepLayerMediator.cpp \
OW_InstanceRepository.cpp \
OW_LocalCIMOMHandle.cpp \
OW_MetaRepository.cpp \
OW_NameSpaceProvider.cpp \
OW_PollingManager.cpp \
OW_ProviderIFCLoader.cpp \
OW_ProviderManager.cpp \
OW_RepositoryStreams.cpp \
OW_WQLFilterRep.cpp \
OW_AssocDb.cpp \
OW_HDB.cpp \
OW_ProxyProvider.cpp \
OW_HDBNode.cpp \
OW_CIMOMEnvironment.hpp \
OW_CIMServer.hpp \
OW_GenericHDBRepository.hpp \
OW_Index.hpp \
OW_IndicationRepLayerMediator.hpp \
OW_InstanceRepository.hpp \
OW_LocalCIMOMHandle.hpp \
OW_MetaRepository.hpp \
OW_NameSpaceProvider.hpp \
OW_PollingManager.hpp \
OW_ProviderIFCLoader.hpp \
OW_ProviderManager.hpp \
OW_UnloaderProvider.hpp \
OW_UnloaderProvider.cpp \
OW_SharedLibraryRepository.hpp \
OW_RepositoryStreams.hpp \
OW_WQLFilterRep.hpp \
OW_AssocDb.hpp \
OW_ProxyProvider.hpp \
OW_HDB.hpp \
OW_HDBCommon.hpp \
OW_HDBNode.hpp \
OW_InternalProviderIFC.cpp \
OW_InternalProviderIFC.hpp \
OW_CIMRepository.hpp \
OW_CIMRepository.cpp

INCLUDES = \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cimom/indication \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/server \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/db/include

#use this rule to create the data dir
install-exec-local:
	${INSTALL} -d ${DESTDIR}/@localstatedir@/openwbem

