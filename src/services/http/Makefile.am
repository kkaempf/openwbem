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
-L$(top_builddir)/src/common -lopenwbem $(BLOCXX_LIBS) \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/monitor -lowprivman \
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
OW_DigestAuthentication.cpp
endif

if DISABLE_DIGEST
DIGEST_HEADER_FILES =
else
DIGEST_HEADER_FILES = \
OW_DigestAuthentication.hpp
endif


libowservicehttp_a_SOURCES = \
OW_HTTPServer.cpp \
OW_HTTPSvrConnection.cpp \
OW_LocalAuthentication.cpp \
OW_LocalAuthenticationCommon.cpp \
OW_SPNEGOAuthentication.cpp \
$(DIGEST_SOURCE_FILES)

pkginclude_HEADERS = \
OW_HTTPServer.hpp \
OW_HTTPSvrConnection.hpp \
OW_LocalAuthentication.hpp \
OW_LocalAuthenticationCommon.hpp \
OW_ServicesHttpFwd.hpp \
OW_SPNEGOAuthentication.hpp \
$(DIGEST_HEADER_FILES)


# owlocalhelper rules
owlocalhelperbin_PROGRAMS = owlocalhelper
owlocalhelperbindir = $(libexecdir)/openwbem

owlocalhelper_SOURCES = \
owlocalhelper_main.cpp \
OW_LocalAuthenticationCommon.cpp \
OW_LocalAuthenticationCommon.hpp

owlocalhelper_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o owlocalhelper$(EXEEXT) 

# Do things a bit differently and statically link to libopenwbem to avoid restrictions with LD_LIBRARY_PATH and setuid binaries.
owlocalhelper_LDADD = \
$(top_builddir)/src/common/libopenwbem.a \
$(Z_LIBS) $(SSL_LIBS) $(BLOCXX_LIBS) \
$(BINLINKAPPENDFLAGS)



# ###########################################################################
# VAS specific items (not used unless HAVE_VAS2 or HAVE_VAS3 is defined.
# Putting them here instead of spreading them out among the two conditional
# tests makes it easier to see what is going on.
if USE_RPATH
vas2_rpath_flag=$(RPATH_LINK_FLAG_WITH_PATH):$(VAS2_RPATH)
vas3_rpath_flag=$(RPATH_LINK_FLAG_WITH_PATH):$(VAS3_RPATH)
else
vas2_rpath_flag=
vas3_rpath_flag=
endif

# Override any rpath flags that exist for every other binary.
if HPUX
# HPUX, unlike every most other platforms, uses only the first occurance of the
# rpath flags on the link line.  The others all seem to take the last
# occurance.
vas2_link_flags=$(vas2_rpath_flag) $(CXXBINLINKFLAGS)
vas3_link_flags=$(vas3_rpath_flag) $(CXXBINLINKFLAGS)
else
vas2_link_flags=$(CXXBINLINKFLAGS) $(vas2_rpath_flag)
vas3_link_flags=$(CXXBINLINKFLAGS) $(vas3_rpath_flag)
endif
# ###########################################################################


# This version info is confusing. VAS 2 has libvas.so.3
if HAVE_VAS2
# owspnegovas3helper rules
owspnegovas3helperbin_PROGRAMS = owspnegovas3helper
owspnegovas3helperbindir = $(libexecdir)/openwbem

owspnegovas3helper_SOURCES = \
owspnegovashelper_main.cpp

owspnegovas3helper_LDFLAGS = \
-L$(VAS2_LIB_DIR) \
$(vas3_link_flags) \
-o owspnegovas3helper$(EXEEXT) 

owspnegovas3helper_LDADD = \
-L$(top_builddir)/src/common -lopenwbem $(BLOCXX_LIBS) \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(VAS2_LIB_DIR) -lvas \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owspnegovas3helper_CPPFLAGS = -I$(VAS2_INCLUDE_DIR) -DOW_USE_VAS2_API
endif

# This version info is confusing. VAS 3 has libvas.so.4
if HAVE_VAS3
# owspnegovas4helper rules
owspnegovas4helperbin_PROGRAMS = owspnegovas4helper
owspnegovas4helperbindir = $(libexecdir)/openwbem

owspnegovas4helper_SOURCES = \
owspnegovashelper_main.cpp

owspnegovas4helper_LDFLAGS = \
-L$(VAS3_LIB_DIR) \
$(vas3_link_flags) \
-o owspnegovas4helper$(EXEEXT) 

owspnegovas4helper_LDADD = \
-L$(top_builddir)/src/common -lopenwbem $(BLOCXX_LIBS) \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(VAS3_LIB_DIR) -lvas \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owspnegovas4helper_CPPFLAGS = -I$(VAS3_INCLUDE_DIR) -DOW_USE_VAS3_API
endif

# Here comes our GSSAPI part which has his own helper binary
# This part is only activated, if we have GSSAPI WITH SPENGO
if HAVE_SPNEGO_SUPPORT
owspnegogssapihelperbin_PROGRAMS = owspnegogssapihelper
owspnegogssapihelperbindir = $(libexecdir)/openwbem

owspnegogssapihelper_SOURCES = owspnegogssapihelper_main.cpp

owspnegogssapihelper_LDFLAGS = \
$(KRB5_LDFLAGS) $(LIB_resolv) \
-o owspnegogssapihelper$(EXEEXT)

owspnegogssapihelper_LDADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/http/common -lowhttpcommon \
$(Z_LIBS) $(SSL_LIBS) \
$(BINLINKAPPENDFLAGS)

owspnegogssapihelper_CPPFLAGS = -I$(srcdir) $(KRB5_CPPFLAGS) $(AM_CPPFLAGS) $(DEFS)
endif

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/monitor \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/http/common
