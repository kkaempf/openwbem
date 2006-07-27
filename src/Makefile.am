if DISABLE_INSTANCE_MANIPULATION
MOFDIR=
else
if DISABLE_SCHEMA_MANIPULATION
MOFDIR=
else
if DISABLE_QUALIFIER_DECLARATION
MOFDIR=
else
if DISABLE_NAMESPACE_MANIPULATION
MOFDIR=
else
MOFDIR= mof
endif
endif
endif
endif

if DISABLE_INSTANCE_MANIPULATION
CIMXMLLISTENERDIR=
CIMXMLLISTENER_FILES=
else
CIMXMLLISTENERDIR= cimxmllistener
CIMXMLLISTENER_FILES=$(top_builddir)/src/cimxmllistener/libowhttpxmllistener.a
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
monitor \
 \
services \
$(CIMXMLLISTENERDIR) \
 \
provider \
 \
provideragent \
 \
wql \
authenticators \
authorizers \
requesthandlers \
providerifcs \
oopcpp1pr \
repositories \
cimom \
providers \
$(MOFDIR) \
tools

# Everything below here belongs to the creation of a static cimom.
if OW_STATIC_LINKING

install-exec-local : install-ow-static-cimomd install-static-owoopcpp1pr install-ow-static-lib install-static-owoopcpp1pr-lib

STATIC_CIMOM_FILES= \
$(top_builddir)/src/common/libopenwbem.a \
$(top_builddir)/src/cimom/common/libowcimomcommon.a \
$(top_builddir)/src/cimom/server/libowserver.a \
$(top_builddir)/src/provider/libowprovider.a \
$(top_builddir)/src/repositories/hdb/libowrepositoryhdb.a \
$(top_builddir)/src/providerifcs/cpp/libowcppprovifc.a \
$(top_builddir)/src/wql/libowwql.a \
$(top_builddir)/src/xml/libowxml.a \
$(top_builddir)/src/monitor/libowprivman.a \
$(top_builddir)/db/db/libowdb.a

STATIC_OOP_FILES= \
$(top_builddir)/src/common/libopenwbem.a \
$(top_builddir)/src/provider/libowprovider.a \
$(top_builddir)/src/client/libowclient.a \
$(top_builddir)/src/xml/libowxml.a \
$(top_builddir)/src/http/client/libowhttpclient.a \
$(top_builddir)/src/http/common/libowhttpcommon.a \
$(top_builddir)/src/monitor/libowprivman.a \
$(top_builddir)/src/requesthandlers/binary/libowrequesthandlerbinary.a \
$(top_builddir)/src/providerifcs/cpp/libowcppprovifc.a \
$(top_builddir)/src/oopcpp1pr/libowoopcpp1pr.a

STATIC_CLIENT_FILES= \
$(top_builddir)/src/client/libowclient.a \
$(top_builddir)/src/mof/libowmofc.a

if USE_PAM
PAM_FILES= \
$(top_builddir)/src/authenticators/pam/libpamauthentication.a \
$(top_builddir)/src/authenticators/pamcl/libpamclauthentication.a
endif

if USE_AIX_AUTHENTICATE
AIX_AUTH_FILES=$(top_builddir)/src/authenticators/aix/libaixauthentication.a
endif

STATIC_SERVICE_FILES= \
$(top_builddir)/src/authorizers/simple/libowsimpleauthorizer.a \
$(top_builddir)/src/authorizers/simple2/libowsimpleauthorizer2.a \
$(top_builddir)/src/authorizers/basic/libowbasicauthorizer.a \
$(PAM_FILES) \
$(AIX_AUTH_FILES) \
$(top_builddir)/src/authenticators/simple/libsimpleauthentication.a \
\
$(top_builddir)/src/cimom/indication/libowindicationserver.a \
$(top_builddir)/src/cimom/indication/libowindicationreplayer.a \
$(top_builddir)/src/providers/cpp/indication/IndicationRepLayer/libowprovindIndicationRepLayer.a \
\
$(CIMXMLLISTENER_FILES) \
\
$(top_builddir)/src/http/client/libowhttpclient.a \
$(top_builddir)/src/http/common/libowhttpcommon.a \
$(top_builddir)/src/services/http/libowservicehttp.a \
\
\
$(top_builddir)/src/requesthandlers/cimxml/libowrequesthandlercimxml.a \
\
$(top_builddir)/src/requesthandlers/binary/libowrequesthandlerbinary.a 

if ENABLE_CMPI
STATIC_CMPI_IFC_FILES= \
$(top_builddir)/src/providerifcs/cmpi/common/libcmpisupport.a \
$(top_builddir)/src/providerifcs/cmpi/libcmpiprovifc.a \
$(top_builddir)/src/providerifcs/npi/common/libnpisupport.a
else
STATIC_CMPI_IFC_FILES=
endif

STATIC_IFC_FILES= \
$(STATIC_CMPI_IFC_FILES) \
$(top_builddir)/src/providerifcs/cpp/libowcppprovifc.a \
$(top_builddir)/src/providerifcs/oop/libowoopprovifc.a


STATIC_PROVIDER_FILES= \
$(top_builddir)/src/providers/cpp/indication/IndicationRepLayer/libowprovindIndicationRepLayer.a \
$(top_builddir)/src/providers/cpp/indicationexport/libcppindicationexportxmlhttpprovider.a \
$(top_builddir)/src/providers/cpp/instance/CIM_Namespace/libowprovinstCIM_Namespace.a \
$(top_builddir)/src/providers/cpp/instance/CIM_IndicationFilter/libowprovinstCIM_IndicationFilter.a \
$(top_builddir)/src/providers/cpp/instance/CIM_IndicationSubscription/libowprovCIM_IndicationSubscription.a \
$(top_builddir)/src/providers/cpp/instance/CIM_NamespaceInManager/libowprovinstCIM_NamespaceInManager.a \
$(top_builddir)/src/providers/cpp/instance/OW_NameSpace/libowprovinstOW_NameSpace.a \
$(top_builddir)/src/providers/cpp/instance/OpenWBEM_ObjectManager/libowprovinstOpenWBEM_ObjectManager.a \
$(top_builddir)/src/providers/cpp/instance/OpenWBEM_UnitaryComputerSystem/libowprovinstOpenWBEM_UnitaryComputerSystem.a \
$(top_builddir)/src/providers/cpp/instance/OpenWBEM_ConfigSettingData/libowprovinstOpenWBEM_ConfigSettingData.a \
$(top_builddir)/src/providers/cpp/polled/unloader/libowprovpollUnloader.a

STATIC_SERVICE_FLAGS= \
$(PAM_LIB) $(Z_LIBS) $(DYNAMIC_LINK_FLAGS) $(DL_LIB) $(CURRENT_LINK_MODE_FLAGS)

# In most cases, this library will be ignored.  However, when static services
# are used, it will be required! 
OPENWBEM_STATIC_ALL=libopenwbem_all.a
OOP_STATIC_ALL=liboopcpp1pr_all.a

# We only want to link against .a libraries, so use all of the dynamic
# libraries as names to be replaced. 
EXTERNAL_PROVIDER_FILES=$(filter %.a,$(sort $(EXTERNAL_PROVIDERS:%.so=%.a) $(EXTERNAL_PROVIDERS:%.dylib=%.a) $(EXTERNAL_PROVIDERS:%.sl=%.a)))
OOP_EXTERNAL_PROVIDER_FILES=$(filter %.a,$(sort $(OOP_EXTERNAL_PROVIDERS:%.so=%.a) $(OOP_EXTERNAL_PROVIDERS:%.dylib=%.a) $(OOP_EXTERNAL_PROVIDERS:%.sl=%.a)))

$(OPENWBEM_STATIC_ALL): $(STATIC_CIMOM_FILES) $(STATIC_SERVICE_FILES) $(STATIC_PROVIDER_FILES) $(STATIC_CLIENT_FILES) $(STATIC_IFC_FILES) $(EXTERNAL_PROVIDER_FILES)
	@echo "External providers: $(EXTERNAL_PROVIDER_FILES)"
	$(AR) cru $@ $^
	if [ "x$(EXTERNAL_PROVIDER_FILES)" != "x" ]; then \
		for i in $(EXTERNAL_PROVIDER_FILES) ""; do \
			$(SHELL) $(top_srcdir)/fake_library_creator.sh $(EXPORT_PREFIX) $$i; \
			LIB_BASE=`echo $$i | cut -f1 -d'.'`; \
			FSL="$$LIB_BASE.fsl"; \
			if [ -e "$$FSL" ]; then \
				echo "Installing fake library for $$i"; \
				$(INSTALL_PROGRAM) $FSL $(DESTDIR)$(thelibdir)/$$LIB_BASE.so; \
				rm -f "$$FSL"; \
			else \
				echo "Not installing $$i because no exports were found."; \
			fi; \
		done; \
	fi

$(OOP_STATIC_ALL): $(STATIC_OOP_FILES) $(OOP_EXTERNAL_PROVIDER_FILES)
	echo "OOP External providers: $(OOP_EXTERNAL_PROVIDER_FILES)"; \
	$(AR) cru $@ $^; \
	if [ "x$(OOP_EXTERNAL_PROVIDER_FILES)" != "x" ]; then \
		for i in $(OOP_EXTERNAL_PROVIDER_FILES) ""; do \
			$(SHELL) $(top_srcdir)/fake_library_creator.sh $(EXPORT_PREFIX) $$i; \
			LIB_BASE=`echo $$i | cut -f1 -d'.'`; \
			FSL="$$LIB_BASE.fsl"; \
			if [ -e "$$FSL" ]; then \
				echo "Installing fake library for $$i"; \
				$(INSTALL_PROGRAM) $FSL $(DESTDIR)$(thelibdir)/$$LIB_BASE.so; \
				rm -f "$$FSL"; \
			else \
				echo "Not installing $$i because no exports were found."; \
			fi; \
		done; \
	fi

install-ow-static-lib: $(OPENWBEM_STATIC_ALL)
	$(INSTALL) $^ $(DESTDIR)$(libdir)

install-static-owoopcpp1pr-lib: $(OOP_STATIC_ALL)
	$(INSTALL) $^ $(DESTDIR)$(libdir)

TEMPOBJDIR=.libOWall
CIMOMD_MAIN=$(top_builddir)/src/cimom/libowcimomd_main.a
OWOOPCPP1PR_MAIN=$(top_builddir)/src/oopcpp1pr/bin/libowoopcpp1pr_main.a

if OW_NETWARE_INSTALL
install-ow-static-cimomd: $(OPENWBEM_STATIC_ALL) owcimomd.def
	rm -rf $(TEMPOBJDIR); \
	mkdir $(TEMPOBJDIR); \
	cd $(TEMPOBJDIR) && ar x ../$(OPENWBEM_STATIC_ALL); \
	cd ..; \
	nm -Bpg $(TEMPOBJDIR)/*.o | \
			$(AWK) '{ if((($$2 == "T") || \
				   ($$2 == "D") || \
				   ($$2 == "B")) && \
				  (length($$3) < 255) && \
				  (substr($$3,1,1) != ".")) \
				{ print $$3 } }' | \
			sort -u | $(SED) '$$!s/$$/,/' > owcimomd.exp; \
	$(CXX) $(CXXBINLINKFLAGS) -o owcimomd.nlm -Wl,--def-file,owcimomd.def $(TEMPOBJDIR)/*.o $(CIMOMD_MAIN) $(LIBS) $(SSL_LIBS) $(DYNAMIC_EXPORT_FLAG) $(STATIC_SERVICE_FLAGS) $(BINLINKAPPENDFLAGS); \
	rm -rf $(TEMPOBJDIR); \
	echo 'REPLACING THE DYNAMIC CIMOM WITH A STATIC VERSION!'; \
	$(INSTALL_PROGRAM) owcimomd.nlm $(DESTDIR)$(sbindir)/owcimomd.nlm;

owcimomd.def:
	echo 'description "OpenWBEM CIMOM Daemon"' >$@
	echo 'copyright "Copyright (C) 2000-2004 Vintela, Inc. All rights reserved"' >>$@
	echo 'version $(OPENWBEM_MAJOR_VERSION),$(OPENWBEM_MINOR_VERSION),$(OPENWBEM_MICRO_VERSION)' >>$@
	echo 'stacksize 1065536' >>$@
	echo 'Flag_On 0x00400200' >>$@
	echo 'export @owcimomd.exp' >> $@

clean-local:
	rm -f owcimomd.* $(OPENWBEM_STATIC_ALL)
else
install-ow-static-cimomd: clean-local $(OPENWBEM_STATIC_ALL)
	rm -rf $(TEMPOBJDIR)
	mkdir $(TEMPOBJDIR)
	cd $(TEMPOBJDIR) && ar x ../$(OPENWBEM_STATIC_ALL) && cd ..
	$(CXX) $(CXXBINLINKFLAGS) -o static_cimomd $(TEMPOBJDIR)/*.o $(CIMOMD_MAIN) $(SSL_LIBS) $(DYNAMIC_EXPORT_FLAG) $(STATIC_SERVICE_FLAGS) $(BINLINKAPPENDFLAGS)
	rm -rf $(TEMPOBJDIR)
	@echo 'REPLACING THE DYNAMIC CIMOM WITH A STATIC VERSION!'
	$(INSTALL_PROGRAM) static_cimomd $(DESTDIR)$(sbindir)/owcimomd

install-static-owoopcpp1pr: clean-local $(OOP_STATIC_ALL)
	rm -rf $(TEMPOBJDIR)
	mkdir $(TEMPOBJDIR)
	cd $(TEMPOBJDIR) && ar x ../$(OOP_STATIC_ALL) && cd ..
	$(CXX) $(CXXBINLINKFLAGS) -o static_owoopcpp1pr $(TEMPOBJDIR)/*.o $(OWOOPCPP1PR_MAIN) $(SSL_LIBS) $(DYNAMIC_EXPORT_FLAG) $(STATIC_SERVICE_FLAGS) $(BINLINKAPPENDFLAGS)
	rm -rf $(TEMPOBJDIR)
	@echo 'REPLACING THE DYNAMIC owoopcpp1pr WITH A STATIC VERSION!'
	$(INSTALL_PROGRAM) static_owoopcpp1pr $(DESTDIR)$(libexecdir)/openwbem/owoopcpp1pr

clean-local:
	@echo "Clean-local executed."
	rm -f static_cimomd $(OPENWBEM_STATIC_ALL) static_owoopcpp1pr $(OOP_STATIC_ALL)

endif

else

# need to have a version of this so 'make clean' doesn't puke.
clean-local:

endif

