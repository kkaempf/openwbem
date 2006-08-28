MAKEBUNDLE=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


noinst_LIBRARIES = libowremoteprovifc.a
OW_SHARED_LIBS = libowremoteprovifc.$(LIB_EXT)
SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem -lblocxx \
-L$(top_builddir)/src/provider -lowprovider \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/http/common -lowhttpcommon

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

thelibdir = $(pkglibdir)/provifcs

libowremoteprovifc_a_SOURCES = \
OW_RemoteProviderInterface.hpp \
OW_RemoteProviderInterface.cpp \
OW_RemoteAssociatorProvider.hpp \
OW_RemoteAssociatorProvider.cpp \
OW_RemoteInstanceProvider.hpp \
OW_RemoteInstanceProvider.cpp \
OW_RemoteMethodProvider.hpp \
OW_RemoteMethodProvider.cpp \
OW_RemoteSecondaryInstanceProvider.hpp \
OW_RemoteSecondaryInstanceProvider.cpp \
OW_RemoteProviderUtils.hpp \
OW_RemoteProviderUtils.cpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/http/common \
-I$(top_srcdir)/src/provider

