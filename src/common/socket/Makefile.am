SUBDIRS = test

noinst_LIBRARIES = libOW_Socket.a
libOW_Socket_a_SOURCES = \
OW_GetPass.cpp \
OW_SocketAddress.cpp \
OW_SSLSocketImpl.cpp \
OW_ServerSocketImpl.cpp \
OW_Socket.cpp \
OW_SocketBaseImpl.cpp \
OW_SocketImpl.cpp \
OW_NwIface.cpp \
OW_SSLCtxMgr.cpp \
OW_SocketStreamBuffer.cpp \
OW_SocketUtils.cpp

pkginclude_HEADERS = \
OW_GetPass.hpp \
OW_SocketAddress.hpp \
OW_SSLSocketImpl.hpp \
OW_ServerSocket.hpp \
OW_ServerSocketImpl.hpp \
OW_Socket.hpp \
OW_SocketBaseImpl.hpp \
OW_SocketImpl.hpp \
OW_NwIface.hpp \
OW_SSLCtxMgr.hpp \
OW_SSLException.hpp \
OW_SocketException.hpp \
OW_SocketStreamBuffer.hpp \
OW_SocketUtils.hpp \
OW_SocketFlags.hpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs
