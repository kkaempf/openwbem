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

libowservicehttp_PROJECT_LIBS = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/monitor -lowprivman

libowservicehttp_EXTERNAL_LIBS = \
$(BLOCXX_LIBS) \
$(SSL_LIBS) $(Z_LIBS)

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
owlocalhelperbin_PROGRAMS = owlocalhelper@OPENWBEM_LIBRARY_VERSION@
owlocalhelperbindir = $(libexecdir)/openwbem

owlocalhelper@OPENWBEM_LIBRARY_VERSION@_SOURCES = \
owlocalhelper_main.cpp \
OW_LocalAuthenticationCommon.cpp \
OW_LocalAuthenticationCommon.hpp

owlocalhelper@OPENWBEM_LIBRARY_VERSION@_EXTERNAL_LIBS = \
-L$(top_builddir)/src/common -lopenwbem \
$(Z_LIBS) $(SSL_LIBS) $(BLOCXX_LIBS)




# ###########################################################################
# VAS specific items (not used unless HAVE_VAS2 or HAVE_VAS3 is defined.
# Putting them here instead of spreading them out among the two conditional
# tests makes it easier to see what is going on.
if USE_RPATH
vas2_rpath_flag=$(RPATH_LINK_FLAG_WITH_PATH)@RPATH_SEPARATOR@$(VAS2_RPATH)
vas3_rpath_flag=$(RPATH_LINK_FLAG_WITH_PATH)@RPATH_SEPARATOR@$(VAS3_RPATH)
else
vas2_rpath_flag=
vas3_rpath_flag=
endif

# Override any rpath flags that exist for every other binary.
if RPATH_PREFERS_FIRST_OPTION
# HPUX (at least on parisc), unlike every most other platforms, uses only the
# first occurance of the rpath flags on the link line.  The others all seem to
# take the last occurance.
vas2_early_flags=$(vas2_rpath_flag)
vas2_late_flags=
vas3_early_flags=$(vas3_rpath_flag)
vas3_late_flags=
else
vas2_early_flags=
vas2_late_flags=$(vas2_rpath_flag)
vas3_early_flags=
vas3_late_flags=$(vas3_rpath_flag)
endif
# ###########################################################################


# This version info is confusing. VAS 2 has libvas.so.3
if HAVE_VAS2
# owspnegovas3helper rules
owspnegovas3helperbin_PROGRAMS = owspnegovas3helper@OPENWBEM_LIBRARY_VERSION@
owspnegovas3helperbindir = $(libexecdir)/openwbem

owspnegovas3helper@OPENWBEM_LIBRARY_VERSION@_SOURCES = \
owspnegovashelper_main.cpp

owspnegovas3helper@OPENWBEM_LIBRARY_VERSION@_PROJECT_LIBS = \
$(vas2_early_flags) \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/http/common -lowhttpcommon

owspnegovas3helper@OPENWBEM_LIBRARY_VERSION@_EXTERNAL_LIBS = \
$(vas2_late_flags) \
$(BLOCXX_LIBS) \
-L$(VAS2_LIB_DIR) -lvas \
$(Z_LIBS) $(SSL_LIBS)

owspnegovas3helper@OPENWBEM_LIBRARY_VERSION@_CPPFLAGS = -I$(VAS2_INCLUDE_DIR) -DOW_USE_VAS2_API $(NO_STRICT_ALIASING_FLAG)
endif

# This version info is confusing. VAS 3 has libvas.so.4
if HAVE_VAS3
# owspnegovas4helper rules
owspnegovas4helperbin_PROGRAMS = owspnegovas4helper@OPENWBEM_LIBRARY_VERSION@
owspnegovas4helperbindir = $(libexecdir)/openwbem

owspnegovas4helper@OPENWBEM_LIBRARY_VERSION@_SOURCES = \
owspnegovashelper_main.cpp

owspnegovas4helper@OPENWBEM_LIBRARY_VERSION@_PROJECT_LIBS = \
$(vas3_early_flags) \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/http/common -lowhttpcommon

owspnegovas4helper@OPENWBEM_LIBRARY_VERSION@_EXTERNAL_LIBS = \
$(vas3_late_flags) \
$(BLOCXX_LIBS) \
-L$(VAS3_LIB_DIR) -lvas \
$(Z_LIBS) $(SSL_LIBS)

owspnegovas4helper@OPENWBEM_LIBRARY_VERSION@_CPPFLAGS = -I$(VAS3_INCLUDE_DIR) -DOW_USE_VAS3_API
endif

# Here comes our GSSAPI part which has his own helper binary
# This part is only activated, if we have GSSAPI WITH SPENGO
if HAVE_SPNEGO_SUPPORT
owspnegogssapihelperbin_PROGRAMS = owspnegogssapihelper@OPENWBEM_LIBRARY_VERSION@
owspnegogssapihelperbindir = $(libexecdir)/openwbem

owspnegogssapihelper@OPENWBEM_LIBRARY_VERSION@_SOURCES = owspnegogssapihelper_main.cpp

owspnegogssapihelper@OPENWBEM_LIBRARY_VERSION@_PROJECT_LIBS = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/http/common -lowhttpcommon

owspnegogssapihelper@OPENWBEM_LIBRARY_VERSION@_EXTERNAL_LIBS = \
$(KRB5_LDFLAGS) $(GSSAPI_LIBS) $(LIB_resolv) \
$(BLOCXX_LIBS) \
$(Z_LIBS) $(SSL_LIBS)

owspnegogssapihelper@OPENWBEM_LIBRARY_VERSION@_CPPFLAGS = -I$(srcdir) $(KRB5_CPPFLAGS) $(GSSAPI_CPPFLAGS) $(AM_CPPFLAGS) $(DEFS)
endif

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/monitor \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/http/common


OW_EXECUTABLES = $(owlocalhelperbin_PROGRAMS) $(owspnegovas3helperbin_PROGRAMS) $(owspnegovas4helperbin_PROGRAMS) $(owspnegogssapihelperbin_PROGRAMS)
include $(top_srcdir)/Makefile-executable.inc
