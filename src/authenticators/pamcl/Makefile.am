MAKEBUNDLE=1
if USE_PAM
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

EXPORT_PREFIX=createAuthenticator

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs

owpamauthbin_PROGRAMS = OW_PAMAuth

owpamauthbindir = $(libexecdir)/openwbem
OW_PAMAuth_SOURCES = OW_PAMAuthSA.cpp
OW_PAMAuth_PROJECT_LIBS = -L$(top_builddir)/src/common -lopenwbem
OW_PAMAuth_EXTERNAL_LIBS = -lpam $(DL_LIB) $(SSL_LIBS) $(BLOCXX_LIBS)

OW_SHARED_LIBS = libpamclauthentication.$(LIB_EXT)
libpamclauthentication_PROJECT_LIBS = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/monitor -lowprivman

libpamclauthentication_EXTERNAL_LIBS = -lpam  $(BLOCXX_LIBS)
# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

noinst_LIBRARIES = \
	libpamclauthentication.a 

#install to here
thelibdir = $(libdir)

#create links here
OW_EXTRA_LINK_DIRS=$(pkglibdir)/authentication


libpamclauthentication_a_SOURCES = \
OW_LinuxPAMAuthenticationCL.cpp

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/cim	\
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/monitor

else
clean-local:


endif

EXTRA_DIST = \
OW_PAMAuthSA.cpp \
OW_LinuxPAMAuthenticationCL.cpp

OW_EXECUTABLES=$(owpamauthbin_PROGRAMS)
include $(top_srcdir)/Makefile-executable.inc
