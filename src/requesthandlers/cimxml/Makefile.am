include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


OW_SHARED_LIBS = libowrequesthandlercimxml.$(LIB_EXT)
SHAREDLIBADD = -L$(top_builddir)/src/common -lopenwbem -L$(top_builddir)/src/xml -lowxml 

noinst_LIBRARIES = \
libowrequesthandlercimxml.a

thelibdir = $(pkglibdir)/requesthandlers

libowrequesthandlercimxml_a_SOURCES = \
OW_XMLExecute.cpp \
OW_XMLExecute.hpp

INCLUDES = \
-I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/xml \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim

