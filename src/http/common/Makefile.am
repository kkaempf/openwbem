include $(top_srcdir)/Makefile-shared-lib.inc

noinst_LIBRARIES = libowhttpcommon.a
SHAREDLIBADD =

OW_SHARED_LIBS = libowhttpcommon.so
thelibdir = $(libdir)

all-local: $(OW_SHARED_LIBS)

install-exec-local:
	@INSTALL@ -d $(DESTDIR)$(thelibdir)
	for i in $(OW_SHARED_LIBS); do \
		@INSTALL@  $$i.$(SO_EXT) $(DESTDIR)$(thelibdir); \
		ln -s $$i.$(SO_EXT) $(DESTDIR)$(thelibdir)/$$i; \
	done
	
clean-local:
	for i in $(OW_SHARED_LIBS); do \
		rm -rf $$i* .$$i.libs; \
	done


libowhttpcommon_a_SOURCES =	\
OW_HTTPChunkedIStream.cpp \
OW_HTTPChunkedOStream.cpp \
OW_HTTPCounter.cpp \
OW_HTTPDeflateIStream.cpp \
OW_HTTPDeflateOStream.cpp \
OW_HTTPLenLimitIStream.cpp \
OW_HTTPUtils.cpp \
OW_MD5.cpp


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
OW_MD5.hpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/common/socket \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim 

