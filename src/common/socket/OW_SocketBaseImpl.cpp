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
#include "OW_SocketBaseImpl.hpp"
#include "OW_SocketUtils.hpp"
#include "OW_Format.hpp"
#include "OW_Assertion.hpp"

extern "C"
{
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#ifdef OW_USE_GNU_PTH
#include <pth.h>
#endif
}

#include <fstream>

using std::istream;
using std::ostream;
using std::iostream;
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::ios;

OW_String OW_SocketBaseImpl::m_traceFileOut = "";
OW_String OW_SocketBaseImpl::m_traceFileIn = "";

//////////////////////////////////////////////////////////////////////////////
OW_SocketBaseImpl::OW_SocketBaseImpl()
	: OW_SelectableIFC()
	, OW_IOIFC()
	, m_isConnected(false)
	, m_sockfd(-1)
	, m_localAddress()
	, m_peerAddress()
	, m_recvTimeoutExprd(false)
	, m_streamBuf(this)
	, m_in(&m_streamBuf)
	, m_out(&m_streamBuf)
	, m_inout(&m_streamBuf)
	, m_recvTimeout(-1)
	, m_sendTimeout(-1)
	, m_connectTimeout(0)
{
	m_in.tie(&m_out);
}

//////////////////////////////////////////////////////////////////////////////
OW_SocketBaseImpl::OW_SocketBaseImpl(OW_SocketHandle_t fd, 
		OW_SocketAddress::AddressType addrType)
	: OW_SelectableIFC()
	, OW_IOIFC()
	, m_isConnected(true)
	, m_sockfd(fd)
	, m_localAddress(OW_SocketAddress::getAnyLocalHost())
	, m_peerAddress(OW_SocketAddress::allocEmptyAddress())
	, m_recvTimeoutExprd(false)
	, m_streamBuf(this)
	, m_in(&m_streamBuf)
	, m_out(&m_streamBuf)
	, m_inout(&m_streamBuf)
	, m_recvTimeout(-1)
	, m_sendTimeout(-1)
	, m_connectTimeout(0)
{
	m_in.tie(&m_out);
	if (addrType == OW_SocketAddress::INET)
	{
		fillInetAddrParms();
	}
	else if (addrType == OW_SocketAddress::UDS)
	{
		fillUnixAddrParms();
	}
	else
	{
		OW_ASSERT(0);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_SocketBaseImpl::OW_SocketBaseImpl(const OW_SocketAddress& addr)
	: OW_SelectableIFC()
	, OW_IOIFC()
	, m_isConnected(false)
	, m_sockfd(-1)
	, m_localAddress(OW_SocketAddress::getAnyLocalHost())
	, m_peerAddress(addr)
	, m_recvTimeoutExprd(false)
	, m_streamBuf(this)
	, m_in(&m_streamBuf)
	, m_out(&m_streamBuf)
	, m_inout(&m_streamBuf)
	, m_recvTimeout(-1)
	, m_sendTimeout(-1)
	, m_connectTimeout(0)
{
	m_in.tie(&m_out);
	connect(m_peerAddress);
}
//////////////////////////////////////////////////////////////////////////////
OW_SocketBaseImpl::~OW_SocketBaseImpl()
{
	disconnect();
}

//////////////////////////////////////////////////////////////////////////////
OW_Select_t
OW_SocketBaseImpl::getSelectObj() const
{
	return m_sockfd;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_SocketBaseImpl::connect(const OW_SocketAddress& addr)
{
	if(m_isConnected)
	{
		disconnect();
	}
	m_streamBuf.reset();
	m_in.clear();
	m_out.clear();
	m_inout.clear();

	OW_ASSERT(addr.getType() == OW_SocketAddress::INET 
			|| addr.getType() == OW_SocketAddress::UDS);

	if((m_sockfd = ::socket(addr.getType() == OW_SocketAddress::INET ?
		AF_INET : AF_LOCAL, SOCK_STREAM, 0)) == -1)
	{
		OW_THROW(OW_SocketException,
			format("Failed to create a socket: %1", strerror(errno)).c_str());
	}

	int lerrno;

	if(m_connectTimeout > 0)
	{
		int n;
		int flags = ::fcntl(m_sockfd, F_GETFL, 0);
		::fcntl(m_sockfd, F_SETFL, flags | O_NONBLOCK);

#ifdef OW_USE_GNU_PTH
		if((n = ::pth_connect(m_sockfd, addr.getNativeForm(),
						addr.getNativeFormSize())) < 0)
#else
		if((n = ::connect(m_sockfd, addr.getNativeForm(),
						addr.getNativeFormSize())) < 0)
#endif
		{
			if(errno != EINPROGRESS)
			{
				lerrno = errno;
				::close(m_sockfd);
				OW_THROW(OW_SocketException,
					format("Failed to connect to: %1", addr.getAddress()).c_str());
			}
		}

		if(n != 0)
		{
			fd_set rset, wset;
			struct timeval tval;

			FD_ZERO(&rset);
			FD_SET(m_sockfd, &rset);
			wset = rset;
			tval.tv_sec = m_connectTimeout;
			tval.tv_usec = 0;
#ifdef OW_USE_GNU_PTH
			if((n = ::pth_select(m_sockfd+1, &rset, &wset, NULL, &tval)) == 0)
#else
			if((n = ::select(m_sockfd+1, &rset, &wset, NULL, &tval)) == 0)
#endif
			{
				::close(m_sockfd);
				OW_THROW(OW_SocketException, "OW_SocketBaseImpl::connect");
			}

			if(FD_ISSET(m_sockfd, &rset) || FD_ISSET(m_sockfd, &wset))
			{
				int error;
				socklen_t len = sizeof(error);

				if(::getsockopt(m_sockfd, SOL_SOCKET, SO_ERROR, &error,
							&len) < 0)
				{
					::close(m_sockfd);
					OW_THROW(OW_SocketException,
							"OW_SocketBaseImpl::connect");
				}
			}
			else
			{
				::close(m_sockfd);
				OW_THROW(OW_SocketException, "OW_SocketBaseImpl::connect");
			}
		}

   	    ::fcntl(m_sockfd, F_SETFL, flags);
	}
	else
	{
#ifdef OW_USE_GNU_PTH
		if(::pth_connect(m_sockfd, addr.getNativeForm(),
					addr.getNativeFormSize()) == -1)
#else
		if(::connect(m_sockfd, addr.getNativeForm(),
					addr.getNativeFormSize()) == -1)
#endif
		{
			lerrno = errno;
			::close(m_sockfd);
			OW_THROW(OW_SocketException,
				format("Failed to connect to: %1", addr.toString()).c_str());
		}
	}

	m_isConnected = true;
	if (addr.getType() == OW_SocketAddress::INET)
	{
		fillInetAddrParms();
	}
	else if (addr.getType() == OW_SocketAddress::UDS)
	{
		fillUnixAddrParms();
	}
	else
	{
		OW_ASSERT(0);
	}
}


//////////////////////////////////////////////////////////////////////////////
void
OW_SocketBaseImpl::disconnect()
{
	if (m_in)
	{
		m_in.clear(ios::eofbit);
	}
	if (m_out)
	{
		m_out.clear(ios::eofbit);
	}
	if (m_inout)
	{
		m_inout.clear(ios::eofbit);
	}
	if(m_sockfd != -1 && m_isConnected)
	{
		::close(m_sockfd);
		m_isConnected = false;
		m_sockfd = -1;
	}
}

//////////////////////////////////////////////////////////////////////////////
#if 0
/*
void
OW_SocketBaseImpl::fillAddrParms() //throw (OW_SocketException)
{
	socklen_t len;
	struct sockaddr_in addr;

	len = sizeof(addr);
	if(getsockname(m_sockfd, (struct sockaddr*) &addr, &len) == -1)
		OW_THROW(OW_SocketException,
				"OW_SocketBaseImpl::fillAddrParms: getsockname");

	m_localAddress = addr.sin_addr.s_addr;
	m_localPort = addr.sin_port;

	len = sizeof(addr);
	if(getpeername(m_sockfd, (struct sockaddr*) &addr, &len) == -1)
		OW_THROW(OW_SocketException,
				"OW_SocketBaseImpl::fillAddrParms: getpeername");

	m_peerAddress = addr.sin_addr.s_addr;
	m_peerPort = addr.sin_port;
}
*/
#endif
//////////////////////////////////////////////////////////////////////////////
// JBW this needs reworked.
void
OW_SocketBaseImpl::fillInetAddrParms() /*throw (OW_SocketException)*/
{
	socklen_t len;
	OW_InetSocketAddress_t addr;
	memset(&addr, 0, sizeof(addr));

	len = sizeof(addr);
	if(getsockname(m_sockfd, (struct sockaddr*) &addr, &len) == -1)
	{
		OW_THROW(OW_SocketException,
				"OW_SocketBaseImpl::fillInetAddrParms: getsockname");
	}
	m_localAddress.assignFromNativeForm(&addr, len);

	len = sizeof(addr);
	if(getpeername(m_sockfd, (struct sockaddr*) &addr, &len) == -1)
	{
		OW_THROW(OW_SocketException,
				"OW_SocketBaseImpl::fillInetAddrParms: getpeername");
	}

	m_peerAddress.assignFromNativeForm(&addr, len);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_SocketBaseImpl::fillUnixAddrParms() /*throw (OW_SocketException)*/
{
	socklen_t len;
	OW_UnixSocketAddress_t addr;
	memset(&addr, 0, sizeof(addr));

	len = sizeof(addr);
	if(getsockname(m_sockfd, (struct sockaddr*) &addr, &len) == -1)
	{
		OW_THROW(OW_SocketException,
				"OW_SocketBaseImpl::fillUnixAddrParms: getsockname");
	}
	m_localAddress.assignFromNativeForm(&addr, len);
	m_peerAddress.assignFromNativeForm(&addr, len);
}

//////////////////////////////////////////////////////////////////////////////
int
OW_SocketBaseImpl::write(const void* dataOut, int dataOutLen, OW_Bool errorAsException)
	/*throw (OW_SocketException)*/
{
	int rc = 0;
	OW_Bool isError = false;

	if(m_isConnected)
	{
		isError = waitForOutput(m_sendTimeout);

		if(isError)
		{
			rc = -1;
		}
		else
		{
			rc = writeAux(dataOut, dataOutLen);
			if(m_traceFileOut.length() > 0)
			{
				ofstream traceFile(m_traceFileOut.c_str(), std::ios::app);
				traceFile.write((const char*)dataOut, rc);
			}
		}
	}
	else
	{
		rc = -1;
	}

	if(rc < 0)
	{
		if (m_isConnected)
		{
			disconnect();
		}
		if(errorAsException)
			OW_THROW(OW_SocketException, "OW_SocketBaseImpl::write");
	}

	return rc;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_SocketBaseImpl::read(void* dataIn, int dataInLen, OW_Bool errorAsException) 	
	/*throw (OW_SocketException)*/
{
	int rc = 0;
	OW_Bool isError = false;

	if(m_isConnected)
	{
		isError = waitForInput(m_recvTimeout);

		if(isError)
		{
			rc = -1;
		}
		else
		{
			rc = readAux(dataIn, dataInLen);
			if(m_traceFileIn.length() > 0)
			{
				ofstream traceFile(m_traceFileIn.c_str(), std::ios::app);
				traceFile.write((const char*)dataIn, rc);
			}
		}
	}
	else
	{
		rc = -1;
	}

	if(rc < 0)
	{
		if(errorAsException)
			OW_THROW(OW_SocketException, "OW_SocketBaseImpl::read");
	}

	return rc;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_SocketBaseImpl::waitForInput(int timeOutSecs) /*throw (OW_SocketException)*/
{
	int rval = OW_SocketUtils::waitForIO(m_sockfd, timeOutSecs, true);
	if (rval == ETIMEDOUT)
	{
		m_recvTimeoutExprd = true;
	}
	return (rval != 0);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_SocketBaseImpl::waitForOutput(int timeOutSecs) /*throw (OW_SocketException)*/
{
	return OW_SocketUtils::waitForIO(m_sockfd, timeOutSecs, false) != 0;
}

//////////////////////////////////////////////////////////////////////////////
istream&
OW_SocketBaseImpl::getInputStream() /*throw (OW_SocketException)*/
{
	return m_in;
}

//////////////////////////////////////////////////////////////////////////////
ostream&
OW_SocketBaseImpl::getOutputStream() /*throw (OW_SocketException)*/
{
	return m_out;
}

//////////////////////////////////////////////////////////////////////////////
iostream&
OW_SocketBaseImpl::getIOStream() /*throw (OW_SocketException)*/
{
	return m_inout;
}


//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_SocketBaseImpl::setDumpFiles(const OW_String& in, const OW_String& out)
{
	m_traceFileOut = out;
	m_traceFileIn = in;
}
