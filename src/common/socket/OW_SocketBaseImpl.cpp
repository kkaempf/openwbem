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

#if !defined(OW_WIN32)

#include "OW_SocketBaseImpl.hpp"
#include "OW_SocketUtils.hpp"
#include "OW_Format.hpp"
#include "OW_Assertion.hpp"
#include "OW_IOException.hpp"
#include "OW_Mutex.hpp"
#include "OW_MutexLock.hpp"
#include "OW_PosixUnnamedPipe.hpp"
#include "OW_Socket.hpp"
#include "OW_Thread.hpp"
extern "C"
{
#ifdef OW_HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

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
}
#include <fstream>

namespace OpenWBEM
{

using std::istream;
using std::ostream;
using std::iostream;
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::ios;
String SocketBaseImpl::m_traceFileOut;
String SocketBaseImpl::m_traceFileIn;
//////////////////////////////////////////////////////////////////////////////
SocketBaseImpl::SocketBaseImpl()
	: SelectableIFC()
	, IOIFC()
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
	m_out.exceptions(std::ios::badbit);
	m_inout.exceptions(std::ios::badbit);
}
//////////////////////////////////////////////////////////////////////////////
SocketBaseImpl::SocketBaseImpl(SocketHandle_t fd,
		SocketAddress::AddressType addrType)
	: SelectableIFC()
	, IOIFC()
	, m_isConnected(true)
	, m_sockfd(fd)
	, m_localAddress(SocketAddress::getAnyLocalHost())
	, m_peerAddress(SocketAddress::allocEmptyAddress(addrType))
	, m_recvTimeoutExprd(false)
	, m_streamBuf(this)
	, m_in(&m_streamBuf)
	, m_out(&m_streamBuf)
	, m_inout(&m_streamBuf)
	, m_recvTimeout(-1)
	, m_sendTimeout(-1)
	, m_connectTimeout(0)
{
	m_out.exceptions(std::ios::badbit);
	m_inout.exceptions(std::ios::badbit);
	if (addrType == SocketAddress::INET)
	{
		fillInetAddrParms();
	}
	else if (addrType == SocketAddress::UDS)
	{
		fillUnixAddrParms();
	}
	else
	{
		OW_ASSERT(0);
	}
}
//////////////////////////////////////////////////////////////////////////////
SocketBaseImpl::SocketBaseImpl(const SocketAddress& addr)
	: SelectableIFC()
	, IOIFC()
	, m_isConnected(false)
	, m_sockfd(-1)
	, m_localAddress(SocketAddress::getAnyLocalHost())
	, m_peerAddress(addr)
	, m_recvTimeoutExprd(false)
	, m_streamBuf(this)
	, m_in(&m_streamBuf)
	, m_out(&m_streamBuf)
	, m_inout(&m_streamBuf)
	, m_recvTimeout(-1)
	, m_sendTimeout(-1)
	, m_connectTimeout(-1)
{
	m_out.exceptions(std::ios::badbit);
	m_inout.exceptions(std::ios::badbit);
	connect(m_peerAddress);
}
//////////////////////////////////////////////////////////////////////////////
SocketBaseImpl::~SocketBaseImpl()
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
Select_t
SocketBaseImpl::getSelectObj() const
{
	return m_sockfd;
}
//////////////////////////////////////////////////////////////////////////////
void
SocketBaseImpl::connect(const SocketAddress& addr)
{
	if (m_isConnected)
	{
		disconnect();
	}
	m_streamBuf.reset();
	m_in.clear();
	m_out.clear();
	m_inout.clear();
	OW_ASSERT(addr.getType() == SocketAddress::INET
			|| addr.getType() == SocketAddress::UDS);
	if ((m_sockfd = ::socket(addr.getType() == SocketAddress::INET ?
		AF_INET : PF_UNIX, SOCK_STREAM, 0)) == -1)
	{
		OW_THROW(SocketException,
			Format("Failed to create a socket: %1", strerror(errno)).c_str());
	}
	// set the close on exec flag so child process can't keep the socket.
	if (::fcntl(m_sockfd, F_SETFD, FD_CLOEXEC) == -1)
	{
		::close(m_sockfd);
		OW_THROW(SocketException, Format("SocketBaseImpl::connect() failed to set "
			"close-on-exec flag on socket: %1",
			strerror(errno)).c_str());
	}
	int n;
	int flags = ::fcntl(m_sockfd, F_GETFL, 0);
	::fcntl(m_sockfd, F_SETFL, flags | O_NONBLOCK);
	if ((n = ::connect(m_sockfd, addr.getNativeForm(),
					addr.getNativeFormSize())) < 0)
	{
		if (errno != EINPROGRESS)
		{
			::close(m_sockfd);
			OW_THROW(SocketException,
				Format("Failed to connect to: %1: %2(%3)", addr.toString(), errno, strerror(errno)).c_str());
		}
	}
	if (n == -1) 
	{
		// because of the above check for EINPROGRESS
		// not connected yet, need to select and wait for connection to complete.
		PosixUnnamedPipeRef lUPipe;
		int pipefd = -1;
		if (Socket::getShutDownMechanism())
		{
			UnnamedPipeRef foo = Socket::getShutDownMechanism();
			lUPipe = foo.cast_to<PosixUnnamedPipe>();
			OW_ASSERT(lUPipe);
			pipefd = lUPipe->getInputHandle();
		}
		fd_set rset, wset;
		// here we spin checking for thread cancellation every so often.
		UInt32 remainingMsWait = m_connectTimeout != -1 ? m_connectTimeout * 1000 : ~0U;
		do
		{
			FD_ZERO(&rset);
			FD_SET(m_sockfd, &rset);
			if (pipefd != -1)
			{
				FD_SET(pipefd, &rset);
			}
			FD_ZERO(&wset);
			FD_SET(m_sockfd, &wset);
			int maxfd = m_sockfd > pipefd ? m_sockfd : pipefd;

			const UInt32 waitMs = 100; // 1/10 of a second
			struct timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = std::min((waitMs % 1000) * 1000, remainingMsWait);

			Thread::testCancel();
			n = ::select(maxfd+1, &rset, &wset, NULL, &tv);

			if (m_connectTimeout != -1)
			{
				remainingMsWait -= std::min(waitMs, remainingMsWait);
			}
		} while (n == 0 && remainingMsWait > 0);

		if (n == 0)
		{
			::close(m_sockfd);
			OW_THROW(SocketException, "SocketBaseImpl::connect() select timedout");
		}
		else if (n == -1)
		{
			::close(m_sockfd);
			if (errno == EINTR)
			{
				Thread::testCancel();
			}
			OW_THROW(SocketException, Format("SocketBaseImpl::connect() select failed: %1(%2)", errno, strerror(errno)).c_str());
		}
		if (pipefd != -1 && FD_ISSET(pipefd, &rset))
		{
			::close(m_sockfd);
			OW_THROW(SocketException, "Sockets have been shutdown");
		}
		else if (FD_ISSET(m_sockfd, &rset) || FD_ISSET(m_sockfd, &wset))
		{
			int error;
			socklen_t len = sizeof(error);
			if (::getsockopt(m_sockfd, SOL_SOCKET, SO_ERROR, &error,
						&len) < 0)
			{
				::close(m_sockfd);
				OW_THROW(SocketException,
						Format("SocketBaseImpl::connect() getsockopt() failed: %1(%2)", errno, strerror(errno)).c_str());
			}
			if (error != 0)
			{
				::close(m_sockfd);
				OW_THROW(SocketException,
						Format("SocketBaseImpl::connect() failed: %1(%2)", error, strerror(error)).c_str());
			}
		}
		else
		{
			::close(m_sockfd);
			OW_THROW(SocketException, "SocketBaseImpl::connect(). Logic error, m_sockfd not in FD set.");
		}
	}
	::fcntl(m_sockfd, F_SETFL, flags);
	m_isConnected = true;
	if (addr.getType() == SocketAddress::INET)
	{
		fillInetAddrParms();
	}
	else if (addr.getType() == SocketAddress::UDS)
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
SocketBaseImpl::disconnect()
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
	if (m_sockfd != -1 && m_isConnected)
	{
		::close(m_sockfd);
		m_isConnected = false;
		m_sockfd = -1;
	}
}
//////////////////////////////////////////////////////////////////////////////
// JBW this needs reworked.
void
SocketBaseImpl::fillInetAddrParms()
{
	socklen_t len;
	InetSocketAddress_t addr;
	memset(&addr, 0, sizeof(addr));
	len = sizeof(addr);
	if (getsockname(m_sockfd, reinterpret_cast<struct sockaddr*>(&addr), &len) == -1)
	{
// Don't error out here, we can still operate without working DNS.
//		OW_THROW(SocketException,
//				Format("SocketBaseImpl::fillInetAddrParms: getsockname: %1(%2)", errno, strerror(errno)).c_str());
	}
	else
	{
		m_localAddress.assignFromNativeForm(&addr, len);
	}
	len = sizeof(addr);
	if (getpeername(m_sockfd, reinterpret_cast<struct sockaddr*>(&addr), &len) == -1)
	{
// Don't error out here, we can still operate without working DNS.
//		OW_THROW(SocketException,
//				Format("SocketBaseImpl::fillInetAddrParms: getpeername: %1(%2)", errno, strerror(errno)).c_str());
	}
	else
	{
		m_peerAddress.assignFromNativeForm(&addr, len);
	}
}
//////////////////////////////////////////////////////////////////////////////
void
SocketBaseImpl::fillUnixAddrParms()
{
	socklen_t len;
	UnixSocketAddress_t addr;
	memset(&addr, 0, sizeof(addr));
	len = sizeof(addr);
	if (getsockname(m_sockfd, reinterpret_cast<struct sockaddr*>(&addr), &len) == -1)
	{
		OW_THROW(SocketException,
				"SocketBaseImpl::fillUnixAddrParms: getsockname");
	}
	m_localAddress.assignFromNativeForm(&addr, len);
	m_peerAddress.assignFromNativeForm(&addr, len);
}
static Mutex guard;
//////////////////////////////////////////////////////////////////////////////
int
SocketBaseImpl::write(const void* dataOut, int dataOutLen, bool errorAsException)
{
	int rc = 0;
	bool isError = false;
	if (m_isConnected)
	{
		isError = waitForOutput(m_sendTimeout);
		if (isError)
		{
			rc = -1;
		}
		else
		{
			rc = writeAux(dataOut, dataOutLen);
			if (!m_traceFileOut.empty() && rc > 0)
			{
				MutexLock ml(guard);
				ofstream traceFile(m_traceFileOut.c_str(), std::ios::app);
				if (!traceFile)
				{
					OW_THROW(IOException, "Failed opening socket dump file");
				}
				if (!traceFile.write(static_cast<const char*>(dataOut), rc))
				{
					OW_THROW(IOException, "Failed writing to socket dump");
				}

				ofstream comboTraceFile(String(m_traceFileOut + "Combo").c_str(), std::ios::app);
				if (!comboTraceFile)
				{
					OW_THROW(IOException, "Failed opening socket dump file");
				}
				comboTraceFile << "\n--->Out " << rc << " bytes<---\n";
				if (!comboTraceFile.write(static_cast<const char*>(dataOut), rc))
				{
					OW_THROW(IOException, "Failed writing to socket dump");
				}
			}
		}
	}
	else
	{
		rc = -1;
	}
	if (rc < 0 && errorAsException)
	{
		OW_THROW(SocketException, "SocketBaseImpl::write");
	}
	return rc;
}
//////////////////////////////////////////////////////////////////////////////
int
SocketBaseImpl::read(void* dataIn, int dataInLen, bool errorAsException) 	
{
	int rc = 0;
	bool isError = false;
	if (m_isConnected)
	{
		isError = waitForInput(m_recvTimeout);
		if (isError)
		{
			rc = -1;
		}
		else
		{
			rc = readAux(dataIn, dataInLen);
			if (!m_traceFileIn.empty() && rc > 0)
			{
				MutexLock ml(guard);
				ofstream traceFile(m_traceFileIn.c_str(), std::ios::app);
				if (!traceFile)
				{
					OW_THROW(IOException, "Failed opening tracefile");
				}
				if (!traceFile.write(reinterpret_cast<const char*>(dataIn), rc))
				{
					OW_THROW(IOException, "Failed writing to socket dump");
				}

				ofstream comboTraceFile(String(m_traceFileOut + "Combo").c_str(), std::ios::app);
				if (!comboTraceFile)
				{
					OW_THROW(IOException, "Failed opening socket dump file");
				}
				comboTraceFile << "\n--->In " << rc << " bytes<---\n";
				if (!comboTraceFile.write(reinterpret_cast<const char*>(dataIn), rc))
				{
					OW_THROW(IOException, "Failed writing to socket dump");
				}
			}
		}
	}
	else
	{
		rc = -1;
	}
	if (rc < 0)
	{
		if (errorAsException)
		{
			OW_THROW(SocketException, "SocketBaseImpl::read");
		}
	}
	return rc;
}
//////////////////////////////////////////////////////////////////////////////
bool
SocketBaseImpl::waitForInput(int timeOutSecs)
{
	int rval = SocketUtils::waitForIO(m_sockfd, timeOutSecs, SocketFlags::E_WAIT_FOR_INPUT);
	if (rval == ETIMEDOUT)
	{
		m_recvTimeoutExprd = true;
	}
	return (rval != 0);
}
//////////////////////////////////////////////////////////////////////////////
bool
SocketBaseImpl::waitForOutput(int timeOutSecs)
{
	return SocketUtils::waitForIO(m_sockfd, timeOutSecs, SocketFlags::E_WAIT_FOR_OUTPUT) != 0;
}
//////////////////////////////////////////////////////////////////////////////
istream&
SocketBaseImpl::getInputStream()
{
	return m_in;
}
//////////////////////////////////////////////////////////////////////////////
ostream&
SocketBaseImpl::getOutputStream()
{
	return m_out;
}
//////////////////////////////////////////////////////////////////////////////
iostream&
SocketBaseImpl::getIOStream()
{
	return m_inout;
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
SocketBaseImpl::setDumpFiles(const String& in, const String& out)
{
	m_traceFileOut = out;
	m_traceFileIn = in;
}

} // end namespace OpenWBEM

#endif	// #if !defined(OW_WIN32)

