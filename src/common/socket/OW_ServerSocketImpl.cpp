/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
*  - Neither the name of Vintela, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Vintela, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_ServerSocketImpl.hpp"
#include "OW_Format.hpp"
#include "OW_ByteSwap.hpp"
#include "OW_FileSystem.hpp"
#include "OW_File.hpp"
#include "OW_Thread.hpp"
#include "OW_SocketUtils.hpp"
#include "OW_System.hpp"

extern "C"
{
#if !defined(OW_WIN32)
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#endif
}

#include <cerrno>

namespace OpenWBEM
{
//////////////////////////////////////////////////////////////////////////////
ServerSocketImpl::ServerSocketImpl(SSLServerCtxRef sslCtx)
	: m_sockfd(-1)
	, m_localAddress(SocketAddress::allocEmptyAddress(SocketAddress::INET))
	, m_isActive(false)
	, m_sslCtx(sslCtx)
#if defined(OW_WIN32)
	, m_event(NULL)
	, m_shuttingDown(false)
{
	m_event = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	OW_ASSERT(m_event != NULL);
}
#else
	, m_udsFile()
{
}
#endif

//////////////////////////////////////////////////////////////////////////////
ServerSocketImpl::ServerSocketImpl(SocketFlags::ESSLFlag isSSL)
	: m_sockfd(-1)
	, m_localAddress(SocketAddress::allocEmptyAddress(SocketAddress::INET))
	, m_isActive(false)
	, m_isSSL(isSSL)
#if defined(OW_WIN32)
	, m_event(NULL)
	, m_shuttingDown(false)
{
	m_event = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	OW_ASSERT(m_event != NULL);
}
#else
	, m_udsFile()
{
}
#endif

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
#if defined(OW_WIN32)
	::CloseHandle(m_event);
#endif
}

//////////////////////////////////////////////////////////////////////////////
Select_t
ServerSocketImpl::getSelectObj() const
{
#if defined(OW_WIN32)
	Select_t st;
	st.event = m_event;
	st.sockfd = m_sockfd;
	st.networkevents = FD_ACCEPT;
	return st;
#else
	return m_sockfd;
#endif
}

//////////////////////////////////////////////////////////////////////////////
void
ServerSocketImpl::doListen(UInt16 port, SocketFlags::ESSLFlag isSSL,
	int queueSize, const String& listenAddr,
	SocketFlags::EReuseAddrFlag reuseAddr)
{
	m_isSSL = isSSL;
	doListen(port, queueSize,listenAddr, reuseAddr);
}

#if defined(OW_WIN32)
//////////////////////////////////////////////////////////////////////////////
void
ServerSocketImpl::doListen(UInt16 port,
	int queueSize, const String& listenAddr,
	SocketFlags::EReuseAddrFlag reuseAddr)
{
	m_localAddress = SocketAddress::allocEmptyAddress(SocketAddress::INET);
	close();
	if ((m_sockfd = ::socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		OW_THROW(SocketException, Format("ServerSocketImpl: %1",
			System::lastErrorMsg(true)).c_str());
	}

	// Set listen socket to nonblocking
	unsigned long cmdArg = 1;
	if (::ioctlsocket(m_sockfd, FIONBIO, &cmdArg) == SOCKET_ERROR)
	{
		OW_THROW(SocketException, Format("ServerSocketImpl: %1",
			System::lastErrorMsg(true)).c_str());
	}

	if (reuseAddr)
	{
		DWORD reuse = 1;
		::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR,
			(const char*)&reuse, sizeof(reuse));
	}
		
	InetSocketAddress_t inetAddr;
	inetAddr.sin_family = AF_INET;
	if (listenAddr == SocketAddress::ALL_LOCAL_ADDRESSES)
	{
		inetAddr.sin_addr.s_addr = hton32(INADDR_ANY);
	}
	else
	{
		SocketAddress addr = SocketAddress::getByName(listenAddr);
		inetAddr.sin_addr.s_addr = addr.getInetAddress()->sin_addr.s_addr;
	}
	inetAddr.sin_port = hton16(port);
	if (::bind(m_sockfd, reinterpret_cast<sockaddr*>(&inetAddr), sizeof(inetAddr)) == -1)
	{
		close();
		OW_THROW(SocketException, Format("ServerSocketImpl: %1",
			System::lastErrorMsg(true)).c_str());
	}
	if (::listen(m_sockfd, queueSize) == -1)
	{
		close();
		OW_THROW(SocketException, Format("ServerSocketImpl: %1",
			System::lastErrorMsg(true)).c_str());
	}
	fillAddrParms();
	m_isActive = true;
}

namespace
{

int
waitForAcceptIO(SocketHandle_t fd, HANDLE eventArg, int timeOutSecs,
	long networkEvents)
{
	DWORD timeout = (timeOutSecs != -1)
		? static_cast<DWORD>(timeOutSecs * 1000)
		: INFINITE;

	if (networkEvents != -1L)
	{
		if(::WSAEventSelect(fd, eventArg, networkEvents) != 0)
		{
			OW_THROW(SocketException,
				Format("WSAEventSelect failed in waitForAcceptIO: %1",
				System::lastErrorMsg(true)).c_str());
		}
	}

	int cc;
	switch(::WaitForSingleObject(eventArg, timeout))
	{
		case WAIT_OBJECT_0:
			::ResetEvent(eventArg);
			cc = 0;
			break;
		case WAIT_TIMEOUT:
			cc = ETIMEDOUT;
			break;
		default:
			cc = -1;
			break;
	}

	return cc;
}

}

//////////////////////////////////////////////////////////////////////////////
Socket
ServerSocketImpl::accept(int timeoutSecs)
{
	OW_ASSERT(m_localAddress.getType() == SocketAddress::INET);

	if (!m_isActive)
	{
		OW_THROW(SocketException, "ServerSocketImpl::accept: NONE");
	}

	// Register interest in FD_ACCEPT events
	if(::WSAEventSelect(m_sockfd, m_event, FD_ACCEPT) != 0)
	{
		OW_THROW(SocketException,
			Format("WSAEventSelect failed in accept: %1",
			System::lastErrorMsg(true)).c_str());
	}

	SOCKET clntfd;
	socklen_t clntlen;
	struct sockaddr_in clntInetAddr;
	HANDLE events[2];
	int cc;

	while (true)
	{
		clntlen = sizeof(clntInetAddr);
		clntfd = ::accept(m_sockfd,
			reinterpret_cast<struct sockaddr*>(&clntInetAddr), &clntlen);
		if (clntfd != INVALID_SOCKET)
		{
			// Got immediate connection
			break;
		}

		if (::WSAGetLastError() != WSAEWOULDBLOCK)
		{
			OW_THROW(SocketException, Format("ServerSocketImpl: %1",
				System::lastErrorMsg(true)).c_str());
		}

		//cc = SocketUtils::waitForIO(m_sockfd, m_event, INFINITE, FD_ACCEPT);
		cc = waitForAcceptIO(m_sockfd, m_event, timeoutSecs, FD_ACCEPT);
		if(m_shuttingDown)
		{
			cc = -2;
		}

		switch (cc)
		{
			case -1:
				OW_THROW(SocketException, "Error while waiting for network events");
			case -2:
				OW_THROW(SocketException, "Shutdown event was signaled");
			case ETIMEDOUT:
				OW_THROW(SocketTimeoutException,"Timed out waiting for a connection");
		}
	}

	if (!m_sslCtx && m_isSSL == SocketFlags::E_SSL)
	{
		return Socket(clntfd, m_localAddress.getType(), m_isSSL);
	}

	return Socket(clntfd, m_localAddress.getType(), m_sslCtx);
}
#else
//////////////////////////////////////////////////////////////////////////////
void
ServerSocketImpl::doListen(UInt16 port,
	int queueSize, const String& listenAddr,
	SocketFlags::EReuseAddrFlag reuseAddr)
{
	m_localAddress = SocketAddress::allocEmptyAddress(SocketAddress::INET);
	close();
	if ((m_sockfd = ::socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		OW_THROW(SocketException, Format("ServerSocket2Impl: %1",
			strerror(errno)).c_str());
	}
	// set the close on exec flag so child process can't keep the socket.
	if (::fcntl(m_sockfd, F_SETFD, FD_CLOEXEC) == -1)
	{
		close();
		OW_THROW(SocketException, Format("ServerSocket2Impl failed to set "
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
	if (listenAddr == SocketAddress::ALL_LOCAL_ADDRESSES)
	{
		inetAddr.sin_addr.s_addr = hton32(INADDR_ANY);
	}
	else
	{
		SocketAddress addr = SocketAddress::getByName(listenAddr);
		inetAddr.sin_addr.s_addr = addr.getInetAddress()->sin_addr.s_addr;
	}
	inetAddr.sin_port = hton16(port);
	if (bind(m_sockfd, reinterpret_cast<sockaddr*>(&inetAddr), sizeof(inetAddr)) == -1)
	{
		close();
		OW_THROW(SocketException, Format("ServerSocket2Impl: %1",
				strerror(errno)).c_str());
	}
	if (listen(m_sockfd, queueSize) == -1)
	{
		close();
		OW_THROW(SocketException, Format("ServerSocket2Impl: %1",
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
	if ((m_sockfd = ::socket(PF_UNIX,SOCK_STREAM, 0)) == -1)
	{
		OW_THROW(SocketException, Format("ServerSocketImpl: %1",
			strerror(errno)).c_str());
	}
	// set the close on exec flag so child process can't keep the socket.
	if (::fcntl(m_sockfd, F_SETFD, FD_CLOEXEC) == -1)
	{
		close();
		OW_THROW(SocketException, Format("ServerSocketImpl failed to set "
			"close-on-exec flag on listen socket: %1",
			strerror(errno)).c_str());
	}

	// set listen socket to nonblocking; see Unix Network Programming,
	// pages 422-424.
	int fdflags = ::fcntl(m_sockfd, F_GETFL, 0);
	::fcntl(m_sockfd, F_SETFL, fdflags | O_NONBLOCK);
	
	if (reuseAddr)
	{
		// Let the kernel reuse the port without waiting for it to time out.
		// Without this line, you can't stop and immediately re-start the daemon.
		int reuse = 1;
		::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	}
	String lockfilename = filename + ".lock";
	m_udsFile = FileSystem::openOrCreateFile(lockfilename);
	if (!m_udsFile)
	{
		OW_THROW(SocketException,
			Format("Unable to open or create Unix Domain Socket lock: %1, errno: %2(%3)",
				lockfilename, errno, strerror(errno)).c_str());
	}
	// if we can't get a lock, someone else has got it open.
	if (m_udsFile.tryLock() == -1)
	{
		OW_THROW(SocketException,
			Format("Unable to lock Unix Domain Socket: %1, errno: %2(%3)",
				filename, errno, strerror(errno)).c_str());
	}
	// We got the lock, so clobber the UDS if it's there so bind will succeed.
	// If it's not gone, bind will fail.
	if (FileSystem::exists(filename))
	{
		if (!FileSystem::removeFile(filename.c_str()))
		{
			OW_THROW(SocketException,
				Format("Unable to unlink Unix Domain Socket: %1, errno: %2(%3)",
					filename, errno, strerror(errno)).c_str());
		}
	}
		
	if (::bind(m_sockfd, m_localAddress.getNativeForm(),
		m_localAddress.getNativeFormSize()) == -1)
	{
		close();
		OW_THROW(SocketException, Format("ServerSocketImpl: bind failed: %1",
				strerror(errno)).c_str());
	}
	// give anybody access to the socket
	// unfortunately, fchmod() doesn't work on a UDS
	if (::chmod(filename.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) == -1)
	{
		close();
		OW_THROW(SocketException, Format("ServerSocketImpl: chmod failed: %1",
				strerror(errno)).c_str());
	}
	if (::listen(m_sockfd, queueSize) == -1)
	{
		close();
		OW_THROW(SocketException, Format("ServerSocketImpl: listen failed: %1",
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
	return SocketUtils::waitForIO(fd, timeOutSecs, forInput) == 0;
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
	if (!m_isActive)
	{
		OW_THROW(SocketException, "ServerSocketImpl::accept: NONE");
	}
	if (SocketUtils::waitForIO(m_sockfd, timeoutSecs, SocketFlags::E_WAIT_FOR_INPUT) == 0)
	{
		int clntfd;
		socklen_t clntlen;
		struct sockaddr_in clntInetAddr;
		struct sockaddr_un clntUnixAddr;
		struct sockaddr* pSA(0);
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
		
		clntfd = ::accept(m_sockfd, pSA, &clntlen);
		if (clntfd < 0)
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
		// TODO, how to make this bw compatible?
		//return Socket(clntfd, m_localAddress.getType(), m_isSSL);
		if (!m_sslCtx && m_isSSL == SocketFlags::E_SSL)
		{
			return Socket(clntfd, m_localAddress.getType(), m_isSSL); // for bw compat.
		}
		return Socket(clntfd, m_localAddress.getType(), m_sslCtx);
	}
	else
	{
		// The timeout expired.
		OW_THROW(SocketTimeoutException,"Timed out waiting for a connection");
	}
}
#endif

//////////////////////////////////////////////////////////////////////////////
void
ServerSocketImpl::close()
{
	if (m_isActive)
	{
#if defined(OW_WIN32)
		::closesocket(m_sockfd);
		m_sockfd = INVALID_SOCKET;
#else
		::close(m_sockfd);
		if (m_localAddress.getType() == SocketAddress::UDS)
		{
			String filename = m_localAddress.toString();
			if (!FileSystem::removeFile(filename.c_str()))
			{
				OW_THROW(SocketException,
					Format("Unable to unlink Unix Domain Socket: %1, errno: %2",
						filename, errno).c_str());
			}
			if (m_udsFile)
			{
				String lockfilename = filename + ".lock";
				if (m_udsFile.unlock() == -1)
				{
					OW_THROW(SocketException,
						Format("Failed to unlock Unix Domain Socket: %1, errno: %2(%3)",
							lockfilename, errno, strerror(errno)).c_str());
				}
				m_udsFile.close();
				if (!FileSystem::removeFile(lockfilename.c_str()))
				{
					OW_THROW(SocketException,
						Format("Unable to unlink Unix Domain Socket lock: %1, errno: %2",
							lockfilename, errno).c_str());
				}
			}
		}
#endif
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
		if (getsockname(m_sockfd, reinterpret_cast<struct sockaddr*>(&addr), &len) == -1)
		{
			OW_THROW(SocketException, "SocketImpl::fillAddrParms: "
				"getsockname");
		}
		m_localAddress.assignFromNativeForm(&addr, len);
	}
#if !defined(OW_WIN32)
	else if (m_localAddress.getType() == SocketAddress::UDS)
	{
		struct sockaddr_un addr;
		memset(&addr, 0, sizeof(addr));
		len = sizeof(addr);
		if (getsockname(m_sockfd, reinterpret_cast<struct sockaddr*>(&addr), &len) == -1)
		{
			OW_THROW(SocketException, "SocketImpl::fillAddrParms: "
				"getsockname");
		}
		m_localAddress.assignFromNativeForm(&addr, len);
	}
#endif
	else
	{
		OW_ASSERT(0);
	}
}

} // end namespace OpenWBEM

