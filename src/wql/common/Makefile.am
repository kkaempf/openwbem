MAKEDYLIB=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


OW_SHARED_LIBS = libowwqlcommon.$(LIB_EXT)
SHAREDLIBADD = -L$(top_builddir)/src/common -lopenwbem
thelibdir = $(libdir)

noinst_LIBRARIES = libowwqlcommon.a

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim

libowwqlcommon_a_SOURCES = \
OW_WQLCompile.cpp \
OW_WQLOperand.cpp \
OW_WQLPropertySource.cpp \
OW_WQLSelectStatement.cpp \
OW_WQLInstancePropertySource.cpp

pkginclude_HEADERS = \
OW_WQLCompile.hpp \
OW_WQLOperand.hpp \
OW_WQLOperation.hpp \
OW_WQLPropertySource.hpp \
OW_WQLSelectStatement.hpp \
OW_WQLInstancePropertySource.hpp

