/*******************************************************************************
* Copyright (C) 2001-2005 Vintela, Inc. All rights reserved.
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
 * @author Jon Carey
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_PosixUnnamedPipe.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_IOException.hpp"
#include "OW_Format.hpp"
#include "OW_SocketUtils.hpp"
#include "OW_Assertion.hpp"
#include "OW_DescriptorUtils.hpp"
#include "OW_SignalScope.hpp"
#include "OW_Logger.hpp"

// extern "C"
// {
#ifndef OW_WIN32
	#include "OW_Thread.hpp"
	#ifdef OW_HAVE_UNISTD_H
		#include <unistd.h>
	#endif
	#include <sys/socket.h>
	#include <sys/types.h>
#endif

#include <fcntl.h>
#include <errno.h>
// }
#include <cstring>

namespace
{
	using namespace OpenWBEM;
#ifndef OW_WIN32
	int upclose(int fd)
	{
		int rc;
		do
		{
			rc = ::close(fd);
		} while (rc < 0 && errno == EINTR);
		if (rc == -1)
		{
			int lerrno = errno;
			Logger lgr("ow.common");
			OW_LOG_ERROR(lgr, Format("Closing pipe handle %1 failed: %2", fd, lerrno));
		}
		return rc;
	}

	::ssize_t upread(int fd, void * buf, std::size_t count)
	{
		::ssize_t rv;
		do
		{
			Thread::testCancel();
			rv = ::read(fd, buf, count);
		} while (rv < 0 && errno == EINTR);
		return rv;
	}

	::ssize_t upwrite(int fd, void const * buf, std::size_t count)
	{
		::ssize_t rv;
		// block SIGPIPE so we don't kill the process if the pipe is closed.
		SignalScope ss(SIGPIPE, SIG_IGN);
		do
		{
			Thread::testCancel();
			rv = ::write(fd, buf, count);
		} while (rv < 0 && errno == EINTR);
		return rv;
	}

	int upaccept(int s, struct sockaddr * addr, socklen_t * addrlen)
	{
		int rv;
		do
		{
			rv = ::accept(s, addr, addrlen);
		} while (rv < 0 && errno == EINTR);
		return rv;
	}

	enum EDirection
	{
		E_WRITE_PIPE, E_READ_PIPE
	};

	// bufsz MUST be an int, and not some other integral type (address taken)
	//
	void setKernelBufferSize(int sockfd, int bufsz, EDirection edir)
	{
		if (sockfd == -1)
		{
			return;
		}

		int optname = (edir == E_WRITE_PIPE ? SO_SNDBUF : SO_RCVBUF);

		int getbufsz;
		socklen_t getbufsz_len = sizeof(getbufsz);
		int errc = ::getsockopt(sockfd, SOL_SOCKET, optname, &getbufsz, &getbufsz_len);
		if (errc == 0 && getbufsz < bufsz)
		{
			::setsockopt(sockfd, SOL_SOCKET, optname, &bufsz, sizeof(bufsz));
		}
	}

	void setDefaultKernelBufsz(int sockfd_read, int sockfd_write)
	{
		int const BUFSZ = 64 * 1024;
		setKernelBufferSize(sockfd_read, BUFSZ, E_READ_PIPE);
		setKernelBufferSize(sockfd_write, BUFSZ, E_WRITE_PIPE);
	}

#endif

}

namespace OW_NAMESPACE
{

#ifdef OW_NETWARE
namespace
{
class AcceptThread
{
public:
	AcceptThread(int serversock)
		: m_serversock(serversock)
		, m_serverconn(-1)
	{
	}

	void acceptConnection();
	int getConnectFD() { return m_serverconn; }
private:
	int m_serversock;
	int m_serverconn;
};

void
AcceptThread::acceptConnection()
{
    struct sockaddr_in sin;
	size_t val;
    int tmp = 1;

	tmp = 1;
	::setsockopt(m_serversock, IPPROTO_TCP, 1,		// #define TCP_NODELAY 1
		(char*) &tmp, sizeof(int));

	val = sizeof(struct sockaddr_in);
	if ((m_serverconn = upaccept(m_serversock, (struct sockaddr*)&sin, &val))
	   == -1)
	{
		return;
	}
	tmp = 1;
	::setsockopt(m_serverconn, IPPROTO_TCP, 1, // #define TCP_NODELAY 1
		(char *) &tmp, sizeof(int));
	tmp = 0;
	::setsockopt(m_serverconn, SOL_SOCKET, SO_KEEPALIVE,
				 (char*) &tmp, sizeof(int));
}

void*
runConnClass(void* arg)
{
	AcceptThread* acceptThread = (AcceptThread*)(arg);
	acceptThread->acceptConnection();
	::pthread_exit(NULL);
	return 0;
}

int
_pipe(int *fds)
{
	int svrfd, lerrno, connectfd;
	size_t val;
    struct sockaddr_in sin;

	svrfd = socket( AF_INET, SOCK_STREAM, 0 );
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl( 0x7f000001 ); // loopback
	sin.sin_port = 0;
	memset(sin.sin_zero, 0, 8 );
	if (bind(svrfd, (struct sockaddr * )&sin, sizeof( struct sockaddr_in ) ) == -1)
	{
		int lerrno = errno;
		upclose(svrfd);
		fprintf(stderr, "CreateSocket(): Failed to bind on socket" );
		return -1;
	}
	if (listen(svrfd, 1) == -1)
	{
		int lerrno = errno;
		upclose(svrfd);
		return -1;
	}
  	val = sizeof(struct sockaddr_in);
	if (getsockname(svrfd, ( struct sockaddr * )&sin, &val ) == -1)
	{
		int lerrno = errno;
		fprintf(stderr, "CreateSocket(): Failed to obtain socket name" );
		upclose(svrfd);
		return -1;
	}

	AcceptThread* pat = new AcceptThread(svrfd);
	pthread_t athread;
	// Start thread that will accept connection on svrfd.
	// Once a connection is made the thread will exit.
	pthread_create(&athread, NULL, runConnClass, pat);

	int clientfd = socket(AF_INET, SOCK_STREAM, 0);
	if (clientfd == -1)
	{
		delete pat;
		return -1;
	}

	// Connect to server
	struct sockaddr_in csin;
	csin.sin_family = AF_INET;
	csin.sin_addr.s_addr = htonl(0x7f000001); // loopback
	csin.sin_port = sin.sin_port;
	if (::connect(clientfd, (struct sockaddr*)&csin, sizeof(csin)) == -1)
	{
		delete pat;
		return -1;
	}

#define TCP_NODELAY 1
	int tmp = 1;
	//
	// Set for Non-blocking writes and disable keepalive
	//
	::setsockopt(clientfd, IPPROTO_TCP, TCP_NODELAY, (char*)&tmp, sizeof(int));
	tmp = 0;
	::setsockopt(clientfd, SOL_SOCKET, SO_KEEPALIVE, (char*)&tmp, sizeof(int));

	void* threadResult;
	// Wait for accept thread to terminate
	::pthread_join(athread, &threadResult);

	upclose(svrfd);
	fds[0] = pat->getConnectFD();
	fds[1] = clientfd;
	delete pat;
	return 0;
}
}
#endif // OW_NETWARE

//////////////////////////////////////////////////////////////////////////////
PosixUnnamedPipe::PosixUnnamedPipe(EOpen doOpen)
{
	m_fds[0] = m_fds[1] = -1;
	if (doOpen)
	{
		open();
	}
	setTimeouts(Timeout::relative(60 * 10)); // 10 minutes. This helps break deadlocks when using safePopen()
#ifndef OW_WIN32
	setBlocking(E_BLOCKING); // necessary to set the pipes up right.
#endif
}

//////////////////////////////////////////////////////////////////////////////
PosixUnnamedPipe::PosixUnnamedPipe(AutoDescriptor inputfd, AutoDescriptor outputfd)
{
	m_fds[0] = inputfd.get();
	m_fds[1] = outputfd.get();
	setTimeouts(Timeout::relative(60 * 10)); // 10 minutes. This helps break deadlocks when using safePopen()
	setBlocking(E_BLOCKING);
	setDefaultKernelBufsz(m_fds[0], m_fds[1]);
	inputfd.release();
	outputfd.release();
}

//////////////////////////////////////////////////////////////////////////////
PosixUnnamedPipe::~PosixUnnamedPipe()
{
	close();
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
#ifndef OW_WIN32
	typedef UnnamedPipe::EBlockingMode EBlockingMode;
	void set_desc_blocking(
		int d, EBlockingMode & bmflag, EBlockingMode blocking_mode)
	{
		OW_ASSERT(d != -1);
		int fdflags = fcntl(d, F_GETFL, 0);
		if (fdflags == -1)
		{
			OW_THROW_ERRNO_MSG(IOException, "Failed to set pipe blocking mode");
		}
		if (blocking_mode == UnnamedPipe::E_BLOCKING)
		{
			fdflags &= ~O_NONBLOCK;
		}
		else
		{
			fdflags |= O_NONBLOCK;
		}
		if (fcntl(d, F_SETFL, fdflags) == -1)
		{
			OW_THROW_ERRNO_MSG(IOException, "Failed to set pipe blocking mode");
		}
		bmflag = blocking_mode;
	}
#endif
}
//////////////////////////////////////////////////////////////////////////////
void
PosixUnnamedPipe::setBlocking(EBlockingMode blocking_mode)
{
	OW_ASSERT(m_fds[0] != -1 || m_fds[1] != -1);
#ifdef OW_WIN32
	m_blocking[0] = blocking_mode;
	m_blocking[1] = blocking_mode;
#else
	for (size_t i = 0; i < 2; ++i)
	{
		if (m_fds[i] != -1)
		{
			set_desc_blocking(m_fds[i], m_blocking[i], blocking_mode);
		}
	}
#endif
}
//////////////////////////////////////////////////////////////////////////////
void
PosixUnnamedPipe::setWriteBlocking(EBlockingMode blocking_mode)
{
	set_desc_blocking(m_fds[1], m_blocking[1], blocking_mode);
}
//////////////////////////////////////////////////////////////////////////////
void
PosixUnnamedPipe::setReadBlocking(EBlockingMode blocking_mode)
{
	set_desc_blocking(m_fds[0], m_blocking[0], blocking_mode);
}
//////////////////////////////////////////////////////////////////////////////
void
PosixUnnamedPipe::open()
{
	if (m_fds[0] != -1)
	{
		close();
	}
#if defined(OW_WIN32)
	HANDLE pipe = CreateNamedPipe( "\\\\.\\pipe\\TestPipe",
		PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_MESSAGE,
		PIPE_UNLIMITED_INSTANCES,
		2560,
		2560,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL );

	HANDLE client = CreateFile( "\\\\.\\pipe\\TestPipe",
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL );

	HANDLE event1 = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE event2 = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Should return immediately since the client connection is open.
	BOOL bConnected = ConnectNamedPipe( pipe, NULL );
	if( !bConnected && GetLastError() == ERROR_PIPE_CONNECTED )
		bConnected = TRUE;

	BOOL bSuccess =
		pipe != INVALID_HANDLE_VALUE &&
		client != INVALID_HANDLE_VALUE &&
		event1 != INVALID_HANDLE_VALUE &&
		event2 != INVALID_HANDLE_VALUE &&
		bConnected;

	if( !bSuccess )
	{
		CloseHandle(pipe);
		CloseHandle(client);
		CloseHandle(event1);
		CloseHandle(event2);
	}
	else
	{
		m_fds[0] = (int)client;		// read descriptor
		m_fds[1] = (int)pipe;		// write descriptor
		m_events[0] = (int)event1;
		m_events[1] = (int)event2;
	}

	if( !bSuccess )
//	if (::_pipe(m_fds, 2560, _O_BINARY) == -1)
#elif defined(OW_NETWARE)
	if (_pipe(m_fds) == -1)
	{
		m_fds[0] = m_fds[1] = -1;
		OW_THROW_ERRNO_MSG(UnnamedPipeException, "PosixUnamedPipe::open(): soketpair()");
	}
#else
	if (::socketpair(AF_UNIX, SOCK_STREAM, 0, m_fds) == -1)
	{
		m_fds[0] = m_fds[1] = -1;
		OW_THROW_ERRNO_MSG(UnnamedPipeException, "PosixUnamedPipe::open(): soketpair()");
	}
	::shutdown(m_fds[0], SHUT_WR);
	::shutdown(m_fds[1], SHUT_RD);
	setDefaultKernelBufsz(m_fds[0], m_fds[1]);
#endif
}
//////////////////////////////////////////////////////////////////////////////
int
PosixUnnamedPipe::close()
{
	int rc = -1;

	// handle the case where both input and output are the same descriptor.  It can't be closed twice.
	if (m_fds[0] == m_fds[1])
	{
		m_fds[1] = -1;
	}

	if (m_fds[0] != -1)
	{
#ifdef OW_WIN32
		HANDLE h = (HANDLE)m_fds[0];
		HANDLE e = (HANDLE)m_events[0];
		if( CloseHandle(h) && CloseHandle(e) )
			rc = 0;
#else
		rc = upclose(m_fds[0]);
#endif
		m_fds[0] = -1;
	}
	if (m_fds[1] != -1)
	{
#ifdef OW_WIN32
		HANDLE h = (HANDLE)m_fds[1];
		HANDLE e = (HANDLE)m_events[1];
		if( CloseHandle(h) && CloseHandle(e) )
			rc = 0;
#else
		rc = upclose(m_fds[1]);
#endif
		m_fds[1] = -1;
	}
	return rc;
}
//////////////////////////////////////////////////////////////////////////////
bool
PosixUnnamedPipe::isOpen() const
{
	return (m_fds[0] != -1) || (m_fds[1] != -1);
}

//////////////////////////////////////////////////////////////////////////////
int
PosixUnnamedPipe::closeInputHandle()
{
	int rc = -1;
	if (m_fds[0] != -1)
	{
#ifdef OW_WIN32
		HANDLE h = (HANDLE)m_fds[0];
		HANDLE e = (HANDLE)m_events[0];
		if( CloseHandle(h) && CloseHandle(e) )
			rc = 0;
#else
		if (m_fds[0] != m_fds[1])
		{
			rc = upclose(m_fds[0]);
		}
#endif
		m_fds[0] = -1;
	}
	return rc;
}
//////////////////////////////////////////////////////////////////////////////
int
PosixUnnamedPipe::closeOutputHandle()
{
	int rc = -1;
	if (m_fds[1] != -1)
	{
#ifdef OW_WIN32
		HANDLE h = (HANDLE)m_fds[1];
		HANDLE e = (HANDLE)m_events[1];
		if( CloseHandle(h) && CloseHandle(e) )
			rc = 0;
#else
		if (m_fds[0] != m_fds[1])
		{
			rc = upclose(m_fds[1]);
		}
#endif
		m_fds[1] = -1;
	}
	return rc;
}
//////////////////////////////////////////////////////////////////////////////
int
PosixUnnamedPipe::write(const void* data, int dataLen, ErrorAction errorAsException)
{
	int rc = -1;
	if (m_fds[1] != -1)
	{
#ifndef OW_WIN32
		if (m_blocking[1] == E_BLOCKING)
		{
			rc = SocketUtils::waitForIO(m_fds[1], getWriteTimeout(), SocketFlags::E_WAIT_FOR_OUTPUT);
			if (rc != 0)
			{
				if (rc == ETIMEDOUT)
				{
					errno = ETIMEDOUT;
				}
				if (errorAsException == E_THROW_ON_ERROR)
				{
					OW_THROW_ERRNO_MSG(IOException, "SocketUtils::waitForIO failed.");
				}
				else
				{
					return -1;
				}
			}
		}
		rc = upwrite(m_fds[1], data, dataLen);
#else
		BOOL bSuccess = FALSE;

		OVERLAPPED ovl;

		ovl.hEvent = (HANDLE)m_events[1];
		ovl.Offset = 0;
		ovl.OffsetHigh = 0;

#error "This is broken for non-blocking io. If no output buffer is available, write() should return -1 and set errno == ETIMEDOUT"
		bSuccess = WriteFile(
			(HANDLE)m_fds[1],
			data,
			dataLen,
			NULL,
			&ovl);

		if( bSuccess && m_blocking[1] == E_BLOCKING )
		{
#error "This needs to honor the timeout value"
			bSuccess = WaitForSingleObject( (HANDLE)m_events[1], INFINITE ) == WAIT_OBJECT_0;
		}

		if( bSuccess )
			rc = 0;
#endif
	}
	if (errorAsException == E_THROW_ON_ERROR && rc == -1)
	{
		if (m_fds[1] == -1)
		{
			OW_THROW(IOException, "pipe write failed because pipe is closed");
		}
		else
		{
			OW_THROW_ERRNO_MSG(IOException, "pipe write failed");
		}
	}
	return rc;
}
//////////////////////////////////////////////////////////////////////////////
int
PosixUnnamedPipe::read(void* buffer, int bufferLen, ErrorAction errorAsException)
{
	int rc = -1;
	if (m_fds[0] != -1)
	{
#ifndef OW_WIN32
		if (m_blocking[0] == E_BLOCKING)
		{
			rc = SocketUtils::waitForIO(m_fds[0], getReadTimeout(), SocketFlags::E_WAIT_FOR_INPUT);
			if (rc != 0)
			{
				if (rc == ETIMEDOUT)
				{
					errno = ETIMEDOUT;
				}
				if (errorAsException == E_THROW_ON_ERROR)
				{
					OW_THROW_ERRNO_MSG(IOException, "SocketUtils::waitForIO failed.");
				}
				else
				{
					return -1;
				}
			}
		}
		rc = upread(m_fds[0], buffer, bufferLen);
#else
		BOOL bSuccess = FALSE;

		OVERLAPPED ovl;

		ovl.hEvent = (HANDLE)m_events[0];
		ovl.Offset = 0;
		ovl.OffsetHigh = 0;
#error "This is broken for non-blocking io. If no input is available, read() should return -1 and set errno == ETIMEDOUT"
		bSuccess = ReadFile(
			(HANDLE)m_fds[0],
			buffer,
			bufferLen,
			NULL,
			&ovl);

		if( bSuccess && m_blocking[0] == E_BLOCKING )
		{
#error "This needs to honor the timeout value"
			bSuccess = WaitForSingleObject( (HANDLE)m_events[0], INFINITE ) == WAIT_OBJECT_0;
		}

		if( bSuccess )
			rc = 0;
#endif
	}
	if (rc == 0)
	{
		closeInputHandle();
	}
	if (errorAsException == E_THROW_ON_ERROR && rc == -1)
	{
		if (m_fds[0] == -1)
		{
			OW_THROW(IOException, "pipe read failed because pipe is closed");
		}
		else
		{
			OW_THROW_ERRNO_MSG(IOException, "pipe read failed");
		}
	}
	return rc;
}
//////////////////////////////////////////////////////////////////////////////
Select_t
PosixUnnamedPipe::getReadSelectObj() const
{
#ifdef OW_WIN32
	Select_t selectObj;
	selectObj.event = (HANDLE)m_events[0];
	selectObj.sockfd = INVALID_SOCKET;
	selectObj.networkevents = 0;
	selectObj.doreset = false;

	return selectObj;
#else
	return m_fds[0];
#endif
}

//////////////////////////////////////////////////////////////////////////////
Select_t
PosixUnnamedPipe::getWriteSelectObj() const
{
#ifdef OW_WIN32
	Select_t selectObj;
	selectObj.event = (HANDLE)m_events[1];
	selectObj.sockfd = INVALID_SOCKET;
	selectObj.networkevents = 0;
	selectObj.doreset = false;

	return selectObj;
#else
	return m_fds[1];
#endif
}

//////////////////////////////////////////////////////////////////////////////
void
PosixUnnamedPipe::passDescriptor(Descriptor descriptor)
{
	int rc = -1;
	if (m_fds[1] != -1)
	{
		if (m_blocking[1] == E_BLOCKING)
		{
			rc = SocketUtils::waitForIO(m_fds[1], getWriteTimeout(), SocketFlags::E_WAIT_FOR_OUTPUT);
			if (rc != 0)
			{
				if (rc == ETIMEDOUT)
				{
					errno = ETIMEDOUT;
				}
				OW_THROW_ERRNO_MSG(IOException, "SocketUtils::waitForIO failed.");
			}
		}
		rc = OpenWBEM::passDescriptor(m_fds[1], descriptor);
	}
	if (rc == -1)
	{
		if (m_fds[1] == -1)
		{
			OW_THROW(IOException, "sendDescriptor() failed because pipe is closed");
		}
		else
		{
			OW_THROW_ERRNO_MSG(IOException, "sendDescriptor() failed");
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
AutoDescriptor
PosixUnnamedPipe::receiveDescriptor()
{
	int rc = -1;
	AutoDescriptor descriptor;
	if (m_fds[0] != -1)
	{
		if (m_blocking[0] == E_BLOCKING)
		{
			rc = SocketUtils::waitForIO(m_fds[0], getReadTimeout(), SocketFlags::E_WAIT_FOR_INPUT);
			if (rc != 0)
			{
				if (rc == ETIMEDOUT)
				{
					errno = ETIMEDOUT;
				}
				OW_THROW_ERRNO_MSG(IOException, "SocketUtils::waitForIO failed.");
			}
		}
		descriptor = OpenWBEM::receiveDescriptor(m_fds[0]);
	}
	else
	{
		OW_THROW(IOException, "receiveDescriptor() failed because pipe is closed");
	}
	return descriptor;
}

//////////////////////////////////////////////////////////////////////////////
Descriptor
PosixUnnamedPipe::getInputDescriptor() const
{
	return m_fds[0];
}

//////////////////////////////////////////////////////////////////////////////
Descriptor
PosixUnnamedPipe::getOutputDescriptor() const
{
	return m_fds[1];
}


} // end namespace OW_NAMESPACE

