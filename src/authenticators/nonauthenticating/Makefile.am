MAKEBUNDLE=1
MAKEDYLIB=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs

EXPORT_PREFIX=createAuthenticator

OW_SHARED_LIBS = libnonauthenticatingauthentication.$(LIB_EXT)
SHAREDLIBADD = -L$(top_builddir)/src/common -lopenwbem
# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

noinst_LIBRARIES = \
	libnonauthenticatingauthentication.a 

#install to here
thelibdir = $(libdir)

#create links here
OW_EXTRA_LINK_DIRS=$(pkglibdir)/authentication


libnonauthenticatingauthentication_a_SOURCES = \
OW_NonAuthenticatingAuthenticator.cpp \
OW_NonAuthenticatingAuthenticator.hpp 

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/cim	\
-I$(top_srcdir)/src/ifcs

