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

#ifndef OW_INETSERVERSOCKET_HPP_INCLUDE_GUARD_
#define OW_INETSERVERSOCKET_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_SelectableIFC.hpp"
#include "OW_ServerSocketImpl.hpp"
#include "OW_Reference.hpp"
#include "OW_Types.h"
#include "OW_SocketFlags.hpp"

class OW_ServerSocket : public OW_SelectableIFC
{
public:

	/** Allocate a new Inet Server Socket.
	 * @param isSSL is the Server Socket an SSL socket?
	 */
	OW_ServerSocket(OW_SocketFlags::ESSLFlag isSSL = OW_SocketFlags::E_NOT_SSL)
	  : OW_SelectableIFC()
	  , m_impl(new OW_ServerSocketImpl(isSSL))
   {
   }

	/**
	 * Copy ctor
	 */
	OW_ServerSocket(const OW_ServerSocket& arg)
		: OW_SelectableIFC()
		, m_impl(arg.m_impl) {}


	/**
	 * Accept a connection to the server socket
	 * @param timeoutSecs the timeout
	 * @return an OW_Socket for the connection just accepted.
	 */
	OW_Socket accept(int timeoutSecs=-1)
	{
		return m_impl->accept(timeoutSecs);
	}

	/**
	 * Start listening on a port
	 *
	 * @param port The port to listen on
	 * @param isSSL is the Server Socket an SSL socket?
	 * @param queueSize the size of the listen queue
	 * @param allInterfaces do we listen on all interfaces?
	 * @throws OW_SocketException
	 */
	void doListen(OW_UInt16 port, OW_SocketFlags::ESSLFlag isSSL, int queueSize=10,
			OW_SocketFlags::EAllInterfacesFlag allInterfaces = OW_SocketFlags::E_NOT_ALL_INTERFACES, 
			OW_SocketFlags::EReuseAddrFlag reuseAddr = OW_SocketFlags::E_REUSE_ADDR)
	{
		m_impl->doListen(port, isSSL, queueSize, allInterfaces, reuseAddr);
	}

	/**
	 * Start listening on a Unix Domain Socket
	 *
	 * @param filename The filename for the unix domain socket
	 * @param queueSize the size of the listen queue
	 * @throws OW_SocketException
	 */
	void doListen(const OW_String& filename, int queueSize=10, 
		bool reuseAddr=true)
	{
		m_impl->doListen(filename, queueSize, reuseAddr);
	}

	/**
	 * Close the listen socket
	 * @throws OW_SocketException
	 */
	void close() { m_impl->close();}
	
	/**
	 * Return the address of the local host
	 * @return an OW_SocketAddress representing the local node
	 */
	OW_SocketAddress getLocalAddress() { return m_impl->getLocalAddress(); }

	/**
	 * Get the file descriptor of the listen socket
	 * @return a handle to the listen socket
	 */
	OW_SocketHandle_t getfd() const { return m_impl->getfd(); }

	OW_Select_t getSelectObj() const { return m_impl->getSelectObj(); }

private:
	OW_Reference<OW_ServerSocketImpl> m_impl;
};

#endif

