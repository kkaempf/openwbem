MAKEDYLIB=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


noinst_LIBRARIES = libowhttpclient.a
SHAREDLIBADD = \
-L$(top_builddir)/src/common/ -lopenwbem \
-L$(top_builddir)/src/http/common -lowhttpcommon

OW_SHARED_LIBS = libowhttpclient.$(LIB_EXT)
thelibdir = $(libdir)

libowhttpclient_a_SOURCES =	\
OW_HTTPClient.cpp 

pkginclude_HEADERS =	\
OW_HTTPClient.hpp

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/client \
-I$(top_srcdir)/src/http/common

