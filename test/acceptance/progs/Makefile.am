noinst_PROGRAMS = cimlistener wqlTest1 aclTest CIMOMLocator owcimclientcl \
	fakeclient testinstprov testCIMExceptions

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
-L$(top_builddir)/src/http/client -lowhttpclient

wqlTest1_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o wqlTest1

wqlTest1_SOURCES = \
wqlTest1.cpp

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

INCLUDES = \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/server \
-I$(top_srcdir)/src/http/common \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/cimxmllistener

AM_CXXFLAGS=-ftemplate-depth-25
