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
PROGS= \
owcimclientcl \
testCIMExceptions \
testinstprov \
aclTest \
cimlistener \
testNamespaces \
testMisc \
hammerClient \
testSecondaryInstanceProviders \
testLargeRequest
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
infiniteClient



#CIMOMLocator
CIMOMLocator_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o CIMOMLocator

CIMOMLocator_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient

CIMOMLocator_SOURCES = \
CIMOMLocator.cpp

#cimlistener
cimlistener_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o cimlistener

cimlistener_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/services/http -lowservicehttp \
-L$(top_builddir)/src/cimxmllistener -lowhttpxmllistener \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient

cimlistener_SOURCES = \
cimListenerTest.cpp

#wqlTest1
wqlTest1_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/wql/common -lowwqlcommon \
-L$(top_builddir)/src/wql -lowwql

wqlTest1_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o wqlTest1

wqlTest1_SOURCES = \
wqlTest1.cpp

#wqlTest2
wqlTest2_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/wql/common -lowwqlcommon \
-L$(top_builddir)/src/wql -lowwql

wqlTest2_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o wqlTest2

wqlTest2_SOURCES = \
wqlTest2.cpp

#aclTest
aclTest_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient

aclTest_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o aclTest

aclTest_SOURCES = \
aclTest.cpp

#infiniteClient
infiniteClient_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/xml -lowxml

infiniteClient_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o infiniteClient

infiniteClient_SOURCES = \
InfiniteClient.cpp


#owcimclientcl
owcimclientcl_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/xml -lowxml

owcimclientcl_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owcimclientcl

owcimclientcl_SOURCES = \
OW_CIMClientCL.cpp

#testinstprov
testinstprov_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/xml -lowxml

testinstprov_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o testinstprov

testinstprov_SOURCES = \
OW_InstProvTest.cpp

#fakeclient
fakeclient_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/common -lopenwbem

fakeclient_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o fakeclient

fakeclient_SOURCES = \
fakeClient.cpp

#testCIMExceptions
testCIMExceptions_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/xml -lowxml

testCIMExceptions_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o testCIMExceptions

testCIMExceptions_SOURCES = \
testCIMExceptions.cpp

#testNamespaces
testNamespaces_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/xml -lowxml

testNamespaces_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o testNamespaces

testNamespaces_SOURCES = \
testNamespaces.cpp

#testMisc
testMisc_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/xml -lowxml

testMisc_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o testMisc

testMisc_SOURCES = \
testMisc.cpp

#hammerClient
hammerClient_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/xml -lowxml

hammerClient_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o hammerClient

hammerClient_SOURCES = \
hammerClient.cpp

#testSecondaryInstanceProviders
testSecondaryInstanceProviders_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/xml -lowxml

testSecondaryInstanceProviders_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o testSecondaryInstanceProviders

testSecondaryInstanceProviders_SOURCES = \
testSecondaryInstanceProviders.cpp

#testLargeRequest
testLargeRequest_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/xml -lowxml

testLargeRequest_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o testLargeRequest

testLargeRequest_SOURCES = \
testLargeRequest.cpp


INCLUDES = \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/cimom/server \
-I$(top_srcdir)/src/http/common \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/cimxmllistener \
-I$(top_srcdir)/src/wql \
-I$(top_srcdir)/src/wql/common

