bin_PROGRAMS = owmofinstalltool

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/cimom/server \
-I$(top_srcdir)/src/mof \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/http/common \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/repositories/hdb

#-I$(top_srcdir)/src/repositories/tdbcr

owmofinstalltool_SOURCES = \
owmofinstalltool.cpp

owmofinstalltool_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owmofinstalltool$(EXEEXT)

if ENABLE_DB4_REPOSITORY
REPLIBFLAGS=-L$(top_builddir)/src/repositories/tdbcr -lowrepositorytdbcr
else
REPLIBFLAGS=-L$(top_builddir)/src/repositories/hdb -lowrepositoryhdb
endif


owmofinstalltool_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
$(REPLIBFLAGS) \
-L$(top_builddir)/src/cimom/server -lowserver \
-L$(top_builddir)/db/db -lowdb \
-L$(top_builddir)/src/common -lopenwbem \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

