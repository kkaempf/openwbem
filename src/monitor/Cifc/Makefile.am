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

# SHAREDLIBADD = \
# -L$(top_builddir)/src/monitor -lowprivman \
# -L$(top_builddir)/src/common -lopenwbem

LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

libowcprivman_a_SOURCES = \
OW_IPCIO_noexcept.cpp \
OW_privman_noexcept.cpp \
$(top_srcdir)/src/common/OW_DescriptorUtils_noexcept.cpp \
$(top_srcdir)/src/common/OW_AutoDescriptor.cpp

noinst_HEADERS = OW_privman_noexcept.h OW_IPCIO_noexcept.hpp

AM_CPPFLAGS = \
-I$(top_srcdir)/src/monitor -I$(top_srcdir)/src/common -DOW_NO_EXCEPTIONS
