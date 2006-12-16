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
SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem $(BLOCXX_LIBS) \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/requesthandlers/binary -lowrequesthandlerbinary

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

thelibdir = $(pkglibdir)/provifcs

if DARWIN
# This library has undefined symbols that are defined by the CIMOM.  Since
# MacOS defaults to disallowing weak linking, we explicitly tell it to use
# dynamic lookup for undefined symbols.  This library should never be used
# outside of the location these symbols are used anyway (the CIMOM).  This is
# probably not a good thing to do globally.
export MACOSX_DEPLOYMENT_TARGET=10.3
CXXBUNDLELIBLINKFLAGS += -undefined dynamic_lookup
endif

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
OW_OOPProviderInterface.hpp \
OW_OOPProviderInterface.cpp \
OW_OOPClonedProviderEnv.hpp \
OW_OOPClonedProviderEnv.cpp \
OW_OOPFwd.hpp \
OW_OOPDataOStreamBuf.hpp \
OW_OOPDataOStreamBuf.cpp

pkginclude_HEADERS = \
OW_OpenWBEM_OOPProviderRegistration.hpp

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

