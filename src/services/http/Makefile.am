MAKEDYLIB=1
MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc

# This is an exception to the rule, we need libowservicehttp to be in $(libdir) 
# for listeners to use as well as the services dir for the cimom to use.
install-exec-local : install-ow-shared-libs
if OW_NETWARE_INSTALL
# NetWare does not handle symbolic links so we'll just copy it.
	rm -f $(DESTDIR)$(libdir)/$(OW_SHARED_LIBS)
	cp -f $(thelibdir)/$(OW_SHARED_LIBS) $(DESTDIR)$(libdir)
else
	ln -sf $(thelibdir)/$(OW_SHARED_LIBS) $(DESTDIR)$(libdir)
endif
	if [ "$(LIB_EXT)" = "dylib" ]; then \
		if [ -f $(DESTDIR)$(thelibdir)/`echo $(OW_SHARED_LIBS)|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT).bundle ]; then \
			mv $(DESTDIR)$(thelibdir)/`echo $(OW_SHARED_LIBS)|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT) $(DESTDIR)$(thelibdir)/`echo $(OW_SHARED_LIBS)|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT).dso; \
			cp $(DESTDIR)$(thelibdir)/`echo $(OW_SHARED_LIBS)|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT).bundle $(DESTDIR)$(thelibdir)/`echo $(OW_SHARED_LIBS)|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT); \
			ln -sf $(thelibdir)/`echo $(OW_SHARED_LIBS)|cut -f1 -d'.'`.$(SO_EXT).$(LIB_EXT) $(DESTDIR)$(thelibdir)/$(OW_SHARED_LIBS); \
			fi; \
	fi;

uninstall-local : uninstall-ow-shared-libs

EXPORT_PREFIX=createService

OW_SHARED_LIBS = libowservicehttp.$(LIB_EXT)
SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/http/common -lowhttpcommon \
$(SSL_LIBS)

owstatlib_LIBRARIES = \
libowservicehttp.a

thelibdir = $(pkglibdir)/services

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
$(DIGEST_SOURCE_FILES)

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/http/common

