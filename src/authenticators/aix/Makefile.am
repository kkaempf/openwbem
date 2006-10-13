MAKEBUNDLE=1
if USE_AIX_AUTHENTICATE
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs

EXPORT_PREFIX=createAuthenticator

OW_SHARED_LIBS = libaixauthentication.$(LIB_EXT)
SHAREDLIBADD = -L$(top_builddir)/src/common -lopenwbem $(BLOCXX_LIBS)
# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

noinst_LIBRARIES = \
	libaixauthentication.a 

#install to here
thelibdir = $(libdir)

#create links here
OW_EXTRA_LINK_DIRS=$(pkglibdir)/authentication

libaixauthentication_a_SOURCES = \
OW_AIXAuthentication.cpp

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/cim	\
-I$(top_srcdir)/src/ifcs

else
clean-local:


endif
EXTRA_DIST = OW_AIXAuthentication.cpp
