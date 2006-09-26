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
CIMOMLocator \
fakeclient \
infiniteClient \
testProviderAgent \
socketCat \
testListener \
testSPNEGO \
testSPNEGOGSSAPI

testProviderAgent_SOURCES = \
testProviderAgent.cpp

testProviderAgent_LDADD = \
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
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

testProviderAgent_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o testProviderAgent$(EXEEXT)



#CIMOMLocator
CIMOMLocator_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o CIMOMLocator$(EXEEXT)

CIMOMLocator_LDADD = \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

CIMOMLocator_SOURCES = \
CIMOMLocator.cpp

#cimlistener
cimlistener_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o cimlistener$(EXEEXT)

cimlistener_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/cimxmllistener -lowhttpxmllistener \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/services/http -lowservicehttp \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

cimlistener_SOURCES = \
cimListenerTest.cpp

#wqlTest1
wqlTest1_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/wql -lowwql \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

wqlTest1_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o wqlTest1$(EXEEXT)

wqlTest1_SOURCES = \
wqlTest1.cpp

#wqlTest2
wqlTest2_LDADD = \
-L$(top_builddir)/src/wql -lowwql \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

wqlTest2_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o wqlTest2$(EXEEXT)

wqlTest2_SOURCES = \
wqlTest2.cpp

#aclTest
aclTest_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

aclTest_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o aclTest$(EXEEXT)

aclTest_SOURCES = \
aclTest.cpp

#aclTestBasic
aclTestBasic_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

aclTestBasic_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o aclTestBasic$(EXEEXT)

aclTestBasic_SOURCES = \
aclTestBasic.cpp

#infiniteClient
infiniteClient_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

infiniteClient_LDFLAGS = \
$(CXXBINLINKFLAGS) \
$(SSL_LIBS) \
-o infiniteClient$(EXEEXT)

infiniteClient_SOURCES = \
InfiniteClient.cpp


#owcimclientcl
owcimclientcl_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

owcimclientcl_LDFLAGS = \
$(CXXBINLINKFLAGS) \
$(SSL_LIBS) \
-o owcimclientcl$(EXEEXT)

owcimclientcl_SOURCES = \
OW_CIMClientCL.cpp

#testOOPAssoc
testOOPAssoc_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

testOOPAssoc_LDFLAGS = \
$(CXXBINLINKFLAGS) \
$(SSL_LIBS) \
-o testOOPAssoc$(EXEEXT)

testOOPAssoc_SOURCES = \
testOOPAssoc.cpp

#testinstprov
testinstprov_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

testinstprov_LDFLAGS = \
$(CXXBINLINKFLAGS) \
$(SSL_LIBS) \
-o testinstprov$(EXEEXT)

testinstprov_SOURCES = \
OW_InstProvTest.cpp

#fakeclient
fakeclient_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

fakeclient_LDFLAGS = \
$(CXXBINLINKFLAGS) \
$(SSL_LIBS) \
-o fakeclient$(EXEEXT)

fakeclient_SOURCES = \
fakeClient.cpp

#testCIMExceptions
testCIMExceptions_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

testCIMExceptions_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o testCIMExceptions$(EXEEXT)

testCIMExceptions_SOURCES = \
testCIMExceptions.cpp

#testNamespaces
testNamespaces_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

testNamespaces_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o testNamespaces$(EXEEXT)

testNamespaces_SOURCES = \
testNamespaces.cpp

#testMisc
testMisc_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

testMisc_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o testMisc$(EXEEXT)

testMisc_SOURCES = \
testMisc.cpp

#hammerClient
hammerClient_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

hammerClient_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o hammerClient$(EXEEXT)

hammerClient_SOURCES = \
hammerClient.cpp

#testSecondaryInstanceProviders
testSecondaryInstanceProviders_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

testSecondaryInstanceProviders_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o testSecondaryInstanceProviders$(EXEEXT)

testSecondaryInstanceProviders_SOURCES = \
testSecondaryInstanceProviders.cpp

#testLargeRequest
testLargeRequest_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

testLargeRequest_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o testLargeRequest$(EXEEXT)

testLargeRequest_SOURCES = \
testLargeRequest.cpp


#testClientTimeout
testClientTimeout_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

testClientTimeout_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o testClientTimeout$(EXEEXT)

testClientTimeout_SOURCES = \
testClientTimeout.cpp

#testEmbeddedCIMOM
testEmbeddedCIMOM_SOURCES = \
testEmbeddedCIMOM.cpp

testEmbeddedCIMOM_LDADD = \
-L$(top_builddir)/src/cimom/embedded -lowembeddedcimom \
-L$(top_builddir)/src/repositories/hdb -lowrepositoryhdb \
-L$(top_builddir)/src/cimom/server -lowserver \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc \
-L$(top_builddir)/src/cimom/common -lowcimomcommon \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
-L$(top_builddir)/db/db -lowdb \
$(REALTIME_LIBS) \
$(SSL_LIBS) $(BINLINKAPPENDFLAGS)

testEmbeddedCIMOM_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o testEmbeddedCIMOM$(EXEEXT)

#socketCat
socketCat_LDADD = \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

socketCat_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o socketCat$(EXEEXT)

socketCat_SOURCES = \
socketCat.cpp

### testListener
testListener_SOURCES = \
testListener.cpp

testListener_LDADD = \
-L$(top_builddir)/src/cimxmllistener -lowhttpxmllistener \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/services/http -lowservicehttp \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

testListener_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o testListener$(EXEEXT)

### testSPNEGO
testSPNEGO_SOURCES = \
testSPNEGO.cpp

testSPNEGO_LDADD = \
-L$(top_builddir)/src/cimxmllistener -lowhttpxmllistener \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/services/http -lowservicehttp \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

testSPNEGO_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o testSPNEGO$(EXEEXT)

### testSPNEGOGSSAPI
testSPNEGOGSSAPI_SOURCES = \
testSPNEGOGSSAPI.cpp

testSPNEGOGSSAPI_LDADD = \
-L$(top_builddir)/src/cimxmllistener -lowhttpxmllistener \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/services/http -lowservicehttp \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

testSPNEGOGSSAPI_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o testSPNEGOGSSAPI$(EXEEXT)

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
-I$(top_srcdir)/src/requesthandlers/cimxml


#

