MAKEDYLIB=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


OW_SHARED_LIBS = libowprovideragent.$(LIB_EXT)
SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/services/http -lowservicehttp 


owstatlib_LIBRARIES = \
libowprovideragent.a

thelibdir = $(libdir)
owstatlibdir = $(thelibdir)

libowprovideragent_a_SOURCES = \
OW_ProviderAgentCIMOMHandle.hpp \
OW_ProviderAgentCIMOMHandle.cpp \
OW_ProviderAgentEnvironment.hpp \
OW_ProviderAgentEnvironment.cpp \
OW_ProviderAgentProviderEnvironment.hpp \
OW_ProviderAgentProviderEnvironment.cpp \
OW_ProviderAgent.cpp \
OW_ProviderAgentLockerIFC.cpp

pkginclude_HEADERS = \
OW_ProviderAgent.hpp \
OW_ProviderAgentLockerIFC.hpp


INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/client	\
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/http/common \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/services/http \
-I$(top_srcdir)/src/providerifcs/cpp \
-I$(top_srcdir)/src/provider

