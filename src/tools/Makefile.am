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
owinvokemethod \
owcheckrepository


### owdigestgenpass
owdigestgenpass_SOURCES = \
OW_DigestPasswdGen.cpp

owdigestgenpass_PROJECT_LIBS = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/http/common -lowhttpcommon

owdigestgenpass_EXTERNAL_LIBS = \
libtoolscommon.a \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)


### owexecwql
owexecwql_SOURCES = \
owexecwql.cpp

owexecwql_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem

owexecwql_EXTERNAL_LIBS = \
libtoolscommon.a \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)


### owcreatenamespace
owcreatenamespace_SOURCES = \
owcreatenamespace.cpp

owcreatenamespace_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem

owcreatenamespace_EXTERNAL_LIBS = \
libtoolscommon.a \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)

### owdeletenamespace
owdeletenamespace_SOURCES = \
owdeletenamespace.cpp

owdeletenamespace_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem

owdeletenamespace_EXTERNAL_LIBS = \
libtoolscommon.a \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)

### owcheckrepository
owcheckrepository_SOURCES = owcheckrepository.cpp

owcheckrepository_PROJECT_LIBS = \
-L$(top_builddir)/src/repositories/hdb -lowrepositoryhdb \
-L$(top_builddir)/db/db -lowdb \
-L$(top_builddir)/src/common -lopenwbem

owcheckrepository_EXTERNAL_LIBS = \
libtoolscommon.a \
$(BLOCXX_LIBS) \
$(SSL_LIBS)


### owenumnamespace
owenumnamespace_SOURCES = \
owenumnamespace.cpp

owenumnamespace_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem

owenumnamespace_EXTERNAL_LIBS = \
libtoolscommon.a \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)


### owenumqualifiers
owenumqualifiers_SOURCES = \
owenumqualifiers.cpp

owenumqualifiers_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem

owenumqualifiers_EXTERNAL_LIBS = \
libtoolscommon.a \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)


### owenumclassnames
owenumclassnames_SOURCES = \
owenumclassnames.cpp

owenumclassnames_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem

owenumclassnames_EXTERNAL_LIBS = \
libtoolscommon.a \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)


### owenumclasses
owenumclasses_SOURCES = \
owenumclasses.cpp

owenumclasses_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem

owenumclasses_EXTERNAL_LIBS = \
libtoolscommon.a \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)


### owenuminstances
owenuminstances_SOURCES = \
owenuminstances.cpp

owenuminstances_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem

owenuminstances_EXTERNAL_LIBS = \
libtoolscommon.a \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)


### owenuminstancenames
owenuminstancenames_SOURCES = \
owenuminstancenames.cpp

owenuminstancenames_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem

owenuminstancenames_EXTERNAL_LIBS = \
libtoolscommon.a \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)


### owcimindicationlistener
owcimindicationlistener_SOURCES = \
owcimindicationlistener.cpp

owcimindicationlistener_PROJECT_LIBS = \
-L$(top_builddir)/src/cimxmllistener -lowhttpxmllistener \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/services/http -lowservicehttp \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/common -lopenwbem

owcimindicationlistener_EXTERNAL_LIBS = \
libtoolscommon.a \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)


### owrepositorydump
owrepositorydump_SOURCES = \
owrepositorydump.cpp

owrepositorydump_PROJECT_LIBS = \
-L$(top_builddir)/src/repositories/hdb -lowrepositoryhdb \
-L$(top_builddir)/db/db -lowdb \
-L$(top_builddir)/src/common -lopenwbem

owrepositorydump_EXTERNAL_LIBS = \
libtoolscommon.a \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)\
$(BINLINKAPPENDFLAGS)


### owgetclass
owgetclass_SOURCES = \
owgetclass.cpp

owgetclass_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem

owgetclass_EXTERNAL_LIBS = \
libtoolscommon.a \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)


### owgetinstance
owgetinstance_SOURCES = \
owgetinstance.cpp

owgetinstance_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem

owgetinstance_EXTERNAL_LIBS = \
libtoolscommon.a \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)


### owdeleteinstance
owdeleteinstance_SOURCES = \
owdeleteinstance.cpp

owdeleteinstance_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem

owdeleteinstance_EXTERNAL_LIBS = \
libtoolscommon.a \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)


### owinvokemethod
owinvokemethod_SOURCES = \
owinvokemethod.cpp

owinvokemethod_PROJECT_LIBS = \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/common -lopenwbem

owinvokemethod_EXTERNAL_LIBS = \
libtoolscommon.a \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)


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

include $(top_srcdir)/Makefile-executable.inc
