# This library must be loadable (bundle) and linkable (dylib).
MAKEBUNDLE=1
MAKEDYLIB=1
# This library can't be installed as a 'fake' library.
NO_FAKE_LIB_INSTALL=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs

thelibdir = $(libdir)

OW_SHARED_LIBS = libowcprivman.$(LIB_EXT)

noinst_LIBRARIES = libowcprivman.a

LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

libowcprivman_a_SOURCES = \
OW_IPCIO_noexcept.cpp \
OW_privman_noexcept.cpp \
Cifc_DescriptorUtils_noexcept.cpp \
Cifc_AutoDescriptor.cpp

noinst_HEADERS = \
OW_privman_noexcept.h \
OW_IPCIO_noexcept.hpp \
Cifc_AutoDescriptor.hpp \
Cifc_DescriptorUtils_noexcept.hpp 

pkginclude_HEADERS = \
OW_privman.h

AM_CPPFLAGS = \
-I$(top_srcdir)/src/monitor -I$(top_srcdir)/src/common -DOW_NO_EXCEPTIONS
