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
 * @name		OW_SSLSocketImpl.hpp
 * @author	J. Bart Whiteley
 * @author Dan Nuffer
 *
 * @description
 *		Interface file for the OW_SSLSocketImpl class
 */
#ifndef OW_INETSSLSOCKETIMPL_HPP_INCLUDE_GUARD_
#define OW_INETSSLSOCKETIMPL_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_SocketBaseImpl.hpp"
#include "OW_SSLCtxMgr.hpp"
#include "OW_SSLException.hpp"
#ifndef OW_NO_SSL

namespace OpenWBEM
{

class OW_COMMON_API SSLSocketImpl : public SocketBaseImpl
{
public:
	SSLSocketImpl();
	/**
	 * This constructor is to be used only for server sockets.
	 * @param fd A socket handle, presumably created by a ServerSocket's
	 * accept().
	 */
	SSLSocketImpl(SocketHandle_t fd, SocketAddress::AddressType addrType);
	/**
	 * @throws SocketException
	 */
	SSLSocketImpl(const SocketAddress& addr);
	virtual ~SSLSocketImpl();
	/**
	 * @throws SocketException
	 */
	virtual void connect(const SocketAddress& addr);
	virtual void disconnect();
	Select_t getSelectObj() const;
private:
	/**
	 * @throws SocketException
	 */
	virtual int readAux(void* dataIn, int dataInLen);
	/**
	 * @throws SocketException
	 */
	virtual int writeAux(const void* dataOut, int dataOutLen);
	void connectSSL();
	SSL* m_ssl;
	BIO* m_sbio;
	SSLSocketImpl(const SSLSocketImpl& arg);
	SSLSocketImpl& operator =(const SSLSocketImpl& arg);
};

} // end namespace OpenWBEM

#endif // #ifndef OW_NO_SSL

#endif
