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
-L../../common -lopenwbem \
-L../../mof -lowmofc \
-L../../client -lowclient \
-L../../xml -lowxml \
-L../../http/common -lowhttpcommon \
-L../../http/client -lowhttpclient

