MAKEDYLIB=1

include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


SHAREDLIBADD = \
-L$(top_builddir)/src/common/ -lopenwbem \
-L$(top_builddir)/src/http/common -lowhttpcommon \
-L$(top_builddir)/src/http/client -lowhttpclient \
-L$(top_builddir)/src/xml -lowxml \
$(SLP_LIB)

noinst_LIBRARIES = libowclient.a

OW_SHARED_LIBS = libowclient.$(LIB_EXT)

thelibdir = $(libdir)

libowclient_a_SOURCES = \
OW_CIMOMLocator.cpp \
OW_CIMOMInfo.cpp \
OW_CIMClient.cpp \
OW_CIMOMLocatorSLP.cpp \
OW_ClientCIMOMHandle.cpp \
OW_CIMXMLCIMOMHandle.cpp \
OW_BinaryCIMOMHandle.cpp \
OW_ClientCIMOMHandleConnectionPool.cpp

pkginclude_HEADERS = \
OW_CIMOMLocator.hpp \
OW_CIMOMLocatorSLP.hpp \
OW_CIMOMInfo.hpp \
OW_CIMClient.hpp \
OW_ClientCIMOMHandle.hpp \
OW_CIMXMLCIMOMHandle.hpp \
OW_BinaryCIMOMHandle.hpp \
OW_ClientCIMOMHandleConnectionPool.hpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/http/common \
-I$(top_srcdir)/src/http/client


