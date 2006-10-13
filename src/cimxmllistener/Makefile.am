MAKEDYLIB=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


OW_SHARED_LIBS = libowhttpxmllistener.$(LIB_EXT) libowcimxmllistener.$(LIB_EXT)
SHAREDLIBADD = \
-L$(top_builddir)/src/common -lopenwbem $(BLOCXX_LIBS) \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/client -lowclient \
-L$(top_builddir)/src/xml -lowxml \
-L$(top_builddir)/src/monitor -lowprivman \
-L$(top_builddir)/src/services/http -lowservicehttp
# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

owstatlib_LIBRARIES = \
libowhttpxmllistener.a \
libowcimxmllistener.a

thelibdir = $(libdir)
owstatlibdir = $(thelibdir)

libowcimxmllistener_a_SOURCES = \
OW_CIMListenerCallback.cpp \
OW_XMLListener.cpp \
OW_CIMXMLListener.cpp

libowhttpxmllistener_a_SOURCES = \
OW_CIMListenerCallback.cpp \
OW_XMLListener.cpp \
OW_ListenerAuthenticator.cpp \
OW_HTTPXMLCIMListener.cpp \
OW_CIMXMLListener.cpp

pkginclude_HEADERS = \
OW_CIMListenerCallback.hpp \
OW_ListenerAuthenticator.hpp \
OW_XMLListener.hpp \
OW_HTTPXMLCIMListener.hpp \
OW_CIMXMLListenerFwd.hpp \
OW_CIMXMLListener.hpp


INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/http/common \
-I$(top_srcdir)/src/http/client \
-I$(top_srcdir)/src/services/http \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/requesthandlers/cimxml

