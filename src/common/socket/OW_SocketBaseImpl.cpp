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
#include "OW_SocketBaseImpl.hpp"
#include "OW_SocketUtils.hpp"
#include "OW_Format.hpp"
#include "OW_Assertion.hpp"
#include "OW_IOException.hpp"
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include "OW_PosixUnnamedPipe.hpp"
#include "OW_Socket.hpp"

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

OW_String OW_SocketBaseImpl::m_traceFileOut;
OW_String OW_SocketBaseImpl::m_traceFileIn;

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
	m_out.exceptions(std::ios::badbit);
	m_inout.exceptions(std::ios::badbit);
}

//////////////////////////////////////////////////////////////////////////////
OW_SocketBaseImpl::OW_SocketBaseImpl(OW_SocketHandle_t fd,
		OW_SocketAddress::AddressType addrType)
	: OW_SelectableIFC()
	, OW_IOIFC()
	, m_isConnected(true)
	, m_sockfd(fd)
	, m_localAddress(OW_SocketAddress::getAnyLocalHost())
	, m_peerAddress(OW_SocketAddress::allocEmptyAddress(addrType))
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
	m_out.exceptions(std::ios::badbit);
	m_inout.exceptions(std::ios::badbit);
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
	m_out.exceptions(std::ios::badbit);
	m_inout.exceptions(std::ios::badbit);
	connect(m_peerAddress);
}
//////////////////////////////////////////////////////////////////////////////
OW_SocketBaseImpl::~OW_SocketBaseImpl()
{
	try
	{
		disconnect();
	}
	catch (...)
	{
		// don't let exceptions escape
	}
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
		AF_INET : PF_UNIX, SOCK_STREAM, 0)) == -1)
	{
		OW_THROW(OW_SocketException,
			format("Failed to create a socket: %1", strerror(errno)).c_str());
	}

	// set the close on exec flag so child process can't keep the socket.
	if (::fcntl(m_sockfd, F_SETFD, FD_CLOEXEC) == -1)
	{
		::close(m_sockfd);
		OW_THROW(OW_SocketException, format("OW_SocketBaseImpl::connect() failed to set "
			"close-on-exec flag on socket: %1",
			strerror(errno)).c_str());
	}

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
			::close(m_sockfd);
			OW_THROW(OW_SocketException,
				format("Failed to connect to: %1: %2(%3)", addr.getAddress(), errno, strerror(errno)).c_str());
		}
	}

	if(n != 0)
	{
		OW_PosixUnnamedPipeRef lUPipe;

		int pipefd = 0;

		if (OW_Socket::m_pUpipe)
		{
			lUPipe = OW_Socket::m_pUpipe.cast_to<OW_PosixUnnamedPipe>();
			OW_ASSERT(lUPipe);
			pipefd = lUPipe->getInputHandle();

		}

		fd_set rset, wset;
		struct timeval tval;

		FD_ZERO(&rset);
		FD_SET(m_sockfd, &rset);
		FD_SET(pipefd, &rset);

		FD_ZERO(&wset);
		FD_SET(m_sockfd, &wset);

		struct timeval* ptval = 0;
		if (m_connectTimeout > 0)
		{
			tval.tv_sec = m_connectTimeout;
			tval.tv_usec = 0;
			ptval = &tval;
		}

		int maxfd = m_sockfd > pipefd ? m_sockfd : pipefd;
#ifdef OW_USE_GNU_PTH
		if((n = ::pth_select(maxfd+1, &rset, &wset, NULL, ptval)) == 0)
#else
		if((n = ::select(maxfd+1, &rset, &wset, NULL, ptval)) == 0)
#endif
		{
			::close(m_sockfd);
			OW_THROW(OW_SocketException, "OW_SocketBaseImpl::connect");
		}

		if(FD_ISSET(pipefd, &rset))
		{
			OW_THROW(OW_SocketException, "Sockets have been shutdown");
		}
		else if(FD_ISSET(m_sockfd, &rset) || FD_ISSET(m_sockfd, &wset))
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
// JBW this needs reworked.
void
OW_SocketBaseImpl::fillInetAddrParms() /*throw (OW_SocketException)*/
{
	socklen_t len;
	OW_InetSocketAddress_t addr;
	memset(&addr, 0, sizeof(addr));

	len = sizeof(addr);
	if(getsockname(m_sockfd, reinterpret_cast<struct sockaddr*>(&addr), &len) == -1)
	{
		OW_THROW(OW_SocketException,
				format("OW_SocketBaseImpl::fillInetAddrParms: getsockname: %1(%2)", errno, strerror(errno)).c_str());
	}
	m_localAddress.assignFromNativeForm(&addr, len);

	len = sizeof(addr);
	if(getpeername(m_sockfd, reinterpret_cast<struct sockaddr*>(&addr), &len) == -1)
	{
		OW_THROW(OW_SocketException,
				format("OW_SocketBaseImpl::fillInetAddrParms: getpeername: %1(%2)", errno, strerror(errno)).c_str());
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
	if(getsockname(m_sockfd, reinterpret_cast<struct sockaddr*>(&addr), &len) == -1)
	{
		OW_THROW(OW_SocketException,
				"OW_SocketBaseImpl::fillUnixAddrParms: getsockname");
	}
	m_localAddress.assignFromNativeForm(&addr, len);
	m_peerAddress.assignFromNativeForm(&addr, len);
}

static OW_Mutex guard;

//////////////////////////////////////////////////////////////////////////////
int
OW_SocketBaseImpl::write(const void* dataOut, int dataOutLen, bool errorAsException)
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
			if(!m_traceFileOut.empty() && rc > 0)
			{
				OW_MutexLock ml(guard);
				ofstream traceFile(m_traceFileOut.c_str(), std::ios::app);
				if (!traceFile)
				{
					OW_THROW(OW_IOException, "Failed opening socket dump file");
				}
				if (!traceFile.write(static_cast<const char*>(dataOut), rc))
				{
					OW_THROW(OW_IOException, "Failed writing to socket dump");
				}
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
OW_SocketBaseImpl::read(void* dataIn, int dataInLen, bool errorAsException) 	
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
			if(!m_traceFileIn.empty() && rc > 0)
			{
				OW_MutexLock ml(guard);
				ofstream traceFile(m_traceFileIn.c_str(), std::ios::app);
				if (!traceFile)
				{
					OW_THROW(OW_IOException, "Failed opening tracefile");
				}
				if (!traceFile.write(reinterpret_cast<const char*>(dataIn), rc))
				{
					OW_THROW(OW_IOException, "Failed writing to socket dump");
				}
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
