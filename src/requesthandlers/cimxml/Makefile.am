MAKEBUNDLE=1
MAKEDYLIB=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

EXPORT_PREFIX=createRequestHandler

include $(top_srcdir)/Makefile-shared-lib-inst.inc

#TODO: does this extra junk need to be here?
install-exec-local : install-ow-shared-libs
	if [ "$(LIB_EXT)" = "dylib" ]; then \
		if [ -f $(DESTDIR)$(thelibdir)/`echo $(OW_SHARED_LIBS)|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT).bundle ]; then \
			mv $(DESTDIR)$(thelibdir)/`echo $(OW_SHARED_LIBS)|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT) $(DESTDIR)$(thelibdir)/`echo $(OW_SHARED_LIBS)|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT).dso; \
			cp $(DESTDIR)$(thelibdir)/`echo $(OW_SHARED_LIBS)|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT).bundle $(DESTDIR)$(thelibdir)/`echo $(OW_SHARED_LIBS)|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT); \
			ln -sf $(thelibdir)/`echo $(OW_SHARED_LIBS)|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT) $(DESTDIR)$(thelibdir)/$(OW_SHARED_LIBS); \
			fi; \
	fi;

uninstall-local : uninstall-ow-shared-libs


OW_SHARED_LIBS = libowrequesthandlercimxml.$(LIB_EXT)
libowrequesthandlercimxml_PROJECT_LIBS = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/xml -lowxml

libowrequesthandlercimxml_EXTERNAL_LIBS = \
$(BLOCXX_LIBS)

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

noinst_LIBRARIES = \
libowrequesthandlercimxml.a

#install to here
thelibdir = $(libdir)

#create links here
OW_EXTRA_LINK_DIRS=$(pkglibdir)/requesthandlers


libowrequesthandlercimxml_a_SOURCES = \
OW_XMLExecute.cpp \
OW_XMLExecute.hpp

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim

