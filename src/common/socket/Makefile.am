SUBDIRS = test

noinst_LIBRARIES = libOW_Socket.a
libOW_Socket_a_SOURCES = \
OW_DomainSocketConnection.cpp \
OW_GetPass.cpp \
OW_IPCConnection.cpp \
OW_SocketAddress.cpp \
OW_InetSSLSocketImpl.cpp \
OW_InetServerSocketImpl.cpp \
OW_InetSocket.cpp \
OW_SocketBaseImpl.cpp \
OW_InetSocketImpl.cpp \
OW_NwIface.cpp \
OW_SSLCtxMgr.cpp \
OW_SocketStreamBuffer.cpp \
OW_SocketUtils.cpp

pkginclude_HEADERS = \
OW_DomainSocketConnection.hpp \
OW_GetPass.hpp \
OW_IPCConnection.hpp \
OW_SocketAddress.hpp \
OW_InetSSLSocketImpl.hpp \
OW_InetServerSocket.hpp \
OW_InetServerSocketImpl.hpp \
OW_InetSocket.hpp \
OW_SocketBaseImpl.hpp \
OW_InetSocketImpl.hpp \
OW_NwIface.hpp \
OW_SSLCtxMgr.hpp \
OW_SSLException.hpp \
OW_SocketException.hpp \
OW_SocketStreamBuffer.hpp \
OW_SocketUtils.hpp

INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs
