noinst_PROGRAMS = cimlistener wqlTest1 aclTest CIMOMLocator

CIMOMLocator_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o CIMOMLocator

CIMOMLocator_LDADD = \
-L$(top_builddir)/src/common -lopenwbem

CIMOMLocator_SOURCES = \
CIMOMLocator.cpp
 
cimlistener_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o cimlistener

cimlistener_LDADD = \
-L$(top_builddir)/src/common -L$(top_builddir)/src/services/http -L$(top_builddir)/src/listener -lopenwbem -lowservicehttp -lowhttpxmllistener

cimlistener_SOURCES = \
cimListenerTest.cpp

wqlTest1_LDADD = \
-L$(top_builddir)/src/common -lopenwbem 

wqlTest1_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o wqlTest1

wqlTest1_SOURCES = \
wqlTest1.cpp

aclTest_LDADD = \
-L$(top_builddir)/src/common -lopenwbem 

aclTest_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o aclTest

aclTest_SOURCES = \
aclTest.cpp

INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/cim -I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/server -I$(top_srcdir)/src/http \
-I$(top_srcdir)/src/thread \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/net/socket \
-I$(top_srcdir)/src/listener

AM_CXXFLAGS = -ftemplate-depth-25
