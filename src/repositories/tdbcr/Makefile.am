MAKEDYLIB=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


OW_SHARED_LIBS = libowrepositorytdbcr.$(LIB_EXT)

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = 4

noinst_LIBRARIES = \
libowrepositorytdbcr.a

thelibdir = $(libdir)

SHAREDLIBADD = \
-ldb

if DISABLE_ASSOCIATION_TRAVERSAL
ASSOCIATION_S=
else
ASSOCIATION_S= \
OW_AssocDb2.cpp \
OW_AssocDb2.hpp
endif

libowrepositorytdbcr_a_SOURCES = \
$(ASSOCIATION_S) \
OW_CIMRepository2.cpp \
OW_CIMRepository2.hpp \
OW_InstanceRepository2.cpp \
OW_InstanceRepository2.hpp \
OW_MetaRepository2.cpp \
OW_MetaRepository2.hpp \
OW_dbDatabase.hpp \
OW_dbDatabase.cpp \
OW_dbUtils.hpp \
OW_dbUtils.cpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/common

