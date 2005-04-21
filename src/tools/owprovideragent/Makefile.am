sbin_PROGRAMS = owprovideragent

owprovideragent_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owprovideragent$(EXEEXT)

owprovideragent_LDADD = \
-L$(top_builddir)/src/provideragent -lowprovideragent \
-L$(top_builddir)/src/requesthandlers/cimxml -lowrequesthandlercimxml \
-L$(top_builddir)/src/providerifcs/cpp -lowcppprovifc \
-L$(top_builddir)/src/services/http -lowservicehttp \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/cimom/server -lowserver \
-L$(top_builddir)/src/common -lopenwbem \
$(SSL_LIBS) $(Z_LIBS) $(BINLINKAPPENDFLAGS)

owprovideragent_SOURCES = \
owprovideragent_main.cpp 

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/cimom/server \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/services/http \
-I$(top_srcdir)/src/provideragent

