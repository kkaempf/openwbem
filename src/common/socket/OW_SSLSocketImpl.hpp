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
 * @name		OW_SSLSocketImpl.hpp
 * @author	J. Bart Whiteley
 * @company	Caldera Systems, Inc.
 * @since	1997/07/16
 *
 * @description
 *		Interface file for the OW_SSLSocketImpl class
 */
#ifndef OW_INETSSLSOCKETIMPL_HPP_INCLUDE_GUARD_
#define OW_INETSSLSOCKETIMPL_HPP_INCLUDE_GUARD_

#include "OW_config.h"

#ifndef OW_NO_SSL

#include "OW_SocketBaseImpl.hpp"
#include "OW_SSLCtxMgr.hpp"
#include "OW_SSLException.hpp"
#include "OW_Bool.hpp"


class OW_SSLSocketImpl : public OW_SocketBaseImpl
{
public:

	OW_SSLSocketImpl();
	/**
	 * This constructor is to be used only for server sockets.
	 * @param fd A socket handle, presumably created by a ServerSocket's
	 * accept().
	 */
	OW_SSLSocketImpl(OW_SocketHandle_t fd, OW_SocketAddress::AddressType addrType);

	OW_SSLSocketImpl(const OW_SocketAddress& addr)
		/*throw (OW_SocketException)*/;

	virtual ~OW_SSLSocketImpl();

	virtual void connect(const OW_SocketAddress& addr) /*throw (OW_SocketException)*/;
	virtual void disconnect();

	OW_Select_t getSelectObj() const;


private:
	virtual int readAux(void* dataIn, int dataInLen);
		/*throw (OW_SocketException)*/
	virtual int writeAux(const void* dataOut, int dataOutLen);
		/*throw (OW_SocketException)*/

	void connectSSL();

	SSL* m_ssl;
	BIO* m_sbio;

	OW_SSLSocketImpl(const OW_SSLSocketImpl& arg);
	OW_SSLSocketImpl operator =(const OW_SSLSocketImpl& arg);
};

#endif // #ifndef OW_NO_SSL

#endif

