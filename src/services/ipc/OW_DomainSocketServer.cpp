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
#include "OW_DomainSocketServer.hpp"
#include "OW_DomainSocketConnection.hpp"
#include "OW_Format.hpp"
#include "OW_RandomNumber.hpp"
#include "OW_MutexLock.hpp"

extern "C"
{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <stddef.h>
#include <fcntl.h>
#include <pwd.h>
}

#ifdef OW_USE_GNU_PTH
	extern "C"
	{
	#include <pth.h>
	}
	#define NATIVE_ACCEPT pth_accept
	#define NATIVE_READ pth_read
	#define NATIVE_WRITE pth_write
#else
	#define NATIVE_ACCEPT ::accept
	#define NATIVE_READ ::read
	#define NATIVE_WRITE ::write
#endif

// TODO: Fix this!
#include <iostream>
#define OW_LOGERROR(msg) std::cerr << msg << std::endl;
#define OW_LOGCUSTINFO(msg) std::cerr << msg << std::endl;
#define OW_LOGDEBUG(msg) std::cerr << msg << std::endl;

//////////////////////////////////////////////////////////////////////////////
inline int
sockRead(int fd, void* dataIn, int dataInLen)
{
	return OW_DomainSocketConnectionImpl::dsockRead(fd, dataIn, dataInLen);
}

//////////////////////////////////////////////////////////////////////////////
inline int
sockWrite(int fd, const void* dataOut, int dataOutLen)
{
	return OW_DomainSocketConnectionImpl::dsockWrite(fd, dataOut, dataOutLen);
}

//////////////////////////////////////////////////////////////////////////////
inline int
waitForInput(int fd, int timeOutSecs=-1)
{
	return OW_DomainSocketConnectionImpl::waitForIO(fd, true, timeOutSecs);
}

//////////////////////////////////////////////////////////////////////////////
OW_DomainSocketServer::OW_DomainSocketServer()
	: OW_IPCServerImpl()
	, m_fd(-1)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_DomainSocketServer::~OW_DomainSocketServer()
{
	close();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DomainSocketServer::close()
{
	if(m_fd != -1)
	{
		::close(m_fd);
		m_fd = -1;
		::unlink(OW_DOMAIN_SOCKET_NAME);
	}
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DomainSocketServer::initialize()
{
	close();

	struct sockaddr_un address;
	socklen_t addrLength;

	if((m_fd = ::socket(PF_UNIX, SOCK_STREAM, 0)) < 0)
	{
		int lerrno = errno;
		OW_THROW(OW_IPCConnectionException,
			format("socket call failed: %1", ::strerror(lerrno)).c_str());
	}

	::unlink(OW_DOMAIN_SOCKET_NAME);

	address.sun_family = AF_UNIX;		// Unix Domain Socket
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

	if(::bind(m_fd, (struct sockaddr*) &address, addrLength))
	{
		int lerrno = errno;
		OW_THROW(OW_IPCConnectionException,
			format("bind call failed: %1", ::strerror(lerrno)).c_str());
	}

	if(::listen(m_fd, 20))
	{
		int lerrno = errno;
		OW_THROW(OW_IPCConnectionException,
			format("listen call failed: %1", ::strerror(lerrno)).c_str());
	}

	::chmod(OW_DOMAIN_SOCKET_NAME, 0777);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_DomainSocketServer::cleanup()
{
	close();
}

//////////////////////////////////////////////////////////////////////////////
OW_IPCConnection
OW_DomainSocketServer::getClientConnection()
{
	if(m_fd == -1)
	{
		OW_THROW(OW_IPCConnectionException, "server socket not open");
	}

	int clientfd;
	struct sockaddr_un address;
	socklen_t addrLength = sizeof(address);


	if((clientfd = NATIVE_ACCEPT(m_fd, (struct sockaddr*)&address,
		&addrLength)) < 0)
	{
		int lerrno = errno;
		if(lerrno == EAGAIN || lerrno == EWOULDBLOCK)
		{
			return OW_IPCConnection(OW_IPCConnectionImplRef(0));
		}

		OW_THROW(OW_IPCConnectionException,
			format("accept call failed: %1", ::strerror(lerrno)).c_str());
	}

	OW_String userName;
	uid_t uid;
	gid_t gid;
	try
	{
		if(!authenticateClient(clientfd, userName, uid, gid))
		{
			::close(clientfd);
			return OW_IPCConnection(OW_IPCConnectionImplRef(0));
		}
	}
	catch(...)
	{
		::close(clientfd);
		throw;
	}
	
	OW_DomainSocketConnectionImpl* conn = new OW_DomainSocketConnectionImpl(clientfd,
		userName, uid, gid);

	return OW_IPCConnection(OW_IPCConnectionImplRef(conn));
}

static OW_Mutex authlock;

//////////////////////////////////////////////////////////////////////////////
// STATIC
bool
OW_DomainSocketServer::authenticateClient(int fd, OW_String& userName,
	uid_t& userid, gid_t& gid)
{
	//-- Create connection object and read the user id from it
	if(sockRead(fd, &userid, sizeof(userid)) != sizeof(userid))
	{
		OW_LOGERROR("IPC Authenticate: Failed reading user id on "
			"authentication");
		return false;
	}

	//-- Look up the user name for the user id
	OW_MutexLock ml(authlock);
	passwd* pw = ::getpwuid(userid);	// Re-entrant problem?
	if(!pw)
	{
		//-- FAILED to look up the user name for the given uid
		OW_LOGERROR(format("IPC Authenticate: Invalid user id: %1", userid));
		return false;
	}

	userName = pw->pw_name;
	gid = pw->pw_gid;
	ml.release();

	//-- Create temporary file for auth process
	char tfname[64];
	int authfd;
	::strcpy(tfname, "/tmp/OwAuThTmpFileXXXXXX");
	authfd = ::mkstemp(tfname);
	if(authfd == -1)
	{
		int lerrno = errno;
		OW_THROW(OW_IPCConnectionException,
			format("IPC Authenticate: mkstemp failed: %1",
				::strerror(lerrno)).c_str());
	}

	// Generate random number to put in file for client to read
	OW_RandomNumber rng(1, 268435455);
	OW_Int32 rn = rng.getNextNumber();
	OW_Int32 originalRandomNumber = rn;
		
	// Write the servers random number to the temp file
	if(NATIVE_WRITE(authfd, &rn, sizeof(rn)) != sizeof(rn))
	{
		int lerrno = errno;
		::close(authfd);
		::unlink(tfname);
		OW_THROW(OW_IPCConnectionException,
			format("IPC Authenticate: Failed writing to temp file %1  "
				"error: %2", tfname, ::strerror(lerrno)).c_str());
	}

	//-- Change file permission on temp file to read/write for user only
	::close(authfd);
	if(::chmod(tfname, 0600) == -1)
	{
		int lerrno = errno;
		::unlink(tfname);
		OW_THROW(OW_IPCConnectionException,
			format("IPC Authenticate: Failed changing permissions on temp "
				"file %1  error: %2", tfname, ::strerror(lerrno)).c_str());
	}

	//-- Change file so the user connecting is the owner
	if(::chown(tfname, userid, gid) == -1)
	{
		int lerrno = errno;
		::unlink(tfname);
		OW_LOGERROR(format("IPC Authenticate: Failed changing ownership on file "
			"%1 to userid %2. Error = %3", tfname, userid,
			::strerror(lerrno)));
		return false;
	}

	//-- Send length of file name with random value to client
	OW_Int32 fnameLen = strlen(tfname)+1;
	if(sockWrite(fd, &fnameLen, sizeof(fnameLen)) != sizeof(fnameLen))
	{
		::unlink(tfname);
		OW_LOGERROR(format("IPC Authenticate: Failed sending length of file "
			"name file to client with userid %1", userid));
		return false;
	}

	//-- Tell the client about the file to read the random number from
	if(sockWrite(fd, tfname, fnameLen) != fnameLen)
	{
		::unlink(tfname);
		OW_LOGERROR(format("IPC Authenticate: Failed sending name of random "
			"file to client with userid %1", userid));
		return false;
	}

	//-- Now wait for the client to respond
	if(waitForInput(fd, 15))
	{
		::unlink(tfname);
		OW_LOGERROR(format("IPC Authenticate: Timed out waiting for client "
			"to respond. Userid = %1", userid));
		return false;
	}

	//-- Read the 1st random number from the socket. This should be the
	//-- same value we put in the file for the client to read.
	OW_Int32 rnread;
	if(sockRead(fd, &rnread, sizeof(rnread)) != sizeof(rnread))
	{
		::unlink(tfname);
		OW_LOGERROR(format("IPC Authenticate: Failed reading initial "
			"authentication value from client. Userid = %1", userid));
		return false;
	}

	//-- If 1st number read is not the same as the previously generated
	//-- random number, then the client failed to authenticate
	if(rnread != rn)
	{
		::unlink(tfname);
		OW_LOGERROR(format("IPC Authenticate: Initial value from client was "
			"not valid. Userid = %1", userid));
		return false;
	}

	//-- Read the client's random number. This should be what is in our temp
	//-- file now.
	if(sockRead(fd, &rnread, sizeof(rnread)) != sizeof(rnread))
	{
		::unlink(tfname);
		OW_LOGERROR(format("IPC Authenticate: Failed reading second "
			"authentication value from client. Userid = %1", userid));
		return false;
	}

	//-- Ensure that the random number the client placed in the file is
	//-- different from the random number we put there. If it is the same,
	//-- don't authenticate the client.
	if(rnread == originalRandomNumber)
	{
		::unlink(tfname);
		OW_LOGERROR(format("IPC Authenticate: Client failed authentication. "
			"Using the same random number. Userid = %1", userid));
		return false;
	}

	//-- Now open the temp file again, and read the random value from it.
	//-- This value should match what the client just sent
	authfd = ::open(tfname, O_RDONLY);
	if(authfd == -1)
	{
		int lerrno = errno;
		::unlink(tfname);
		OW_LOGERROR(format("IPC Authenticate: Failed opening file: %1 "
			"error: %2. Userid = %3", tfname, strerror(lerrno), userid));
		return false;
	}

	//-- Read the client generated random number from the file.
	if(NATIVE_READ(authfd, &rn, sizeof(rn)) != sizeof(rn))
	{
		int lerrno = errno;
		::close(authfd);
		::unlink(tfname);
		OW_LOGERROR(format("IPC Authenticate: Failed reading from file: %1 "
			"error: %2. Userid = %3", tfname, strerror(lerrno), userid));
		return false;
	}

	//-- We're done with the temp file now, so clean it up
	::close(authfd);
	::unlink(tfname);

	//-- If the value in the file is different from what the client says it is,
	//-- then fail the authentication
	if(rn != rnread)
	{
		OW_LOGERROR(format("IPC Authenticate: Client failed authentication. "
			"Userid = %1", userid));
		return false;
	}

	rn = 0;
	if(sockWrite(fd, &rn, sizeof(rn)) != sizeof(rn))
	{
		OW_LOGERROR(format("IPC Authenticate: Failed sending ACK to client. "
			"Userid = %1", userid));
		return false;

	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_IPCServerImplRef
OW_IPCServerImpl::createAPIServer()
{
	return OW_IPCServerImplRef(new OW_DomainSocketServer);
}


