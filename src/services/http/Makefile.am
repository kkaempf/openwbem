MAKEDYLIB=1
MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc

# This is an exception to the rule, we need libowservicehttp to be in $(libdir) 
# for listeners to use as well as the services dir for the cimom to use.
install-exec-local : install-ow-shared-libs

uninstall-local : uninstall-ow-shared-libs

EXPORT_PREFIX=createService

OW_SHARED_LIBS = libowservicehttp.$(LIB_EXT)
SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/http/common -lowhttpcommon \
$(SSL_LIBS)

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

owstatlib_LIBRARIES = \
libowservicehttp.a

#install to here
thelibdir = $(libdir)

#create links here
OW_EXTRA_LINK_DIRS=$(pkglibdir)/services

# We want the static lib in $(libdir), not $(thelibdir) in this case. 
owstatlibdir = $(libdir)

if DISABLE_DIGEST
DIGEST_SOURCE_FILES =
else
DIGEST_SOURCE_FILES = \
OW_DigestAuthentication.cpp \
OW_DigestAuthentication.hpp
endif


libowservicehttp_a_SOURCES = \
OW_HTTPServer.cpp \
OW_HTTPSvrConnection.cpp \
OW_HTTPServer.hpp \
OW_HTTPSvrConnection.hpp \
OW_LocalAuthentication.cpp \
OW_LocalAuthentication.hpp \
OW_LocalAuthenticationCommon.cpp \
OW_LocalAuthenticationCommon.hpp \
$(DIGEST_SOURCE_FILES)

pkginclude_HEADERS = \
OW_ServicesHttpFwd.hpp


# owlocalhelper rules
owlocalhelperbin_PROGRAMS = owlocalhelper
owlocalhelperbindir = $(libexecdir)/openwbem

owlocalhelper_SOURCES = \
owlocalhelper_main.cpp

owlocalhelper_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owlocalhelper$(EXEEXT) 

# Do things a bit differently and statically link to libopenwbem to avoid restrictions with LD_LIBRARY_PATH and setuid binaries.
owlocalhelper_LDADD = \
$(top_builddir)/src/common/libopenwbem.a \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)


INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/http/common

