if DISABLE_INSTANCE_MANIPULATION
MOFDIR=
else
if DISABLE_SCHEMA_MANIPULATION
MOFDIR=
else
if DISABLE_QUALIFIER_DECLARATION
MOFDIR=
else
MOFDIR= mof
endif
endif
endif

if DISABLE_INSTANCE_MANIPULATION
CIMXMLLISTENERDIR=
else
CIMXMLLISTENERDIR= cimxmllistener
endif

SUBDIRS = \
cim \
ifcs \
common \
 \
http \
 \
xml \
 \
client \
 \
services \
$(CIMXMLLISTENERDIR) \
 \
 \
provider \
 \
wql \
authenticators \
authorizers \
requesthandlers \
repositories \
providerifcs \
cimom \
providers \
 \
$(MOFDIR) \
tools

# Everything below here belongs to the creation of a static cimom.

install-exec-local : install-ow-static-lib install-ow-static-cimomd

STATIC_CIMOM_FILES= \
$(top_builddir)/src/common/libopenwbem.a \
$(top_builddir)/src/provider/libowprovider.a \
$(top_builddir)/src/cimom/common/libowcimomcommon.a \
$(top_builddir)/src/cimom/server/libowserver.a \
$(top_builddir)/src/provider/libowprovider.a \
$(top_builddir)/src/repositories/hdb/libowrepositoryhdb.a \
$(top_builddir)/src/providerifcs/cpp/libowcppprovifc.a \
$(top_builddir)/src/wql/common/libowwqlcommon.a \
$(top_builddir)/src/wql/libowwql.a \
$(top_builddir)/src/xml/libowxml.a \
$(top_builddir)/db/db/libowdb.a

STATIC_CLIENT_FILES= \
$(top_builddir)/src/client/libowclient.a \
$(top_builddir)/src/mof/libowmofc.a

STATIC_SERVICE_FILES= \
$(top_builddir)/src/authorizers/simple/libowsimpleauthorizer.a \
$(top_builddir)/src/authenticators/pam/libpamauthentication.a \
$(top_builddir)/src/authenticators/pamcl/libpamclauthentication.a \
$(top_builddir)/src/authenticators/simple/libsimpleauthentication.a \
\
$(top_builddir)/src/cimom/indication/libowindicationserver.a \
$(top_builddir)/src/cimom/indication/libowindicationreplayer.a \
$(top_builddir)/src/providers/cpp/indication/IndicationRepLayer/libowprovindIndicationRepLayer.a \
\
$(top_builddir)/src/cimxmllistener/libowhttpxmllistener.a \
\
$(top_builddir)/src/http/client/libowhttpclient.a \
$(top_builddir)/src/http/common/libowhttpcommon.a \
$(top_builddir)/src/services/http/libowservicehttp.a \
\
\
$(top_builddir)/src/requesthandlers/cimxml/libowrequesthandlercimxml.a \
\
$(top_builddir)/src/requesthandlers/binary/libowrequesthandlerbinary.a 

STATIC_IFC_FILES= \
$(top_builddir)/src/providerifcs/cpp/libowcppprovifc.a \
$(top_builddir)/src/providerifcs/cmpi/common/libcmpisupport.a \
$(top_builddir)/src/providerifcs/cmpi/libcmpiprovifc.a \
$(top_builddir)/src/providerifcs/npi/common/libnpisupport.a


STATIC_PROVIDER_FILES= \
$(top_builddir)/src/providers/cpp/indication/IndicationRepLayer/libowprovindIndicationRepLayer.a \
$(top_builddir)/src/providers/cpp/indicationexport/libcppindicationexportxmlhttpprovider.a \
$(top_builddir)/src/providers/cpp/instance/CIM_Namespace/libowprovinstCIM_Namespace.a \
$(top_builddir)/src/providers/cpp/instance/CIM_IndicationFilter/libowprovinstCIM_IndicationFilter.a \
$(top_builddir)/src/providers/cpp/instance/CIM_IndicationSubscription/libowprovCIM_IndicationSubscription.a \
$(top_builddir)/src/providers/cpp/instance/CIM_NamespaceInManager/libowprovinstCIM_NamespaceInManager.a \
$(top_builddir)/src/providers/cpp/instance/CIM_ObjectManagerCommunicationMechanism/libowprovinstCIM_ObjectManagerCommunicationMechanism.a \
$(top_builddir)/src/providers/cpp/instance/OW_NameSpace/libowprovinstOW_NameSpace.a \
$(top_builddir)/src/providers/cpp/instance/OpenWBEM_ObjectManager/libowprovinstOpenWBEM_ObjectManager.a \
$(top_builddir)/src/providers/cpp/polled/unloader/libowprovpollUnloader.a 

STATIC_SERVICE_FLAGS= \
-lpam $(Z_LIBS) $(DYNAMIC_LINK_FLAGS) $(DL_LIB) $(CURRENT_LINK_MODE_FLAGS)

# In most cases, this library will be ignored.  However, when static services
# are used, it will be required! 
OPENWBEM_STATIC_ALL=libopenwbem_all.a

# We only want to link against .a libraries, so use all of the dynamic
# libraries as names to be replaced. 
EXTERNAL_PROVIDER_FILES=$(filter %.a,$(sort $(EXTERNAL_PROVIDERS:%.so=%.a) $(EXTERNAL_PROVIDERS:%.dylib=%.a) $(EXTERNAL_PROVIDERS:%.sl=%.a)))

$(OPENWBEM_STATIC_ALL): $(STATIC_CIMOM_FILES) $(STATIC_SERVICE_FILES) $(STATIC_PROVIDER_FILES) $(STATIC_CLIENT_FILES) $(STATIC_IFC_FILES) $(EXTERNAL_PROVIDER_FILES)
	if test "x$(STATIC_SERVICES)" != "x"; then \
		echo "External providers: $(EXTERNAL_PROVIDER_FILES)"; \
		$(AR) cru $@ $^; \
	else \
		touch $@; \
	fi

install-ow-static-lib: $(OPENWBEM_STATIC_ALL)
	if test "x$(STATIC_SERVICES)" != "x"; then \
		$(INSTALL) $^ $(DESTDIR)$(libdir); \
	fi

TEMPOBJDIR=$@.libs
CIMOMD_MAIN=$(top_builddir)/src/cimom/libowcimomd_main.a
install-ow-static-cimomd: $(OPENWBEM_STATIC_ALL) $(CIMOMD_MAIN)
	if test "x$(STATIC_SERVICES)" != "x"; then \
		rm -rf $(TEMPOBJDIR); \
		mkdir $(TEMPOBJDIR); \
		cd $(TEMPOBJDIR) && ar x ../$(OPENWBEM_STATIC_ALL); \
		cd ..; \
		$(CXX) $(CXXBINLINKFLAGS) -o static_cimomd $(TEMPOBJDIR)/*.o $(CIMOMD_MAIN) $(SSL_LIBS) $(DYNAMIC_EXPORT_FLAG) $(STATIC_SERVICE_FLAGS) $(BINLINKAPPENDFLAGS); \
		rm -rf $(TEMPOBJDIR); \
		echo 'REPLACING THE DYNAMIC CIMOM WITH A STATIC VERSION!'; \
		$(INSTALL_PROGRAM) static_cimomd $(DESTDIR)$(sbindir)/owcimomd; \
	fi
