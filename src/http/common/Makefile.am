noinst_LIBRARIES = libOW_Http.a

libOW_Http_a_SOURCES =	\
OW_HTTPChunkedIStream.cpp \
OW_HTTPChunkedOStream.cpp \
OW_HTTPDeflateOStream.cpp \
OW_HTTPDeflateIStream.cpp \
OW_HTTPCounter.cpp \
OW_HTTPLenLimitIStream.cpp \
OW_MD5.cpp \
OW_HTTPUtils.cpp 


pkginclude_HEADERS =	\
OW_HTTPChunkException.hpp \
OW_HTTPChunkedIStream.hpp \
OW_HTTPChunkedOStream.hpp \
OW_HTTPDeflateOStream.hpp \
OW_HTTPDeflateIStream.hpp \
OW_HTTPCounter.hpp \
OW_HTTPLenLimitIStream.hpp \
OW_HTTPStatusCodes.h \
OW_HTTPException.hpp \
OW_MD5.hpp \
OW_HTTPUtils.hpp                                                                

INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/common	\
-I$(top_srcdir)/src/thread -I$(top_srcdir)/src/net/socket \
-I$(top_srcdir)/src/xml		\
-I$(top_srcdir)/src/http/server		\
-I$(top_srcdir)/src/cim -I$(top_srcdir)/src/net/socket		\
-I$(top_srcdir)/src/cimom -I$(top_srcdir)/src/client 

SUBDIRS = client server

