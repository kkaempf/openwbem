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

/**
 * @name		OW_ServerSocketImpl.hpp
 * @author	Jon M. Carey
 * @company	Caldera Systems, Inc.
 * @since	4/20/2000
 *
 * @description
 *		Interface file for the OW_ServerSocketImpl class
 */
#ifndef __OW_INETSERVERSOCKETIMPL_HPP__
#define __OW_INETSERVERSOCKETIMPL_HPP__

#include "OW_config.h"
#include "OW_SelectableIFC.hpp"
#include "OW_Socket.hpp"
#include "OW_SocketAddress.hpp"
#include "OW_Types.h"
#include "OW_File.hpp"

class OW_ServerSocketImpl : public OW_SelectableIFC
{
public:
	OW_ServerSocketImpl(OW_Bool isSSL);
	~OW_ServerSocketImpl();
	OW_String addrString();
	OW_Socket accept(int timeoutSecs=-1);

	void close();
//	unsigned long getLocalAddressRaw() { return m_localAddress; }
//	unsigned short getLocalPortRaw() { return m_localPort; }

	OW_SocketAddress getLocalAddress() { return m_localAddress; }
	OW_SocketHandle_t getfd() const { return m_sockfd; }
	void doListen(OW_UInt16 port, OW_Bool isSSL, int queueSize=10, 
		OW_Bool allInterfaces=false);
	void doListen(const OW_String& filename, int queueSize=10);
	OW_Bool waitForIO(int fd, int timeOutSecs, OW_Bool forInput);
	OW_Select_t getSelectObj() const;

private:

	void fillAddrParms();

	OW_SocketHandle_t m_sockfd;
//	unsigned long m_localAddress;
//	unsigned short m_localPort;
	OW_SocketAddress m_localAddress;
	OW_Bool m_isActive;

	OW_ServerSocketImpl(const OW_ServerSocketImpl& arg);
	OW_ServerSocketImpl operator=(const OW_ServerSocketImpl& arg);

	OW_Bool m_isSSL;
	OW_File m_udsFile;
};

#endif	// __INETSERVERSOCKETIMPL_HPP__
