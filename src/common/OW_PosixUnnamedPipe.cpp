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

extern "C"
{
#ifdef OW_WIN32
	#define _CLOSE ::_close
	#define _WRITE ::_write
	#define _READ ::_read
	#define _OPEN ::_open
	#include <io.h>
#else
	#ifdef OW_HAVE_UNISTD_H
		#include <unistd.h>
	#endif
	#define _CLOSE ::close
	#define _WRITE ::write
	#define _READ ::read
	#define _OPEN ::open
#endif

#include <fcntl.h>
#include <errno.h>
}
#include <cstring>

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
	if ((m_serverconn = ::accept(m_serversock, (struct sockaddr*)&sin, &val))
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
		::close(svrfd);
		fprintf(stderr, "CreateSocket(): Failed to bind on socket" );
		return -1;
	}
	if (listen(svrfd, 1) == -1) 
	{
		int lerrno = errno;
		::close(svrfd);
		return -1;
	}
  	val = sizeof(struct sockaddr_in);
	if (getsockname(svrfd, ( struct sockaddr * )&sin, &val ) == -1) 
	{
		int lerrno = errno;
		fprintf(stderr, "CreateSocket(): Failed to obtain socket name" );
		::close(svrfd);
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

	::close(svrfd);
	fds[0] = pat->getConnectFD();
	fds[1] = clientfd;
	delete pat;
	return 0;
}
}
#endif // OW_NETWARE

//////////////////////////////////////////////////////////////////////////////
// STATIC
UnnamedPipeRef
UnnamedPipe::createUnnamedPipe(EOpen doOpen)
{
	return UnnamedPipeRef(new PosixUnnamedPipe(doOpen));
}
//////////////////////////////////////////////////////////////////////////////
PosixUnnamedPipe::PosixUnnamedPipe(EOpen doOpen)
#ifndef OW_WIN32
	: m_blocking(E_BLOCKING)
#endif
{
#ifdef OW_WIN32
	m_blocking[0] = m_blocking[1] = E_BLOCKING;
#endif
	m_fds[0] = m_fds[1] = -1;
	if (doOpen)
	{
		open();
	}
	setTimeouts(60 * 10); // 10 minutes. This helps break deadlocks when using safePopen()
	setBlocking(E_BLOCKING); // necessary to set the pipes up right.
}
	
//////////////////////////////////////////////////////////////////////////////
PosixUnnamedPipe::~PosixUnnamedPipe()
{
	close();
}
//////////////////////////////////////////////////////////////////////////////
void
PosixUnnamedPipe::setBlocking(EBlockingMode outputIsBlocking)
{
#ifdef OW_WIN32
	// precondition
	OW_ASSERT(m_fds[0] != -1 && m_fds[1] != -1);

	m_blocking[0] = outputIsBlocking;
	m_blocking[1] = outputIsBlocking;
	// Unnamed pipes on Win32 cannot do non-blocking i/o (aka async, overlapped)
	// Only named pipes can. If this becomes a problem in the future, then
	// PosixUnnamedPipe can be implemented with NamedPipes. I know this can be
	// a problem with the signal handling mechanism that is used in the daemon
	// code, but I plan on do that differently on Win32
//	OW_ASSERT(outputIsBlocking);
	return;
#else
	// precondition
	OW_ASSERT(m_fds[0] != -1 && m_fds[1] != -1);

	m_blocking = outputIsBlocking;

	for (size_t i = 0; i <= 1; ++i)
	{
		int fdflags = fcntl(m_fds[i], F_GETFL, 0);
		if (fdflags == -1)
		{
			OW_THROW_ERRNO_MSG(IOException, "Failed to set pipe to non-blocking");
		}
		if (outputIsBlocking == E_BLOCKING)
		{
			fdflags &= !O_NONBLOCK;
		}
		else
		{
			fdflags |= O_NONBLOCK;
		}
		if (fcntl(m_fds[i], F_SETFL, fdflags) == -1)
		{
			OW_THROW_ERRNO_MSG(IOException, "Failed to set pipe to non-blocking");
		}
	}

#endif
}
//////////////////////////////////////////////////////////////////////////////
void
PosixUnnamedPipe::setOutputBlocking(bool outputIsBlocking)
{
#ifdef OW_WIN32
	// precondition
	OW_ASSERT(m_fds[1] != -1);
	
	m_blocking[1] = outputIsBlocking ? E_BLOCKING : E_NONBLOCKING ;
	// Unnamed pipes on Win32 cannot do non-blocking i/o (aka async, overlapped)
	// Only named pipes can. If this becomes a problem in the future, then
	// PosixUnnamedPipe can be implemented with NamedPipes. I know this can be
	// a problem with the signal handling mechanism that is used in the daemon
	// code, but I plan on do that differently on Win32
//	OW_ASSERT(outputIsBlocking);
	return;
#else
	// precondition
	OW_ASSERT(m_fds[1] != -1);
	
	m_blocking = outputIsBlocking ? E_BLOCKING : E_NONBLOCKING ;
	int fdflags = fcntl(m_fds[1], F_GETFL, 0);
	if (fdflags == -1)
	{
		OW_THROW_ERRNO_MSG(IOException, "Failed to set pipe to non-blocking");
	}
	if (outputIsBlocking)
	{
		fdflags ^= O_NONBLOCK;
	}
	else
	{
		fdflags |= O_NONBLOCK;
	}
	if (fcntl(m_fds[1], F_SETFL, fdflags) == -1)
	{
		OW_THROW_ERRNO_MSG(IOException, "Failed to set pipe to non-blocking");
	}
#endif
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
#else
	if (::pipe(m_fds) == -1)
#endif
	{
		m_fds[0] = m_fds[1] = -1;
		OW_THROW(UnnamedPipeException, ::strerror(errno));
	}
}
//////////////////////////////////////////////////////////////////////////////
int
PosixUnnamedPipe::close()
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
		rc = _CLOSE(m_fds[0]);
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
		rc = _CLOSE(m_fds[1]);
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
		rc = _CLOSE(m_fds[0]);
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
		rc = _CLOSE(m_fds[1]);
#endif
		m_fds[1] = -1;
	}
	return rc;
}
//////////////////////////////////////////////////////////////////////////////
int
PosixUnnamedPipe::write(const void* data, int dataLen, bool errorAsException)
{
	int rc = -1;
	if (m_fds[1] != -1)
	{
#ifndef OW_WIN32
		if (m_blocking == E_BLOCKING)
		{
			rc = SocketUtils::waitForIO(m_fds[1], m_writeTimeout, SocketFlags::E_WAIT_FOR_OUTPUT);
			if (rc != 0)
			{
				if (errorAsException)
				{
					OW_THROW_ERRNO_MSG(IOException, "SocketUtils::waitForIO failed.");
				}
				else
				{
					return rc;
				}
			}
		}
		rc = _WRITE(m_fds[1], data, dataLen);
#else
		BOOL bSuccess = FALSE;

		OVERLAPPED ovl;

		ovl.hEvent = (HANDLE)m_events[1];
		ovl.Offset = 0;
		ovl.OffsetHigh = 0;

		bSuccess = WriteFile(
			(HANDLE)m_fds[1],
			data,
			dataLen,
			NULL,
			&ovl);

		if( bSuccess && m_blocking[1] == E_BLOCKING )
		{
			bSuccess = WaitForSingleObject( (HANDLE)m_events[1], INFINITE ) == WAIT_OBJECT_0;
		}

		if( bSuccess )
			rc = 0;
#endif
	}
	if (errorAsException && rc == -1)
	{
		OW_THROW_ERRNO_MSG(IOException, "pipe write failed.");
	}
	return rc;
}
//////////////////////////////////////////////////////////////////////////////
int
PosixUnnamedPipe::read(void* buffer, int bufferLen, bool errorAsException)
{
	int rc = -1;
	if (m_fds[0] != -1)
	{
#ifndef OW_WIN32
		if (m_blocking == E_BLOCKING)
		{
			rc = SocketUtils::waitForIO(m_fds[0], m_readTimeout, SocketFlags::E_WAIT_FOR_INPUT);
			if (rc != 0)
			{
				if (errorAsException)
				{
					OW_THROW_ERRNO_MSG(IOException, "SocketUtils::waitForIO failed.");
				}
				else
				{
					return rc;
				}
			}
		}
		rc = _READ(m_fds[0], buffer, bufferLen);
#else
		BOOL bSuccess = FALSE;

		OVERLAPPED ovl;

		ovl.hEvent = (HANDLE)m_events[0];
		ovl.Offset = 0;
		ovl.OffsetHigh = 0;

		bSuccess = ReadFile(
			(HANDLE)m_fds[0],
			buffer,
			bufferLen,
			NULL,
			&ovl);

		if( bSuccess && m_blocking[0] == E_BLOCKING )
		{
			bSuccess = WaitForSingleObject( (HANDLE)m_events[0], INFINITE ) == WAIT_OBJECT_0;
		}

		if( bSuccess )
			rc = 0;
#endif
	}
	if (errorAsException && rc == -1)
	{
		OW_THROW_ERRNO_MSG(IOException, "pipe read failed.");
	}
	return rc;
}
//////////////////////////////////////////////////////////////////////////////
Select_t
PosixUnnamedPipe::getSelectObj() const
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

} // end namespace OW_NAMESPACE

