if DISABLE_SCHEMA_MANIPULATION
MOFC_DIR=
else
if DISABLE_QUALIFIER_DECLARATION
MOFC_DIR=
else
if DISABLE_INSTANCE_MANIPULATION
MOFC_DIR=
else
MOFC_DIR= owmofc
endif
endif
endif

if DISABLE_QUALIFIER_DECLARATION
ENUMQUALPROG=
else
ENUMQUALPROG= owenumqualifiers
endif

if DISABLE_INSTANCE_MANIPULATION
NAMESPACE_MANIPULATION_PROGS=
else
NAMESPACE_MANIPULATION_PROGS= \
owcreatenamespace \
owdeletenamespace \
owcimindicationlistener
endif

SUBDIRS = $(MOFC_DIR)

sbin_PROGRAMS = owdigestgenpass 
bin_PROGRAMS = \
owexecwql \
owenumnamespace \
$(NAMESPACE_MANIPULATION_PROGS) \
$(ENUMQUALPROG) \
owenumclassnames \
owenumclasses \
owrepositorydump

### owdigestgenpass
owdigestgenpass_SOURCES = OW_DigestPasswdGen.cpp

owdigestgenpass_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/http/common -lowhttpcommon \
$(SSL_LIBS) $(Z_LIBS) \
$(BINLINKAPPENDFLAGS)

owdigestgenpass_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owdigestgenpass

### owexecwql
owexecwql_SOURCES = owexecwql.cpp

owexecwql_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem \
$(Z_LIBS) $(SSL_LIBS)\
$(BINLINKAPPENDFLAGS)

owexecwql_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owexecwql

### owcreatenamespace
owcreatenamespace_SOURCES = owcreatenamespace.cpp

owcreatenamespace_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owcreatenamespace_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owcreatenamespace

### owdeletenamespace
owdeletenamespace_SOURCES = owdeletenamespace.cpp

owdeletenamespace_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owdeletenamespace_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owdeletenamespace

### owenumnamespace
owenumnamespace_SOURCES = owenumnamespace.cpp

owenumnamespace_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owenumnamespace_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owenumnamespace

### owenumqualifiers
owenumqualifiers_SOURCES = owenumqualifiers.cpp

owenumqualifiers_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owenumqualifiers_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owenumqualifiers

### owenumclassnames
owenumclassnames_SOURCES = owenumclassnames.cpp

owenumclassnames_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owenumclassnames_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owenumclassnames

### owenumclasses
owenumclasses_SOURCES = owenumclasses.cpp

owenumclasses_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owenumclasses_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owenumclasses

### owcimindicationlistener
owcimindicationlistener_SOURCES = owcimindicationlistener.cpp

owcimindicationlistener_LDADD = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/cimxmllistener -lowhttpxmllistener \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/services/http -lowservicehttp \
-L$(top_builddir)/src/common -lopenwbem \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owcimindicationlistener_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owcimindicationlistener

### owrepositorydump
owrepositorydump_SOURCES = owrepositorydump.cpp

owrepositorydump_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/repositories/hdb -lowrepositoryhdb \
-L$(top_builddir)/db/db -lowdb

owrepositorydump_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owrepositorydump




INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/http/common \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/cimxmllistener


