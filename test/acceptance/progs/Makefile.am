MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

SUBDIRS=

if DISABLE_INSTANCE_MANIPULATION
PROGS=
else
if DISABLE_SCHEMA_MANIPULATION
PROGS=
else
if DISABLE_QUALIFIER_DECLARATION
PROGS=
else
if DISABLE_ASSOCIATION_TRAVERSAL
PROGS=
else
if DISABLE_PROPERTY_OPERATIONS
PROGS=
else
if DISABLE_NAMESPACE_MANIPULATION
PROGS=
else
PROGS= \
owcimclientcl \
testOOPAssoc \
testCIMExceptions \
testinstprov \
aclTest \
aclTestBasic \
cimlistener \
testNamespaces \
testMisc \
hammerClient \
testSecondaryInstanceProviders \
testLargeRequest \
testClientTimeout \
testEmbeddedCIMOM
endif
endif
endif
endif
endif
endif

noinst_PROGRAMS = \
$(PROGS) \
wqlTest1 \
wqlTest2 \
wqlTest3 \
CIMOMLocator \
fakeclient \
infiniteClient \
testProviderAgent \
socketCat \
testListener \
testSPNEGO \
testSPNEGOGSSAPI

noinst_LIBRARIES = \
libtestWQLProv.a

OW_SHARED_LIBS = \
libtestWQLProv.$(LIB_EXT)

testProviderAgent_SOURCES = \
testProviderAgent.cpp

testProviderAgent_PROJECT_LIBS = \
-L$(top_builddir)/src/provideragent -lowprovideragent \
-L$(top_builddir)/src/requesthandlers/cimxml -lowrequesthandlercimxml \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc \
-L$(top_builddir)/src/services/http -lowservicehttp \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/common -lopenwbem $(BLOCXX_LIBS)

testProviderAgent_EXTERNAL_LIBS = \
$(SSL_LIBS) $(Z_LIBS)


#CIMOMLocator
CIMOMLocator_PROJECT_LIBS = \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/common -lopenwbem

CIMOMLocator_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS)

CIMOMLocator_SOURCES = \
CIMOMLocator.cpp

#cimlistener
cimlistener_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/cimxmllistener -lowhttpxmllistener \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/services/http -lowservicehttp \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/common -lopenwbem

cimlistener_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS)

cimlistener_SOURCES = \
cimListenerTest.cpp

#wqlTest1
wqlTest1_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/wql -lowwql \
-L$(top_builddir)/src/common -lopenwbem

wqlTest1_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS)

wqlTest1_SOURCES = \
wqlTest1.cpp

#wqlTest2
wqlTest2_PROJECT_LIBS = \
-L$(top_builddir)/src/wql -lowwql \
-L$(top_builddir)/src/common -lopenwbem

wqlTest2_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS)

wqlTest2_SOURCES = \
wqlTest2.cpp

#wqlTest3
wqlTest3_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/wql -lowwql \
-L$(top_builddir)/src/common -lopenwbem

wqlTest3_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS)

wqlTest3_SOURCES = \
wqlTest3.cpp

#aclTest
aclTest_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem

aclTest_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS)

aclTest_SOURCES = \
aclTest.cpp

#aclTestBasic
aclTestBasic_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem

aclTestBasic_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS)

aclTestBasic_SOURCES = \
aclTestBasic.cpp

#infiniteClient
infiniteClient_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem

infiniteClient_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS)

infiniteClient_SOURCES = \
InfiniteClient.cpp


#owcimclientcl
owcimclientcl_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem

owcimclientcl_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS)

owcimclientcl_SOURCES = \
OW_CIMClientCL.cpp

#testOOPAssoc
testOOPAssoc_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem

testOOPAssoc_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS)

testOOPAssoc_SOURCES = \
testOOPAssoc.cpp

#testinstprov
testinstprov_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem

testinstprov_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS)

testinstprov_SOURCES = \
OW_InstProvTest.cpp

#fakeclient
fakeclient_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem

fakeclient_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS)

fakeclient_SOURCES = \
fakeClient.cpp

#testCIMExceptions
testCIMExceptions_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem

testCIMExceptions_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS)

testCIMExceptions_SOURCES = \
testCIMExceptions.cpp

#testNamespaces
testNamespaces_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem

testNamespaces_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS)

testNamespaces_SOURCES = \
testNamespaces.cpp

#testMisc
testMisc_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem

testMisc_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS)

testMisc_SOURCES = \
testMisc.cpp

#hammerClient
hammerClient_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem

hammerClient_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS)

hammerClient_SOURCES = \
hammerClient.cpp

#testSecondaryInstanceProviders
testSecondaryInstanceProviders_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem

testSecondaryInstanceProviders_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS)

testSecondaryInstanceProviders_SOURCES = \
testSecondaryInstanceProviders.cpp

#testLargeRequest
testLargeRequest_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem

testLargeRequest_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS)

testLargeRequest_SOURCES = \
testLargeRequest.cpp


#testClientTimeout
testClientTimeout_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem

testClientTimeout_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS)

testClientTimeout_SOURCES = \
testClientTimeout.cpp

#testEmbeddedCIMOM
testEmbeddedCIMOM_SOURCES = \
testEmbeddedCIMOM.cpp

testEmbeddedCIMOM_PROJECT_LIBS = \
-L$(top_builddir)/src/cimom/embedded -lowembeddedcimom \
-L$(top_builddir)/src/repositories/hdb -lowrepositoryhdb \
-L$(top_builddir)/src/cimom/server -lowserver \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc \
-L$(top_builddir)/src/cimom/common -lowcimomcommon \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/db/db -lowdb

testEmbeddedCIMOM_EXTERNAL_LIBS = \
$(REALTIME_LIBS) \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(BINLINKAPPENDFLAGS)

#socketCat
socketCat_PROJECT_LIBS = \
-L$(top_builddir)/src/common -lopenwbem

socketCat_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS)

socketCat_SOURCES = \
socketCat.cpp

### testListener
testListener_SOURCES = \
testListener.cpp

testListener_PROJECT_LIBS = \
-L$(top_builddir)/src/cimxmllistener -lowhttpxmllistener \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/services/http -lowservicehttp \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/common -lopenwbem

testListener_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)

### testSPNEGO
testSPNEGO_SOURCES = \
testSPNEGO.cpp

testSPNEGO_PROJECT_LIBS = \
-L$(top_builddir)/src/cimxmllistener -lowhttpxmllistener \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/services/http -lowservicehttp \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/common -lopenwbem

testSPNEGO_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)

### testSPNEGOGSSAPI
testSPNEGOGSSAPI_SOURCES = \
testSPNEGOGSSAPI.cpp

testSPNEGOGSSAPI_PROJECT_LIBS = \
-L$(top_builddir)/src/cimxmllistener -lowhttpxmllistener \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/services/http -lowservicehttp \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/common -lopenwbem

testSPNEGOGSSAPI_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)

### testWQLProv
libtestWQLProv_a_SOURCES = \
testWQLProv.cpp

EXTRA_DIST = \
wqlTestReg.mof.in

BUILT_SOURCES = \
wqlTestReg.mof

CLEANFILES = \
$(BUILT_SOURCES)

include ${top_srcdir}/Makefile-replacement.inc

global_substitutions = \
-e 's!@provdir@!${libdir}/openwbem/oopproviders!g'

libtestWQLProv_PROJECT_LIBS = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc \
-L$(top_builddir)/src/mof -lowmofc

libtestWQLProv_EXTERNAL_LIBS = \
$(BLOCXX_LIBS)

LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

INCLUDES = \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cimom/embedded \
-I$(top_srcdir)/src/cimom/common \
-I$(top_srcdir)/src/cimom/server \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/http/common \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/cimxmllistener \
-I$(top_srcdir)/src/wql \
-I$(top_srcdir)/src/provideragent \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/services/http \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/monitor \
-I$(top_srcdir)/src/requesthandlers/cimxml \
-I$(top_srcdir)/src/mof

include $(top_srcdir)/Makefile-executable.inc
