SUBDIRS = owmofc

sbin_PROGRAMS = owdigestgenpass 
bin_PROGRAMS = \
owexecwql \
owcreatenamespace \
owdeletenamespace \
owenumnamespace

### owdigestgenpass
owdigestgenpass_SOURCES = OW_DigestPasswdGen.cpp

owdigestgenpass_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/http/common -lowhttpcommon

owdigestgenpass_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owdigestgenpass

### owexecwql
owexecwql_SOURCES = owexecwql.cpp

owexecwql_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient

owexecwql_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owexecwql

### owcreatenamespace
owcreatenamespace_SOURCES = owcreatenamespace.cpp

owcreatenamespace_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient

owcreatenamespace_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owcreatenamespace

### owdeletenamespace
owdeletenamespace_SOURCES = owdeletenamespace.cpp

owdeletenamespace_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient

owdeletenamespace_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owdeletenamespace

### owenumnamespace
owenumnamespace_SOURCES = owenumnamespace.cpp

owenumnamespace_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient

owenumnamespace_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owenumnamespace

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/http/common \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/client


