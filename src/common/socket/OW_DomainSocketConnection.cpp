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
#include "OW_Types.h"
#include "OW_DomainSocketConnection.hpp"
#include "OW_Format.hpp"
#include "OW_BaseStreamBuffer.hpp"
#include "OW_RandomNumber.hpp"
#include "OW_AutoPtr.hpp"
#include "OW_Assertion.hpp"
#include <cstdio>
#include <fstream>

extern "C"
{
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
}


#ifdef OW_USE_GNU_PTH
	extern "C"
	{
	#include <pth.h>
	}
	#define NATIVE_WRITE pth_write
	#define NATIVE_READ pth_read
	#define NATIVE_CONNECT pth_connect
	#define NATIVE_SELECT pth_select
#else
	#define NATIVE_WRITE ::write
	#define NATIVE_READ ::read
	#define NATIVE_CONNECT ::connect
	#define NATIVE_SELECT ::select
#endif

//////////////////////////////////////////////////////////////////////////////
class BinaryFStreamBuffer : public OW_BaseStreamBufferNonBuffered
{
public:
	BinaryFStreamBuffer(int fd)
		: OW_BaseStreamBufferNonBuffered()
		, m_fd(fd) {}
protected:
	virtual int buffer_to_device(const char *c, int n);
	virtual int buffer_from_device(char *c, int n);
private:
	int m_fd;
};

//////////////////////////////////////////////////////////////////////////////
int
BinaryFStreamBuffer::buffer_to_device(const char *c, int n)
{
	int rc = 0;
	if(n)
	{
		if(OW_DomainSocketConnectionImpl::waitForIO(m_fd, false, 60*5) != 0)
		{
			rc = -1;
		}
		else
		{
			if(NATIVE_WRITE(m_fd, c, n) != n)
			{
				rc = -1;
			}
		}
	}

	return rc;
}

//////////////////////////////////////////////////////////////////////////////
int
BinaryFStreamBuffer::buffer_from_device(char *c, int n)
{
	int rc = 0;
	if(n)
	{
		if(OW_DomainSocketConnectionImpl::waitForIO(m_fd, true, 60*5) != 0)
		{
			rc = -1;
		}
		else
		{
			rc = NATIVE_READ(m_fd, c, n);
			if(rc == 0)
			{
				rc = -1;
			}
		}
	}
	return rc;
}

//////////////////////////////////////////////////////////////////////////////
class BinaryIOStreamBase
{
public:
	BinaryIOStreamBase(int fd) : m_strbuf(fd) {}
	BinaryFStreamBuffer m_strbuf;
};

//////////////////////////////////////////////////////////////////////////////
class BinaryIFStream : private BinaryIOStreamBase, public std::istream
{
public:
	BinaryIFStream(int fd) : BinaryIOStreamBase(fd), std::istream(&m_strbuf) {}
};

//////////////////////////////////////////////////////////////////////////////
class BinaryOFStream : private BinaryIOStreamBase, public std::ostream
{
public:
	BinaryOFStream(int fd) : BinaryIOStreamBase(fd), std::ostream(&m_strbuf) {}
};

//////////////////////////////////////////////////////////////////////////////
class BinaryFStream : private BinaryIOStreamBase, public std::iostream
{
public:
	BinaryFStream(int fd) : BinaryIOStreamBase(fd), std::iostream(&m_strbuf) {}
};

//////////////////////////////////////////////////////////////////////////////
OW_DomainSocketConnectionImpl::OW_DomainSocketConnectionImpl()
	: OW_IPCConnectionImpl()
	, m_fd(-1)
	, m_in(0)
	, m_out(0)
	, m_inout(0)
	, m_userName()
	, m_uid(::getuid())
	, m_gid(::getgid())
{
}

//////////////////////////////////////////////////////////////////////////////
OW_DomainSocketConnectionImpl::OW_DomainSocketConnectionImpl(int fd)
	: OW_IPCConnectionImpl()
	, m_fd(fd)
	, m_in(0)
	, m_out(0)
	, m_inout(0)
	, m_userName()
	, m_uid(::getuid())
	, m_gid(::getgid())
{
	if(m_fd != -1)
	{
		getStreams();
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_DomainSocketConnectionImpl::OW_DomainSocketConnectionImpl(int fd,
	const OW_String& userName, uid_t uid, gid_t gid)
	: OW_IPCConnectionImpl()
	, m_fd(fd)
	, m_in(0)
	, m_out(0)
	, m_inout(0)
	, m_userName(userName)
	, m_uid(uid)
	, m_gid(gid)
{
	if(m_fd != -1)
	{
		getStreams();
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_DomainSocketConnectionImpl::~OW_DomainSocketConnectionImpl()
{
	disconnect();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DomainSocketConnectionImpl::connect()
{
	disconnect();

	struct sockaddr_un address;
	size_t addrLength;

	if((m_fd = ::socket(PF_UNIX, SOCK_STREAM, 0)) < 0)
	{
		int lerrno = errno;
		m_fd = -1;
		OW_THROW(OW_IPCConnectionException,
			format("socket call failed: %1", ::strerror(lerrno)).c_str());
	}

	address.sun_family = AF_UNIX;
	::strncpy(address.sun_path, OW_DOMAIN_SOCKET_NAME,
		sizeof(address.sun_path));

#ifdef OW_SOLARIS
	addrLength = ::strlen(address.sun_path) +
		offsetof(struct sockaddr_un, sun_path);
#elif defined OW_OPENUNIX
    address.sun_len = sizeof(address);
	addrLength = ::strlen(address.sun_path) +
		offsetof(struct sockaddr_un, sun_path);
#else
    addrLength = sizeof(address.sun_family) + ::strlen(address.sun_path);
#endif

	if(NATIVE_CONNECT(m_fd, (struct sockaddr*)&address, addrLength))
	{
		int lerrno = errno;
		::close(m_fd);
		m_fd = -1;
		OW_THROW(OW_IPCConnectionException,
			format("connect call failed: %1", ::strerror(lerrno)).c_str());
	}

	try
	{
		authenticate();
	}
	catch(...)
	{
		::close(m_fd);
		m_fd = -1;
		throw;
	}

	getStreams();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DomainSocketConnectionImpl::authenticate()
{
	if(waitForOutput(15))
	{
		OW_THROW(OW_IPCConnectionException,
			"Timed out waiting for the CIMOM to authenticate me");
	}

	if(write(&m_uid, sizeof(m_uid)) != sizeof(m_uid))
	{
		OW_THROW(OW_IPCConnectionException, "Unable to send user id to CIMOM");
	}

	if(waitForInput(15))
	{
		OW_THROW(OW_IPCConnectionException,
			"Timed out waiting for the CIMOM respond on authenticate");
	}

	OW_Int32 fnameLen;
	if(read(&fnameLen, sizeof(fnameLen)) != sizeof(fnameLen))
	{
		OW_THROW(OW_IPCConnectionException,
			"Timed out waiting for the CIMOM respond on authenticate");
	}

	OW_AutoPtr<char> fname(new char[fnameLen]);
	if(read(fname.get(), fnameLen) != fnameLen)
	{
		OW_THROW(OW_IPCConnectionException,
			"Failed reading file name on authenticate");
	}

	int authfd = ::open(fname.get(), O_RDWR);
	if(authfd == -1)
	{
		int lerrno = errno;
		OW_THROW(OW_IPCConnectionException,
			format("Authenticate failed to open file %1. Error = %2",
				fname.get(), ::strerror(lerrno)).c_str());
	}

	OW_Int32 rn;

	if(NATIVE_READ(authfd, &rn, sizeof(rn)) != sizeof(rn))
	{
		int lerrno = errno;
		::close(authfd);
		OW_THROW(OW_IPCConnectionException,
			format("Authenticate failed to read file %1. Error = %2",
				fname.get(), ::strerror(lerrno)).c_str());
	}

	OW_RandomNumber rng(1, 268435455);
	OW_Int32 myrn = rng.getNextNumber();
	while(myrn == rn)
	{
		myrn = rng.getNextNumber();
	}

	if(::lseek(authfd, (off_t)0, SEEK_SET) == (off_t)-1)
	{
		int lerrno = errno;
		::close(authfd);
		OW_THROW(OW_IPCConnectionException,
			format("Authenticate failed to seek in file %1. Error = %2",
				fname.get(), ::strerror(lerrno)).c_str());
	}

	if(NATIVE_WRITE(authfd, &myrn, sizeof(myrn)) != sizeof(myrn))
	{
		int lerrno = errno;
		::close(authfd);
		OW_THROW(OW_IPCConnectionException,
			format("Authenticate failed to write file %1. Error = %2",
				fname.get(), ::strerror(lerrno)).c_str());
	}

	::close(authfd);
	if(waitForOutput(15))
	{
		OW_THROW(OW_IPCConnectionException,
			"Timed out waiting to send auth tokens to CIMOM");
	}

	if(write(&rn, sizeof(rn)) != sizeof(rn))
	{
		OW_THROW(OW_IPCConnectionException,
			"Authenticate failed to send auth token 1 to CIMOM");
	}

	if(write(&myrn, sizeof(myrn)) != sizeof(myrn))
	{
		OW_THROW(OW_IPCConnectionException,
			"Authenticate failed to send auth token 2 to CIMOM");
	}

	if(waitForInput(15))
	{
		OW_THROW(OW_IPCConnectionException,
			"Timed out waiting for the CIMOM respond on authenticate");
	}

	if(read(&rn, sizeof(rn)) != sizeof(rn)
	   || rn != 0)
	{
		OW_THROW(OW_IPCConnectionException, "Failed to authenticate to CIMOM");
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DomainSocketConnectionImpl::disconnect()
{
	if(m_fd != -1)
	{
		delete m_in;
		m_in = 0;
		delete m_out;
		m_out = 0;
		delete m_inout;
		m_inout = 0;
		::close(m_fd);
		m_fd = -1;
	}
}

//////////////////////////////////////////////////////////////////////////////
int
OW_DomainSocketConnectionImpl::write(const void *dataOut, int dataOutLen,
	OW_Bool errorAsException)

{
	if(m_fd == -1)
	{
		OW_THROW(OW_IPCConnectionException,
			"OW_DomainSocketConnectionImpl::write failed. File handle is"
			" invalid");
	}

	int cc = dsockWrite(m_fd, dataOut, dataOutLen);
	if(cc != dataOutLen && errorAsException)
	{
		OW_THROW(OW_IPCConnectionException,
			"OW_DomainSocketConnectionImpl::write failed");
	}

	return cc;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_DomainSocketConnectionImpl::read(void *dataIn, int dataInLen,
	OW_Bool errorAsException)
{
	if(m_fd == -1)
	{
		OW_THROW(OW_IPCConnectionException,
			"OW_DomainSocketConnectionImpl::read called with invalid file handle");
	}

	int cc = dsockRead(m_fd, dataIn, dataInLen);
	if(cc != dataInLen && errorAsException)
	{
		OW_THROW(OW_IPCConnectionException,
			"OW_DomainSocketConnectionImpl::read failed");
	}
	return cc;
}

//////////////////////////////////////////////////////////////////////////////
std::istream&
OW_DomainSocketConnectionImpl::getInputStream()
{
   return *m_in;
}

//////////////////////////////////////////////////////////////////////////////
std::ostream&
OW_DomainSocketConnectionImpl::getOutputStream()
{
   return *m_out;
}

//////////////////////////////////////////////////////////////////////////////
std::iostream&
OW_DomainSocketConnectionImpl::getIOStream()
{
   return *m_inout;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_DomainSocketConnectionImpl::waitForInput(int timeOutSecs)
{
	return waitForIO(true, timeOutSecs);
}

//////////////////////////////////////////////////////////////////////////////
int
OW_DomainSocketConnectionImpl::waitForOutput(int timeOutSecs)
{
	return waitForIO(false, timeOutSecs);
}

//////////////////////////////////////////////////////////////////////////////
int
OW_DomainSocketConnectionImpl::waitForIO(OW_Bool forInput, int timeOutSecs)
{
	return waitForIO(m_fd, forInput, timeOutSecs);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
int
OW_DomainSocketConnectionImpl::waitForIO(int fd, OW_Bool forInput, int timeOutSecs)
{
	fd_set thefds;
	fd_set* preadfds = 0;
	fd_set* pwritefds = 0;
	struct timeval timeout;
	struct timeval *ptimeval = 0;

	FD_ZERO(&thefds);
	FD_SET(fd, &thefds);

	if(timeOutSecs > 0)
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

	int rc = NATIVE_SELECT(fd+1, preadfds, pwritefds, 0, ptimeval);
	switch(rc)
	{
	  case 0: rc = -2; break;
	  case -1: break;
	  default: rc = 0;
	}
	
	return rc;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
int
OW_DomainSocketConnectionImpl::dsockWrite(int fd, const void* dataOut,
	int dataOutLen)
{
	unsigned char* outBfr = (unsigned char*) dataOut;

	int written = 0;
	while(dataOutLen > 0)
	{
		if(waitForIO(fd, false, -1) != 0)
		{
			OW_THROW(OW_IPCConnectionException,
				"OW_DomainSocketConnectionImpl::write failed waiting to write");
		}

		int rc;
		do
		{
			rc = NATIVE_WRITE(fd, outBfr, dataOutLen);
		} while(rc < 0 && errno == EINTR);

		if(rc == -1)
		{
			int lerrno = errno;
			OW_THROW(OW_IPCConnectionException,
				format("OW_DomainSocketConnectionImpl::write write failed - %1",
					strerror(lerrno)).c_str());
		}

		outBfr += rc;
		dataOutLen -= rc;
		written += rc;
	}

	return written;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
int
OW_DomainSocketConnectionImpl::dsockRead(int fd, void* dataIn, int dataInLen)
{
	int lerrno;
	bool isMore = true;
	int bytesread = 0;
	unsigned char* inBfr = (unsigned char*) dataIn;
	while(dataInLen > 0 && isMore)
	{
		if(OW_DomainSocketConnectionImpl::waitForIO(fd, true, -1) != 0)
		{
			OW_THROW(OW_IPCConnectionException,
				"OW_DomainSocketConnectionImpl::dsockread failed waiting to"
				" read");
		}

		int rc;
		do
		{
			rc = NATIVE_READ(fd, inBfr, dataInLen);
		} while(rc < 0 && errno == EINTR);

		switch(rc)
		{
			case -1:
				lerrno = errno;
				OW_THROW(OW_IPCConnectionException,
					format("OW_DomainSocketConnectionImpl::dsockread read"
						" failed - %1", strerror(lerrno)).c_str());

			case 0:
				if(bytesread > 0)
				{
					isMore = false;
					break;
				}
				OW_THROW(OW_IPCConnectionException,
					"OW_DomainSocketConnectionImpl::dsockread lost connection"
					" with client");

			default:
				inBfr += rc;
				dataInLen -= rc;
				bytesread += rc;
		}
	}

	return bytesread;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DomainSocketConnectionImpl::getStreams()
{
	m_in = new BinaryIFStream(m_fd);
	m_out = new BinaryOFStream(m_fd);
	m_inout = new BinaryFStream(m_fd);
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_IPCConnectionImplRef
OW_IPCConnectionImpl::createIPCConnection()
{
   return OW_IPCConnectionImplRef(new OW_DomainSocketConnectionImpl);
}


