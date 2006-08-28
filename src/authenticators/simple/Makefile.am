MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs

EXPORT_PREFIX=createAuthenticator

OW_SHARED_LIBS = libsimpleauthentication.$(LIB_EXT)
SHAREDLIBADD = -L$(top_builddir)/src/common -lopenwbem -lblocxx
# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

noinst_LIBRARIES = \
	libsimpleauthentication.a 

#install to here
thelibdir = $(libdir)

#create links here
OW_EXTRA_LINK_DIRS=$(pkglibdir)/authentication


libsimpleauthentication_a_SOURCES = \
OW_SimpleAuthenticator.cpp 

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/cim	\
-I$(top_srcdir)/src/ifcs

