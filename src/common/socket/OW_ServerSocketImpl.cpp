/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "OW_config.h"
#include "OW_ServerSocketImpl.hpp"
#include "OW_NwIface.hpp"
#include "OW_Format.hpp"
#include "OW_ByteSwap.hpp"
#include "OW_FileSystem.hpp"
#include "OW_File.hpp"
#include "OW_Thread.hpp"
extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#ifdef OW_USE_GNU_PTH
#include <pth.h>
#endif
}

namespace OpenWBEM
{

//////////////////////////////////////////////////////////////////////////////
ServerSocketImpl::ServerSocketImpl(SocketFlags::ESSLFlag isSSL)
	: m_sockfd(-1)
	, m_localAddress(SocketAddress::allocEmptyAddress(SocketAddress::INET))
	, m_isActive(false)
	, m_isSSL(isSSL)
	, m_udsFile()
{
}
//////////////////////////////////////////////////////////////////////////////
ServerSocketImpl::~ServerSocketImpl()
{
	try
	{
		close();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
}
//////////////////////////////////////////////////////////////////////////////
Select_t
ServerSocketImpl::getSelectObj() const
{
	return m_sockfd;
}
//////////////////////////////////////////////////////////////////////////////
void
ServerSocketImpl::doListen(UInt16 port, SocketFlags::ESSLFlag isSSL,
	int queueSize, SocketFlags::EAllInterfacesFlag allInterfaces, 
	SocketFlags::EReuseAddrFlag reuseAddr)
{
	m_localAddress = SocketAddress::allocEmptyAddress(SocketAddress::INET);
	m_isSSL = isSSL;
	close();
	if((m_sockfd = ::socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		OW_THROW(SocketException, format("ServerSocketImpl: %1",
			strerror(errno)).c_str());
	}
	// set the close on exec flag so child process can't keep the socket.
	if (::fcntl(m_sockfd, F_SETFD, FD_CLOEXEC) == -1)
	{
		close();
		OW_THROW(SocketException, format("ServerSocketImpl failed to set "
			"close-on-exec flag on listen socket: %1",
			strerror(errno)).c_str());
	}
	// set listen socket to nonblocking; see Unix Network Programming,
	// pages 422-424.
	int fdflags = ::fcntl(m_sockfd, F_GETFL, 0);
	::fcntl(m_sockfd, F_SETFL, fdflags | O_NONBLOCK);
	// is this safe? Should be, but some OS kernels have problems with it.
	// It's OK on current linux versions.  Definitely not on
	// OLD (kernel < 1.3.60) ones.  Who knows about on other OS's like UnixWare or
	// OpenServer?
	// See http://monkey.org/openbsd/archive/misc/9601/msg00031.html
	// or just google for "bind() Security Problems"
	// Let the kernel reuse the port without waiting for it to time out.
	// Without this line, you can't stop and immediately re-start the daemon.
	if (reuseAddr)
	{
		int reuse = 1;
		::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	}
		
	InetSocketAddress_t inetAddr;
	inetAddr.sin_family = AF_INET;
	if(allInterfaces)
	{
		inetAddr.sin_addr.s_addr = hton32(INADDR_ANY);
	}
	else
	{
		NwIface ifc;
		inetAddr.sin_addr.s_addr = ifc.getIPAddress();
	}
	inetAddr.sin_port = hton16(port);
	if(bind(m_sockfd, reinterpret_cast<sockaddr*>(&inetAddr), sizeof(inetAddr)) == -1)
	{
		close();
		OW_THROW(SocketException, format("ServerSocketImpl: %1",
				strerror(errno)).c_str());
	}
	if(listen(m_sockfd, queueSize) == -1)
	{
		close();
		OW_THROW(SocketException, format("ServerSocketImpl: %1",
			strerror(errno)).c_str());
	}
	fillAddrParms();
	m_isActive = true;
}
//////////////////////////////////////////////////////////////////////////////
void
ServerSocketImpl::doListen(const String& filename, int queueSize, bool reuseAddr)
{
	m_localAddress = SocketAddress::getUDS(filename);
	close();
	if((m_sockfd = ::socket(PF_UNIX,SOCK_STREAM, 0)) == -1)
	{
		OW_THROW(SocketException, format("ServerSocketImpl: %1",
			strerror(errno)).c_str());
	}
	// set the close on exec flag so child process can't keep the socket.
	if (::fcntl(m_sockfd, F_SETFD, FD_CLOEXEC) == -1)
	{
		close();
		OW_THROW(SocketException, format("ServerSocketImpl failed to set "
			"close-on-exec flag on listen socket: %1",
			strerror(errno)).c_str());
	}
	// set listen socket to nonblocking; see Unix Network Programming,
	// pages 422-424.
	int fdflags = ::fcntl(m_sockfd, F_GETFL, 0);
	::fcntl(m_sockfd, F_SETFL, fdflags | O_NONBLOCK);
	// is this safe? Should be, but some OS kernels have problems with it.
	// It's OK on current linux versions.  Definitely not on
	// OLD (kernel < 1.3.60) ones.  Who knows about on other OS's like UnixWare or
	// OpenServer?
	// See http://monkey.org/openbsd/archive/misc/9601/msg00031.html
	// or just google for "bind() Security Problems"
	
	// Let the kernel reuse the port without waiting for it to time out.
	// Without this line, you can't stop and immediately re-start the daemon.
	if (reuseAddr)
	{
		int reuse = 1;
		::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	}
	String lockfilename = filename + ".lock";
	m_udsFile = FileSystem::openOrCreateFile(lockfilename);
	if (!m_udsFile)
	{
		OW_THROW(SocketException,
			format("Unable to open or create Unix Domain Socket lock: %1, errno: %2(%3)",
				lockfilename, errno, strerror(errno)).c_str());
	}
	// if we can't get a lock, someone else has got it open.
	if (m_udsFile.tryLock() == -1)
	{
		OW_THROW(SocketException,
			format("Unable to lock Unix Domain Socket: %1, errno: %2(%3)",
				filename, errno, strerror(errno)).c_str());
	}
	// We got the lock, so clobber the UDS if it's there so bind will succeed.
	// If it's not gone, bind will fail.
	if (FileSystem::exists(filename))
	{
		if (!FileSystem::removeFile(filename.c_str()))
		{
			OW_THROW(SocketException,
				format("Unable to unlink Unix Domain Socket: %1, errno: %2(%3)",
					filename, errno, strerror(errno)).c_str());
		}
	}
		
	if(bind(m_sockfd, m_localAddress.getNativeForm(),
		m_localAddress.getNativeFormSize()) == -1)
	{
		close();
		OW_THROW(SocketException, format("ServerSocketImpl: %1",
				strerror(errno)).c_str());
	}
	if(listen(m_sockfd, queueSize) == -1)
	{
		close();
		OW_THROW(SocketException, format("ServerSocketImpl: %1",
			strerror(errno)).c_str());
	}
	fillAddrParms();
	m_isActive = true;
}
//////////////////////////////////////////////////////////////////////////////
bool
ServerSocketImpl::waitForIO(int fd, int timeOutSecs, 
	SocketFlags::EWaitDirectionFlag forInput)
{
	fd_set thefds;
	fd_set* preadfds = NULL;
	fd_set* pwritefds = NULL;
	int rc;
	struct timeval *ptimeval = NULL;
	struct timeval timeout;
	FD_ZERO(&thefds);
	FD_SET(fd, &thefds);
	if(timeOutSecs != -1)
	{
		timeout.tv_sec = timeOutSecs;
		timeout.tv_usec = 0;
		ptimeval = &timeout;
	}
	if(forInput == SocketFlags::E_WAIT_FOR_INPUT)
	{
		preadfds = &thefds;
	}
	else
	{
		pwritefds = &thefds;
	}
	if ((rc = ::select(fd+1, preadfds, pwritefds, NULL, ptimeval)) == -1)
	{
		if (errno == EINTR)
		{
			Thread::testCancel();
		}
		OW_THROW(SocketException, "waitForIO: select");
	}
	return (rc > 0);
}
//////////////////////////////////////////////////////////////////////////////
/*
String
ServerSocketImpl::addrString()
{
	return inetAddrToString(m_localAddress, m_localPort);
}
*/
//////////////////////////////////////////////////////////////////////////////
Socket
ServerSocketImpl::accept(int timeoutSecs)
{
	if(!m_isActive)
	{
		OW_THROW(SocketException, "ServerSocketImpl::accept: NONE");
	}
	if(waitForIO(m_sockfd, timeoutSecs, SocketFlags::E_WAIT_FOR_INPUT))
	{
		int clntfd;
		socklen_t clntlen;
		struct sockaddr_in clntInetAddr;
		struct sockaddr_un clntUnixAddr;
		struct sockaddr* pSA;
		if (m_localAddress.getType() == SocketAddress::INET)
		{
			pSA = reinterpret_cast<struct sockaddr*>(&clntInetAddr);
			clntlen = sizeof(clntInetAddr);
		}
		else if (m_localAddress.getType() == SocketAddress::UDS)
		{
			pSA = reinterpret_cast<struct sockaddr*>(&clntUnixAddr);
			clntlen = sizeof(clntUnixAddr);
		}
		else
		{
			OW_ASSERT(0);
		}
		
#ifdef OW_USE_GNU_PTH
		clntfd = ::pth_accept(m_sockfd, pSA, &clntlen);
#else
		clntfd = ::accept(m_sockfd, pSA, &clntlen);
#endif
		if(clntfd < 0)
		{
			// check to see if client aborts connection between select and accept.
			// see Unix Network Programming pages 422-424.
			if (errno == EWOULDBLOCK
				 || errno == ECONNABORTED
#ifdef EPROTO
				 || errno == EPROTO
#endif
				)
			{
				OW_THROW(SocketException, "Client aborted TCP connection "
					"between select() and accept()");
			}
		
			if (errno == EINTR)
			{
				Thread::testCancel();
			}
			OW_THROW(SocketException, "ServerSocketImpl::accept");
		}
		// set socket back to blocking; see Unix Network Programming,
		// pages 422-424.
		int fdflags = ::fcntl(clntfd, F_GETFL, 0);
		// On most OSs non-blocking is inherited from the listen socket,
		// but it's not on Openserver.
		if ((fdflags & O_NONBLOCK) == O_NONBLOCK)
		{
			::fcntl(clntfd, F_SETFL, fdflags ^ O_NONBLOCK);
		}
		return Socket(clntfd, m_localAddress.getType(), m_isSSL);
	}
	else
	{
		// The timeout expired.
		OW_THROW(SocketTimeoutException,"Timed out waiting for a connection");
	}
}
//////////////////////////////////////////////////////////////////////////////
void
ServerSocketImpl::close()
{
	if(m_isActive)
	{
		::close(m_sockfd);
		if (m_localAddress.getType() == SocketAddress::UDS)
		{
			String filename = m_localAddress.toString();
			if (!FileSystem::removeFile(filename.c_str()))
			{
				OW_THROW(SocketException,
					format("Unable to unlink Unix Domain Socket: %1, errno: %2",
						filename, errno).c_str());
			}
			if (m_udsFile)
			{
				String lockfilename = filename + ".lock";
				if (m_udsFile.unlock() == -1)
				{
					OW_THROW(SocketException,
						format("Failed to unlock Unix Domain Socket: %1, errno: %2(%3)",
							lockfilename, errno, strerror(errno)).c_str());
				}
				m_udsFile.close();
				if (!FileSystem::removeFile(lockfilename.c_str()))
				{
					OW_THROW(SocketException,
						format("Unable to unlink Unix Domain Socket lock: %1, errno: %2",
							lockfilename, errno).c_str());
				}
			}
		}
		m_isActive = false;
	}
}
//////////////////////////////////////////////////////////////////////////////
void
ServerSocketImpl::fillAddrParms()
{
	socklen_t len;
	if (m_localAddress.getType() == SocketAddress::INET)
	{
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		len = sizeof(addr);
		if(getsockname(m_sockfd, reinterpret_cast<struct sockaddr*>(&addr), &len) == -1)
		{
			OW_THROW(SocketException, "SocketImpl::fillAddrParms: "
				"getsockname");
		}
		m_localAddress.assignFromNativeForm(&addr, len);
	}
	else if (m_localAddress.getType() == SocketAddress::UDS)
	{
		struct sockaddr_un addr;
		memset(&addr, 0, sizeof(addr));
		len = sizeof(addr);
		if(getsockname(m_sockfd, reinterpret_cast<struct sockaddr*>(&addr), &len) == -1)
		{
			OW_THROW(SocketException, "SocketImpl::fillAddrParms: "
				"getsockname");
		}
		m_localAddress.assignFromNativeForm(&addr, len);
	}
	else
	{
		OW_ASSERT(0);
	}
}

} // end namespace OpenWBEM

