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
#include "OW_DateTime.hpp"
#include "OW_TimeoutTimer.hpp"
#include "OW_AutoDescriptor.hpp"
#include "OW_Logger.hpp"

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
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
}

#include <fstream>
#include <cerrno>
#include <cstdio>

namespace OW_NAMESPACE
{

using std::istream;
using std::ostream;
using std::iostream;
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::ios;

namespace
{
static Mutex g_guard;
}

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
	, m_recvTimeout(Timeout::infinite)
	, m_sendTimeout(Timeout::infinite)
	, m_connectTimeout(Timeout::infinite)
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
	, m_recvTimeout(Timeout::infinite)
	, m_sendTimeout(Timeout::infinite)
	, m_connectTimeout(Timeout::infinite)
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
	, m_recvTimeout(Timeout::infinite)
	, m_sendTimeout(Timeout::infinite)
	, m_connectTimeout(Timeout::infinite)
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
	OW_ASSERT(m_sockfd == -1);
	OW_ASSERT(addr.getType() == SocketAddress::INET || addr.getType() == SocketAddress::UDS);

	int domain_type = PF_UNIX;
	if( addr.getType() == SocketAddress::INET )
	{
		domain_type = PF_INET;
#ifdef OW_HAVE_IPV6
		// set PF_INET6 domain type for IPV6 protocol
		if( reinterpret_cast<const sockaddr*>(addr.getInetAddress())->sa_family == AF_INET6)
		{
			domain_type = PF_INET6;
		}
#endif
	}

	AutoDescriptor sockfd(::socket(domain_type, SOCK_STREAM, 0));
	if (sockfd.get() == -1)
	{
		OW_THROW_ERRNO_MSG(SocketException,
			"Failed to create a socket");
	}

	// set the close on exec flag so child process can't keep the socket.
	if (::fcntl(sockfd.get(), F_SETFD, FD_CLOEXEC) == -1)
	{
		OW_THROW_ERRNO_MSG(SocketException, "SocketBaseImpl::connect() failed to set close-on-exec flag on socket");
	}
	int n;
	int flags = ::fcntl(sockfd.get(), F_GETFL, 0);
	::fcntl(sockfd.get(), F_SETFL, flags | O_NONBLOCK);
	if ((n = ::connect(sockfd.get(), addr.getNativeForm(),
					addr.getNativeFormSize())) < 0)
	{
		if (errno != EINPROGRESS)
		{
			OW_THROW_ERRNO_MSG(SocketException,
				Format("Failed to connect to: %1", addr.toString()).c_str());
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
		TimeoutTimer timer(m_connectTimeout);
		timer.start();
		do
		{
			FD_ZERO(&rset);
			if (sockfd.get() < 0 || sockfd.get() >= FD_SETSIZE)
			{
				OW_THROW(SocketException, "Invalid fd (< 0 || >= FD_SETSIZE)");
			}
			FD_SET(sockfd.get(), &rset);
			if (pipefd != -1 && pipefd < FD_SETSIZE)
			{
				FD_SET(pipefd, &rset);
			}
			FD_ZERO(&wset);
			FD_SET(sockfd.get(), &wset);
			int maxfd = sockfd.get() > pipefd ? sockfd.get() : pipefd;

			Thread::testCancel();
			const float waitSec = 0.1;
			struct timeval tv;
			n = ::select(maxfd+1, &rset, &wset, NULL, timer.asTimeval(tv, waitSec));
			timer.loop();
		} while (n == 0 && !timer.expired());

		if (n == 0)
		{
			OW_THROW(SocketException, "SocketBaseImpl::connect() select timedout");
		}
		else if (n == -1)
		{
			if (errno == EINTR)
			{
				Thread::testCancel();
			}
			OW_THROW_ERRNO_MSG(SocketException, "SocketBaseImpl::connect() select failed");
		}
		if (pipefd != -1 && FD_ISSET(pipefd, &rset))
		{
			OW_THROW(SocketException, "Sockets have been shutdown");
		}
		else if (FD_ISSET(sockfd.get(), &rset) || FD_ISSET(sockfd.get(), &wset))
		{
			int error = 0;
			socklen_t len = sizeof(error);
			if (::getsockopt(sockfd.get(), SOL_SOCKET, SO_ERROR, &error,
						&len) < 0)
			{
				OW_THROW_ERRNO_MSG(SocketException,
						"SocketBaseImpl::connect() getsockopt() failed");
			}
			if (error != 0)
			{
				errno = error;
				OW_THROW_ERRNO_MSG(SocketException,
						"SocketBaseImpl::connect() failed");
			}
		}
		else
		{
			OW_THROW(SocketException, "SocketBaseImpl::connect(). Logic error, sockfd not in FD set.");
		}
	}
	::fcntl(sockfd.get(), F_SETFL, flags);
	m_sockfd = sockfd.release();
	m_isConnected = true;
	m_peerAddress = addr; // To get the hostname from addr
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
		if (::close(m_sockfd) == -1)
		{
			int lerrno = errno;
			Logger lgr("ow.common");
			OW_LOG_ERROR(lgr, Format("Closing socket handle %1 failed: %2", m_sockfd, lerrno));
		}
		m_isConnected = false;

		if (!m_traceFileIn.empty())
		{
			MutexLock ml(g_guard);

			String combofilename = m_traceFileOut + "Combo";
			ofstream comboTraceFile(combofilename.c_str(), std::ios::app);
			if (!comboTraceFile)
			{
				OW_THROW_ERRNO_MSG(IOException, Format("Failed opening socket dump file \"%1\"", combofilename));
			}
			DateTime curDateTime;
			curDateTime.setToCurrent();
			comboTraceFile << "\n--->fd: " << getfd() << " closed at " << curDateTime.toString("%X") <<
				'.' << curDateTime.getMicrosecond() << "<---\n";
		}

		m_sockfd = -1;
	}
}
//////////////////////////////////////////////////////////////////////////////
// JBW this needs reworked.
void
SocketBaseImpl::fillInetAddrParms()
{
	// create LocalAddress and PeerAddress structures for IPV6 protocol
	socklen_t len;
	struct sockaddr *p_addr;
	InetSocketAddress_t ss;
	memset(&ss, 0, sizeof(ss));
	len = sizeof(ss);
	p_addr = reinterpret_cast<struct sockaddr*>(&ss);
	if (getsockname(m_sockfd, p_addr, &len) != -1)
	{
	    m_localAddress.assignFromNativeForm(&ss, len);
	}
	memset(&ss, 0, sizeof(ss));
	len = sizeof(ss);
	if (getpeername(m_sockfd, p_addr, &len) != -1)
	{
	    m_peerAddress.assignFromNativeForm(&ss, len);
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
		OW_THROW_ERRNO_MSG(SocketException, "SocketBaseImpl::fillUnixAddrParms: getsockname");
	}
	m_localAddress.assignFromNativeForm(&addr, len);
	m_peerAddress.assignFromNativeForm(&addr, len);
}
//////////////////////////////////////////////////////////////////////////////
int
SocketBaseImpl::write(const void* dataOut, int dataOutLen, ErrorAction errorAsException)
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
				MutexLock ml(g_guard);
				ofstream traceFile(m_traceFileOut.c_str(), std::ios::app);
				if (!traceFile)
				{
					OW_THROW_ERRNO_MSG(IOException, Format("Failed opening socket dump file \"%1\"", m_traceFileOut));
				}
				if (!traceFile.write(static_cast<const char*>(dataOut), rc))
				{
					OW_THROW_ERRNO_MSG(IOException, "Failed writing to socket dump");
				}

				String combofilename = m_traceFileOut + "Combo";
				ofstream comboTraceFile(combofilename.c_str(), std::ios::app);
				if (!comboTraceFile)
				{
					OW_THROW_ERRNO_MSG(IOException, Format("Failed opening socket dump file \"%1\"", combofilename));
				}
				DateTime curDateTime;
				curDateTime.setToCurrent();
				comboTraceFile << "\n--->fd: " << getfd() << " Out " << rc << " bytes at " << curDateTime.toString("%X") <<
					'.' << curDateTime.getMicrosecond() << "<---\n";
				if (!comboTraceFile.write(static_cast<const char*>(dataOut), rc))
				{
					OW_THROW_ERRNO_MSG(IOException, "Failed writing to socket dump");
				}
			}
		}
	}
	else
	{
		rc = -1;
	}
	if (rc < 0 && errorAsException == E_THROW_ON_ERROR)
	{
		OW_THROW_ERRNO_MSG(SocketException, "SocketBaseImpl::write");
	}
	return rc;
}
//////////////////////////////////////////////////////////////////////////////
int
SocketBaseImpl::read(void* dataIn, int dataInLen, ErrorAction errorAsException)
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
				MutexLock ml(g_guard);
				ofstream traceFile(m_traceFileIn.c_str(), std::ios::app);
				if (!traceFile)
				{
					OW_THROW_ERRNO_MSG(IOException, Format("Failed opening tracefile \"%1\"", m_traceFileIn));
				}
				if (!traceFile.write(reinterpret_cast<const char*>(dataIn), rc))
				{
					OW_THROW_ERRNO_MSG(IOException, "Failed writing to socket dump");
				}

				String combofilename = m_traceFileOut + "Combo";
				ofstream comboTraceFile(combofilename.c_str(), std::ios::app);
				if (!comboTraceFile)
				{
					OW_THROW_ERRNO_MSG(IOException, Format("Failed opening socket dump file \"%1\"", combofilename));
				}
				DateTime curDateTime;
				curDateTime.setToCurrent();
				comboTraceFile << "\n--->fd: " << getfd() << " In " << rc << " bytes at " << curDateTime.toString("%X") <<
					'.' << curDateTime.getMicrosecond() << "<---\n";
				if (!comboTraceFile.write(reinterpret_cast<const char*>(dataIn), rc))
				{
					OW_THROW_ERRNO_MSG(IOException, "Failed writing to socket dump");
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
		if (errorAsException == E_THROW_ON_ERROR)
		{
			OW_THROW_ERRNO_MSG(SocketException, "SocketBaseImpl::read");
		}
	}
	return rc;
}
//////////////////////////////////////////////////////////////////////////////
bool
SocketBaseImpl::waitForInput(const Timeout& timeout)
{
	int rval = SocketUtils::waitForIO(m_sockfd, timeout, SocketFlags::E_WAIT_FOR_INPUT);
	if (rval == ETIMEDOUT)
	{
		m_recvTimeoutExprd = true;
	}
	else
	{
		m_recvTimeoutExprd = false;
	}
	return (rval != 0);
}
//////////////////////////////////////////////////////////////////////////////
bool
SocketBaseImpl::waitForOutput(const Timeout& timeout)
{
	return SocketUtils::waitForIO(m_sockfd, timeout, SocketFlags::E_WAIT_FOR_OUTPUT) != 0;
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

} // end namespace OW_NAMESPACE

#endif	// #if !defined(OW_WIN32)

