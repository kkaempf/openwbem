include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


noinst_LIBRARIES = libnpisupport.a
OW_SHARED_LIBS = libnpisupport.so
SHAREDLIBADD = 

thelibdir = $(libdir)

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
-I$(top_srcdir)/src/cimom \
-I$(top_srcdir)/src/provider \
-I$(top_srcdir)/src/providerifcs/npi \
-I$(top_srcdir)/src/server  \
-I$(top_srcdir)/src/wql/common 

