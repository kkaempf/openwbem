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
#ifndef OW_DOMAIN_SOCKET_CONNECTION_HPP_
#define OW_DOMAIN_SOCKET_CONNECTION_HPP_

#include "OW_config.h"
#include "OW_IPCConnection.hpp"
#include "OW_IOIFCStreamBuffer.hpp"
#include <iostream>

#define OW_DOMAIN_SOCKET_NAME "/tmp/OW@LCL@APIIPC_72859_Xq47Bf_P9r761-5_J-7_Q"

extern "C"
{
#include <unistd.h>
#include <sys/types.h>
}

class OW_DomainSocketConnectionImpl : public OW_IPCConnectionImpl
{
public:

	static const int TIMEOUT_SECS = 60*5;

	OW_DomainSocketConnectionImpl();
	~OW_DomainSocketConnectionImpl();
	virtual void connect();
	virtual void disconnect();
	virtual int read(void *dataIn, int dataInLen, OW_Bool errorAsException=false);
	virtual int write(const void *dataOut, int dataOutLen, OW_Bool errorAsException=false);
	virtual std::istream& getInputStream();
	virtual std::ostream& getOutputStream();
	virtual std::iostream& getIOStream();
	virtual int waitForInput(int timeOutSecs=-1);
	virtual int waitForOutput(int timeOutSecs=-1);
	virtual OW_String getUserName() { return m_userName; }

	OW_Select_t getSelectObj() const { return m_fd; }

	static int waitForIO(int fd, OW_Bool forInput, int timeOutSecs);
	static int dsockRead(int fd, void* dataIn, int dataInLen);
	static int dsockWrite(int fd, const void* dataOut, int dataOutLen);

private:

	OW_DomainSocketConnectionImpl(int fd, const OW_String& userName, uid_t uid,
		gid_t gid);
	OW_DomainSocketConnectionImpl(int fd);
	int waitForIO(OW_Bool forInput, int timeOutSecs=-1);
	void authenticate();
	void getStreams();

	int m_fd;
	std::istream *m_in;
	std::ostream *m_out;
	std::iostream *m_inout;
	OW_String m_userName;
	uid_t m_uid;
	gid_t m_gid;

	friend class OW_DomainSocketServer;
};

#endif

