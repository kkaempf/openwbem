include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


OW_SHARED_LIBS = libowrequesthandlerbinary.$(LIB_EXT)
SHAREDLIBADD = -L$(top_builddir)/src/common -lopenwbem

noinst_LIBRARIES = \
libowrequesthandlerbinary.a

thelibdir = $(pkglibdir)/requesthandlers

libowrequesthandlerbinary_a_SOURCES = \
OW_BinaryRequestHandler.cpp \
OW_BinaryRequestHandler.hpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim

