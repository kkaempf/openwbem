/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
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

//////////////////////////////////////////////////////////////////////////////
OW_ServerSocketImpl::OW_ServerSocketImpl(OW_Bool isSSL) 
	: m_sockfd(-1)
	, m_localAddress(OW_SocketAddress::allocEmptyAddress(OW_SocketAddress::INET))
	, m_isActive(false)
	, m_isSSL(isSSL)
{
}


//////////////////////////////////////////////////////////////////////////////
OW_ServerSocketImpl::~OW_ServerSocketImpl()
{
	close();
}

//////////////////////////////////////////////////////////////////////////////
OW_Select_t
OW_ServerSocketImpl::getSelectObj() const
{
	return m_sockfd;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ServerSocketImpl::doListen(OW_UInt16 port, OW_Bool isSSL,
	int queueSize, OW_Bool allInterfaces)
{
	m_localAddress = OW_SocketAddress::allocEmptyAddress(OW_SocketAddress::INET);
	m_isSSL = isSSL;
	close();

	if((m_sockfd = ::socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		OW_THROW(OW_SocketException, format("OW_ServerSocketImpl: %1",
			strerror(errno)).c_str());
	}

	// set listen socket to nonblocking; see Unix Network Programming,
	// pages 422-424.
	int fdflags = ::fcntl(m_sockfd, F_GETFL, 0);
	::fcntl(m_sockfd, F_SETFL, fdflags | O_NONBLOCK);

//#ifdef OW_DEBUG // TODO is this safe?
	// Let the kernel reuse the port without waiting for it to time out.
	int reuse = 1;
	::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
//#endif

		
	OW_InetSocketAddress_t inetAddr;
	inetAddr.sin_family = AF_INET;

	if(allInterfaces)
	{
		inetAddr.sin_addr.s_addr = OW_hton32(INADDR_ANY);
	}
	else
	{
		OW_NwIface ifc;
		inetAddr.sin_addr.s_addr = ifc.getIPAddress();
	}

	inetAddr.sin_port = OW_hton16(port);
	if(bind(m_sockfd, (sockaddr*)&inetAddr, sizeof(inetAddr)) == -1)
	{
		close();
		OW_THROW(OW_SocketException, format("OW_ServerSocketImpl: %1",
				strerror(errno)).c_str());
	}


	if(listen(m_sockfd, queueSize) == -1)
	{
		close();
		OW_THROW(OW_SocketException, format("OW_ServerSocketImpl: %1",
			strerror(errno)).c_str());
	}

	fillAddrParms();
	m_isActive = true;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ServerSocketImpl::doListen(const OW_String& filename, int queueSize)
{
	m_localAddress = OW_SocketAddress::getUDS(filename);
	close();

	if((m_sockfd = ::socket(AF_LOCAL,SOCK_STREAM, 0)) == -1)
	{
		OW_THROW(OW_SocketException, format("OW_ServerSocketImpl: %1",
			strerror(errno)).c_str());
	}

	// set listen socket to nonblocking; see Unix Network Programming,
	// pages 422-424.
	int fdflags = ::fcntl(m_sockfd, F_GETFL, 0);
	::fcntl(m_sockfd, F_SETFL, fdflags | O_NONBLOCK);

//#ifdef OW_DEBUG // TODO is this safe?
	// Let the kernel reuse the port without waiting for it to time out.
	int reuse = 1;
	::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
//#endif

    if (OW_FileSystem::exists(filename))
    {
        if (::unlink(filename.c_str()) != 0)
        {
            OW_THROW(OW_SocketException, 
                format("Unable to unlink Unix Domain Socket: %1, errno: %2", 
                    filename, errno).c_str());
        }
    }
		
	if(bind(m_sockfd, m_localAddress.getNativeForm(), 
		m_localAddress.getNativeFormSize()) == -1)
	{
		close();
		OW_THROW(OW_SocketException, format("OW_ServerSocketImpl: %1",
				strerror(errno)).c_str());
	}


	if(listen(m_sockfd, queueSize) == -1)
	{
		close();
		OW_THROW(OW_SocketException, format("OW_ServerSocketImpl: %1",
			strerror(errno)).c_str());
	}

	fillAddrParms();
	m_isActive = true;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_ServerSocketImpl::waitForIO(int fd, int timeOutSecs, OW_Bool forInput)
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

	if(forInput)
	{
		preadfds = &thefds;
	}
	else
	{
		pwritefds = &thefds;
	}

#ifdef OW_USE_GNU_PTH
	if((rc = ::pth_select(fd+1, preadfds, pwritefds, NULL, ptimeval)) == -1)
#else
	if((rc = ::select(fd+1, preadfds, pwritefds, NULL, ptimeval)) == -1)
#endif
	{
		OW_THROW(OW_SocketException, "waitForIO: select");
	}

	return (rc > 0);
}


//////////////////////////////////////////////////////////////////////////////
/*
OW_String
OW_ServerSocketImpl::addrString()
{
	return inetAddrToString(m_localAddress, m_localPort);
}
*/
//////////////////////////////////////////////////////////////////////////////
OW_Socket
OW_ServerSocketImpl::accept(int timeoutSecs)
	/*throw (OW_SocketException, OW_TimeOutException)*/
{
	if(!m_isActive)
	{
		OW_THROW(OW_SocketException, "OW_ServerSocketImpl::accept: NONE");
	}

	if(waitForIO(m_sockfd, timeoutSecs, true))
	{
		int clntfd;
		socklen_t clntlen;
		struct sockaddr_in clntInetAddr;
		struct sockaddr_un clntUnixAddr;
		struct sockaddr* pSA;
		if (m_localAddress.getType() == OW_SocketAddress::INET)
		{
			pSA = (struct sockaddr*)&clntInetAddr;
			clntlen = sizeof(clntInetAddr);
		}
		else if (m_localAddress.getType() == OW_SocketAddress::UDS)
		{
			pSA = (struct sockaddr*)&clntUnixAddr;
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
		// check to see if client aborts connection between select and accept.
		// see Unix Network Programming pages 422-424.
		// TODO should we just check for one? eg. the proper one for
		if (errno == EWOULDBLOCK
			 || errno == ECONNABORTED
			 || errno == EPROTO)
		{
			OW_THROW(OW_SocketException, "Client aborted TCP connection "
				"between select() and accept()");
		}
		
		if(clntfd < 0)
		{
			OW_THROW(OW_SocketException, "OW_ServerSocketImpl::accept");
		}

		// set socket back to blocking; see Unix Network Programming,
		// pages 422-424.
		int fdflags = ::fcntl(clntfd, F_GETFL, 0);
		// TODO we never get inside this if block.  Is the man page flawed
		// when it claims that NONBLOCK is inherited from the listen socket
		// though accept()? or is my logic flawed below?
		if ((fdflags & O_NONBLOCK) == O_NONBLOCK)
		{
			::fcntl(clntfd, F_SETFL, fdflags ^ O_NONBLOCK);
		}
		return OW_Socket(clntfd, m_localAddress.getType(), m_isSSL);
	}
	else
	{
		// The timeout expired.
		OW_THROW(OW_TimeOutException,"Timed out waiting for a connection");
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ServerSocketImpl::close() /*throw (OW_SocketException)*/
{
	if(m_isActive)
	{
		::close(m_sockfd);
		m_isActive = false;
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_ServerSocketImpl::fillAddrParms() /*throw (OW_SocketException)*/
{
	socklen_t len;
	if (m_localAddress.getType() == OW_SocketAddress::INET)
	{
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));

		len = sizeof(addr);
		if(getsockname(m_sockfd, (struct sockaddr*) &addr, &len) == -1)
		{
			OW_THROW(OW_SocketException, "OW_SocketImpl::fillAddrParms: "
				"getsockname");
		}

		m_localAddress.assignFromNativeForm(&addr, len);
	}
	else if (m_localAddress.getType() == OW_SocketAddress::UDS)
	{
		struct sockaddr_un addr;
		memset(&addr, 0, sizeof(addr));

		len = sizeof(addr);
		if(getsockname(m_sockfd, (struct sockaddr*) &addr, &len) == -1)
		{
			OW_THROW(OW_SocketException, "OW_SocketImpl::fillAddrParms: "
				"getsockname");
		}

		m_localAddress.assignFromNativeForm(&addr, len);
	}
	else
	{
		OW_ASSERT(0);
	}
}

