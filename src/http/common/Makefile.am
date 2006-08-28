MAKEDYLIB=1
include $(top_srcdir)/Makefile-shared-lib.inc
all-local : all-ow-shared-libs
clean-local : clean-ow-shared-libs

include $(top_srcdir)/Makefile-shared-lib-inst.inc
install-exec-local : install-ow-shared-libs
uninstall-local : uninstall-ow-shared-libs


owstatlib_LIBRARIES = libowhttpcommon.a
SHAREDLIBADD = -L$(top_builddir)/src/common/ -lopenwbem -lblocxx $(Z_LIBS)

OW_SHARED_LIBS = libowhttpcommon.$(LIB_EXT)

# increment this number every time a release breaks binary compatibility
LIBRARY_BINARY_VERSION = $(OPENWBEM_LIBRARY_VERSION)

thelibdir = $(libdir)
owstatlibdir = $(thelibdir)

libowhttpcommon_a_SOURCES =	\
OW_HTTPChunkedIStream.cpp \
OW_HTTPChunkedOStream.cpp \
OW_HTTPChunkException.cpp \
OW_HTTPCounter.cpp \
OW_HTTPDeflateIStream.cpp \
OW_HTTPDeflateOStream.cpp \
OW_HTTPException.cpp \
OW_HTTPLenLimitIStream.cpp \
OW_HTTPUtils.cpp 


pkginclude_HEADERS =	\
OW_Base64FormatException.hpp \
OW_HTTPChunkException.hpp \
OW_HTTPChunkedIStream.hpp \
OW_HTTPChunkedOStream.hpp \
OW_HTTPCounter.hpp \
OW_HTTPDeflateIStream.hpp \
OW_HTTPDeflateOStream.hpp \
OW_HTTPException.hpp \
OW_HTTPLenLimitIStream.hpp \
OW_HTTPStatusCodes.hpp \
OW_HTTPUtils.hpp \
OW_HttpCommonFwd.hpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim 

