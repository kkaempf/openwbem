bin_PROGRAMS = owmofc

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/mof \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/http/common \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/common/socket

owmofc_SOURCES = \
owmofc.cpp

owmofc_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owmofc

owmofc_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/mof -lowmofc \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient

