sbin_PROGRAMS = owdigestgenpass 
bin_PROGRAMS = \
owexecwql \
owcreatenamespace \
owdeletenamespace \
owenumnamespace

### owdigestgenpass
owdigestgenpass_SOURCES = OW_DigestPasswdGen.cpp

owdigestgenpass_LDADD = \
-L../common -lopenwbem

owdigestgenpass_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owdigestgenpass

### owexecwql
owexecwql_SOURCES = owexecwql.cpp

owexecwql_LDADD = \
-L../common -lopenwbem

owexecwql_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owexecwql

### owcreatenamespace
owcreatenamespace_SOURCES = owcreatenamespace.cpp

owcreatenamespace_LDADD = \
-L../common -lopenwbem

owcreatenamespace_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owcreatenamespace

### owdeletenamespace
owdeletenamespace_SOURCES = owdeletenamespace.cpp

owdeletenamespace_LDADD = \
-L../common -lopenwbem

owdeletenamespace_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owdeletenamespace

### owenumnamespace
owenumnamespace_SOURCES = owenumnamespace.cpp

owenumnamespace_LDADD = \
-L../common -lopenwbem

owenumnamespace_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owenumnamespace

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/http \
-I$(top_srcdir)/src/thread \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/net/socket


