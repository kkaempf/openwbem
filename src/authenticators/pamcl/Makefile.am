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

OW_PAMAuth_LDFLAGS = \
$(CXXBINLINKFLAGS) \
-o OW_PAMAuth

owpamauthbindir = $(libexecdir)/openwbem
OW_PAMAuth_SOURCES = OW_PAMAuthSA.cpp
OW_PAMAuth_LDADD = -lpam $(DL_LIB) $(SSL_LIBS) $(BINLINKAPPENDFLAGS)

OW_SHARED_LIBS = libpamclauthentication.$(LIB_EXT)
SHAREDLIBADD = -lpam -L$(top_builddir)/src/common/ -lopenwbem
# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = 4

noinst_LIBRARIES = \
	libpamclauthentication.a 

thelibdir = $(pkglibdir)/authentication

libpamclauthentication_a_SOURCES = \
OW_LinuxPAMAuthenticationCL.cpp

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/cim	\
-I$(top_srcdir)/src/ifcs

else
clean-local:


endif

EXTRA_DIST = \
OW_PAMAuthSA.cpp \
OW_LinuxPAMAuthenticationCL.cpp

