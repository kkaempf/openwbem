MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs

EXPORT_PREFIX=createProviderIFC


noinst_LIBRARIES = libowoopprovifc.a
OW_SHARED_LIBS = libowoopprovifc.$(LIB_EXT)

libowoopprovifc_PROJECT_LIBS = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/requesthandlers/binary -lowrequesthandlerbinary

libowoopprovifc_EXTERNAL_LIBS = \
$(BLOCXX_LIBS)

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

thelibdir = $(pkglibdir)/provifcs

if DARWIN
if ARCH_PPC
# This library has undefined symbols that are defined by the CIMOM.  Since
# MacOS defaults to disallowing weak linking, we explicitly tell it to use
# dynamic lookup for undefined symbols.  This library should never be used
# outside of the location these symbols are used anyway (the CIMOM).  This is
# probably not a good thing to do globally.
#
# TODO: Find out what, if anything, needs to be done to make the perl modules work in MacOS x86 and x86_64.
export MACOSX_DEPLOYMENT_TARGET=10.3
CXXBUNDLELIBLINKFLAGS += -undefined dynamic_lookup
endif #ARCH_PPC
endif #DARWIN

libowoopprovifc_a_SOURCES = \
OW_OOPProtocolCPP1.hpp \
OW_OOPProtocolCPP1.cpp \
OW_OOPProtocolIFC.hpp \
OW_OOPProtocolIFC.cpp \
OW_OOPProviderBase.hpp \
OW_OOPProviderBase.cpp \
OW_OOPAssociatorProvider.hpp \
OW_OOPAssociatorProvider.cpp \
OW_OOPInstanceProvider.hpp \
OW_OOPInstanceProvider.cpp \
OW_OOPSecondaryInstanceProvider.hpp \
OW_OOPSecondaryInstanceProvider.cpp \
OW_OOPMethodProvider.hpp \
OW_OOPMethodProvider.cpp \
OW_OOPIndicationProvider.hpp \
OW_OOPIndicationProvider.cpp \
OW_OOPPolledProvider.hpp \
OW_OOPPolledProvider.cpp \
OW_OOPIndicationExportProvider.hpp \
OW_OOPIndicationExportProvider.cpp \
OW_OOPQueryProvider.hpp \
OW_OOPQueryProvider.cpp \
OW_OOPProviderInterface.hpp \
OW_OOPProviderInterface.cpp \
OW_OOPShuttingDownCallback.hpp \
OW_OOPShuttingDownCallback.cpp \
OW_OOPClonedProviderEnv.hpp \
OW_OOPClonedProviderEnv.cpp \
OW_OOPFwd.hpp \
OW_OOPDataOStreamBuf.hpp \
OW_OOPDataOStreamBuf.cpp \
OW_OOPProcessState.hpp \
OW_OOPCallbackServiceEnv.hpp \
OW_OOPCallbackServiceEnv.cpp \
OW_OOPCIMOMHandleConnectionRunner.hpp \
OW_OOPCIMOMHandleConnectionRunner.cpp \
OW_OOPProtocolCPP1Impl.hpp \
OW_OOPProtocolCPP1Impl.cpp

pkginclude_HEADERS = \
OW_OpenWBEM_OOPProviderRegistration.hpp \
OW_OpenWBEM_OOPAlertIndicationProviderCapabilities.hpp \
OW_OpenWBEM_OOPAssociationProviderCapabilities.hpp \
OW_OpenWBEM_OOPIndicationExportProviderCapabilities.hpp \
OW_OpenWBEM_OOPInstanceProviderCapabilities.hpp \
OW_OpenWBEM_OOPLifecycleIndicationProviderCapabilities.hpp \
OW_OpenWBEM_OOPMethodProviderCapabilities.hpp \
OW_OpenWBEM_OOPPolledProviderCapabilities.hpp \
OW_OpenWBEM_OOPProviderCapabilities.hpp \
OW_OpenWBEM_OOPProviderProcess.hpp \
OW_OpenWBEM_OOPProviderProcessCapabilities.hpp \
OW_OpenWBEM_OOPQueryProviderCapabilities.hpp \
OW_OpenWBEM_OOPSecondaryInstanceProviderCapabilities.hpp


#OW_OOPAssociatorProvider.hpp \
#OW_OOPAssociatorProvider.cpp \
#OW_OOPMethodProvider.hpp \
#OW_OOPMethodProvider.cpp \
#OW_OOPSecondaryInstanceProvider.hpp \
#OW_OOPSecondaryInstanceProvider.cpp \
#OW_OOPProviderUtils.hpp \
#OW_OOPProviderUtils.cpp

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
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/requesthandlers/binary \
-I$(top_srcdir)/src/monitor \
-I$(top_srcdir)/src/cimom/common

