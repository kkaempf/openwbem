if DISABLE_SCHEMA_MANIPULATION
MOFC_DIR=
else
if DISABLE_QUALIFIER_DECLARATION
MOFC_DIR=
else
if DISABLE_INSTANCE_MANIPULATION
MOFC_DIR=
else
if DISABLE_NAMESPACE_MANIPULATION
MOFC_DIR=
else
MOFC_DIR= owmofc
endif
endif
endif
endif

if DISABLE_QUALIFIER_DECLARATION
ENUMQUALPROG=
OWREPOSITORYDUMP=
else
ENUMQUALPROG= owenumqualifiers
OWREPOSITORYDUMP=owrepositorydump
endif

if DISABLE_INSTANCE_MANIPULATION
NAMESPACE_MANIPULATION_PROGS=
else
if DISABLE_NAMESPACE_MANIPULATION
NAMESPACE_MANIPULATION_PROGS=
else
NAMESPACE_MANIPULATION_PROGS= \
owcreatenamespace \
owdeletenamespace \
owcimindicationlistener
endif
endif

SUBDIRS = $(MOFC_DIR) owprovideragent

noinst_LIBRARIES = libtoolscommon.a

libtoolscommon_a_SOURCES = \
OW_ToolsCommon.hpp \
OW_ToolsCommon.cpp

sbin_PROGRAMS = \
$(OWREPOSITORYDUMP) \
owdigestgenpass

bin_PROGRAMS = \
owexecwql \
owenumnamespace \
$(NAMESPACE_MANIPULATION_PROGS) \
$(ENUMQUALPROG) \
owenumclassnames \
owenumclasses \
owenuminstances \
owenuminstancenames \
owgetclass \
owgetinstance \
owdeleteinstance \
owinvokemethod


### owdigestgenpass
owdigestgenpass_SOURCES = \
OW_DigestPasswdGen.cpp

owdigestgenpass_LDADD = \
libtoolscommon.a \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
-L$(top_builddir)/src/http/common -lowhttpcommon \
$(Z_LIBS) $(SSL_LIBS)\
$(BINLINKAPPENDFLAGS)

owdigestgenpass_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owdigestgenpass$(EXEEXT)

### owexecwql
owexecwql_SOURCES = \
owexecwql.cpp

owexecwql_LDADD = \
libtoolscommon.a \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(Z_LIBS) $(SSL_LIBS)\
$(BINLINKAPPENDFLAGS)

owexecwql_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owexecwql$(EXEEXT)

### owcreatenamespace
owcreatenamespace_SOURCES = \
owcreatenamespace.cpp

owcreatenamespace_LDADD = \
libtoolscommon.a \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owcreatenamespace_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owcreatenamespace$(EXEEXT)

### owdeletenamespace
owdeletenamespace_SOURCES = \
owdeletenamespace.cpp

owdeletenamespace_LDADD = \
libtoolscommon.a \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owdeletenamespace_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owdeletenamespace$(EXEEXT)

### owenumnamespace
owenumnamespace_SOURCES = \
owenumnamespace.cpp

owenumnamespace_LDADD = \
libtoolscommon.a \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owenumnamespace_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owenumnamespace$(EXEEXT)

### owenumqualifiers
owenumqualifiers_SOURCES = \
owenumqualifiers.cpp

owenumqualifiers_LDADD = \
libtoolscommon.a \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owenumqualifiers_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owenumqualifiers$(EXEEXT)

### owenumclassnames
owenumclassnames_SOURCES = \
owenumclassnames.cpp

owenumclassnames_LDADD = \
libtoolscommon.a \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owenumclassnames_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owenumclassnames$(EXEEXT)

### owenumclasses
owenumclasses_SOURCES = \
owenumclasses.cpp

owenumclasses_LDADD = \
libtoolscommon.a \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owenumclasses_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owenumclasses$(EXEEXT)

### owenuminstances
owenuminstances_SOURCES = \
owenuminstances.cpp

owenuminstances_LDADD = \
libtoolscommon.a \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owenuminstances_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owenuminstances$(EXEEXT)

### owenuminstancenames
owenuminstancenames_SOURCES = \
owenuminstancenames.cpp

owenuminstancenames_LDADD = \
libtoolscommon.a \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owenuminstancenames_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owenuminstancenames$(EXEEXT)

### owcimindicationlistener
owcimindicationlistener_SOURCES = \
owcimindicationlistener.cpp

owcimindicationlistener_LDADD = \
libtoolscommon.a \
-L$(top_builddir)/src/cimxmllistener -lowhttpxmllistener \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/services/http -lowservicehttp \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owcimindicationlistener_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owcimindicationlistener$(EXEEXT)

### owrepositorydump
owrepositorydump_SOURCES = \
owrepositorydump.cpp

owrepositorydump_LDADD = \
libtoolscommon.a \
-L$(top_builddir)/src/repositories/hdb -lowrepositoryhdb \
-L$(top_builddir)/db/db -lowdb \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(Z_LIBS) $(SSL_LIBS)\
$(BINLINKAPPENDFLAGS)

owrepositorydump_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owrepositorydump$(EXEEXT)

### owgetclass
owgetclass_SOURCES = \
owgetclass.cpp

owgetclass_LDADD = \
libtoolscommon.a \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owgetclass_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owgetclass$(EXEEXT)


### owgetinstance
owgetinstance_SOURCES = \
owgetinstance.cpp

owgetinstance_LDADD = \
libtoolscommon.a \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owgetinstance_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owgetinstance$(EXEEXT)


### owdeleteinstance
owdeleteinstance_SOURCES = \
owdeleteinstance.cpp

owdeleteinstance_LDADD = \
libtoolscommon.a \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owdeleteinstance_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owdeleteinstance$(EXEEXT)


### owinvokemethod
owinvokemethod_SOURCES = \
owinvokemethod.cpp

owinvokemethod_LDADD = \
libtoolscommon.a \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owinvokemethod_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owinvokemethod$(EXEEXT)



INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/http/common \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/cimxmllistener \
-I$(top_srcdir)/src/services/http \
-I$(top_srcdir)/src/repositories/hdb
