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
 * @name		OW_ServerSocketImpl.hpp
 * @author	Jon M. Carey
 * @author Dan Nuffer
 *
 * @description
 *		Interface file for the OW_ServerSocketImpl class
 */
#ifndef OW_INETSERVERSOCKETIMPL_HPP_INCLUDE_GUARD_
#define OW_INETSERVERSOCKETIMPL_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_SelectableIFC.hpp"
#include "OW_Socket.hpp"
#include "OW_SocketAddress.hpp"
#include "OW_Types.hpp"
#include "OW_File.hpp"
#include "OW_SocketFlags.hpp"
#include "OW_IntrusiveCountableBase.hpp"

namespace OpenWBEM
{

class ServerSocketImpl : public SelectableIFC
{
public:
	ServerSocketImpl(SocketFlags::ESSLFlag isSSL);
	~ServerSocketImpl();
	String addrString();
	Socket accept(int timeoutSecs=-1);
	void close();
//	unsigned long getLocalAddressRaw() { return m_localAddress; }
//	unsigned short getLocalPortRaw() { return m_localPort; }
	SocketAddress getLocalAddress() { return m_localAddress; }
	SocketHandle_t getfd() const { return m_sockfd; }
	void doListen(UInt16 port, SocketFlags::ESSLFlag isSSL, int queueSize=10, 
		const String& listenAddr = SocketAddress::ALL_LOCAL_ADDRESSES, 
		SocketFlags::EReuseAddrFlag reuseAddr = SocketFlags::E_REUSE_ADDR);

#if !defined(OW_WIN32)
	void doListen(const String& filename, int queueSize=10, 
		bool reuseAddr = true);
	bool waitForIO(int fd, int timeOutSecs, SocketFlags::EWaitDirectionFlag forInput) OW_DEPRECATED;
#endif

	Select_t getSelectObj() const;
private:
	void fillAddrParms();
	SocketHandle_t m_sockfd;
//	unsigned long m_localAddress;
//	unsigned short m_localPort;
	SocketAddress m_localAddress;
	bool m_isActive;
	ServerSocketImpl(const ServerSocketImpl& arg);
	ServerSocketImpl operator=(const ServerSocketImpl& arg);
	SocketFlags::ESSLFlag m_isSSL;
#if defined(OW_WIN32)
	HANDLE m_event;
#else
	File m_udsFile;
#endif
};

} // end namespace OpenWBEM

#endif
