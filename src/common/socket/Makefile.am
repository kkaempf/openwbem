SUBDIRS = test

pkginclude_HEADERS = \
OW_GetPass.hpp \
OW_SocketAddress.hpp \
OW_ServerSocket.hpp \
OW_Socket.hpp \
OW_NwIface.hpp \
OW_SSLCtxMgr.hpp \
OW_SSLException.hpp \
OW_SocketException.hpp \
OW_SocketStreamBuffer.hpp \
OW_SocketUtils.hpp \
OW_SocketFlags.hpp

# Shouldn't depend on anything not in common. DON'T ADD ANYTHING TO THIS LIST!
INCLUDES = \
-I$(top_srcdir)/src/common \
-I$(top_srcdir)/src/ifcs \
-I$(top_srcdir)/src/cim
