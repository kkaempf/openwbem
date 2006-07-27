MAKEDYLIB=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


owstatlib_LIBRARIES = libnpisupport.a
OW_SHARED_LIBS = libnpisupport.$(LIB_EXT)
SHAREDLIBADD =  \
-L$(top_builddir)/src/common -lopenwbem \
-L$(top_builddir)/src/provider -lowprovider

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

thelibdir = $(libdir)
owstatlibdir = $(thelibdir)

libnpisupport_a_SOURCES = \
NPIExternal.cpp \
NPIExternal.hpp \
npicrash.c \
npicrash.h \
provider.h \
mlogsup.c \
mlogsup.h \
npi.h \
npi_import.h \
os_compile.h

#libOW_CppIFC_a_LDFLAGS = -L$(top_builddir)/src/common/
#libOW_CppIFC_a_LIBADD = -lOpenWBEM


INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/npi


