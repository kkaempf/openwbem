SUBDIRS = indication

cimombin_PROGRAMS = owcimomd
cimombindir = $(sbindir)
 
owcimomd_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owcimomd

owcimomd_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/server -lowserver \
-L$(top_builddir)/db/db -lowdb -ldl

# OpenServer may need this one day, but Linux doesn't like it
#owcimomd_LDFLAGS = "-Wl,-Bexport"

owcimomd_SOURCES = \
main.cpp \
OW_CIMOMEnvironment.cpp \
OW_CIMServer.cpp \
OW_GenericHDBRepository.cpp \
OW_IndexImpl.cpp \
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
OW_HDBNode.cpp

pkginclude_HEADERS = \
OW_ACLInfo.hpp \
OW_CIMOMEnvironment.hpp \
OW_CIMServer.hpp \
OW_GenericHDBRepository.hpp \
OW_Index.hpp \
OW_IndicationRepLayer.hpp \
OW_IndicationServer.hpp \
OW_InstanceRepository.hpp \
OW_LocalCIMOMHandle.hpp \
OW_MetaRepository.hpp \
OW_NameSpaceProvider.hpp \
OW_PollingManager.hpp \
OW_ProviderIFCLoader.hpp \
OW_ProviderManager.hpp \
OW_RepositoryIFC.hpp \
OW_RepositoryStreams.hpp \
OW_WQLFilterRep.hpp \
OW_AssocDb.hpp \
OW_ProxyProvider.hpp \
OW_HDB.hpp \
OW_HDBCommon.hpp \
OW_HDBNode.hpp

INCLUDES = \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/server \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/db/include

#use this rule to create the data dir
install-exec-local:
	${INSTALL} -d ${DESTDIR}/@localstatedir@/openwbem

