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
 * @name		OW_SSLSocketImpl.cpp
 * @author	J. Bart Whiteley
 * @company	Caldera Systems, Inc.
 * @since	1997/07/16
 *
 * @description
 *		Implementation file for the OW_SSLSocketImpl class.
 */
#include "OW_config.h"
#include "OW_SSLSocketImpl.hpp"
#ifdef OW_USE_GNU_PTH
extern "C"
{
#include <pth.h>
}
#endif

#ifdef OW_HAVE_OPENSSL

//////////////////////////////////////////////////////////////////////////////
OW_SSLSocketImpl::OW_SSLSocketImpl() 
	: OW_SocketBaseImpl()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_SSLSocketImpl::OW_SSLSocketImpl(OW_SocketHandle_t fd, 
	OW_SocketAddress::AddressType addrType) 
	: OW_SocketBaseImpl(fd, addrType)
{
#ifdef OW_USE_GNU_PTH
    pth_yield(NULL);
#endif

	m_sbio = BIO_new_socket(fd, BIO_NOCLOSE);
	m_ssl = SSL_new(OW_SSLCtxMgr::getSSLCtxServer());
	SSL_set_bio(m_ssl, m_sbio, m_sbio);
	if (SSL_accept(m_ssl) <= 0)
	{
		SSL_shutdown(m_ssl);
		SSL_free(m_ssl);
		OW_THROW(OW_SSLException, "SSL accept error");
	}
	if (!OW_SSLCtxMgr::checkClientCert(m_ssl, m_peerAddress.getName()))
	{
		SSL_shutdown(m_ssl);
		SSL_free(m_ssl);
		OW_THROW(OW_SSLException, "SSL failed to authenticate client");
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_SSLSocketImpl::OW_SSLSocketImpl(const OW_SocketAddress& addr) 
	: OW_SocketBaseImpl(addr)
{
	connectSSL();
}
//////////////////////////////////////////////////////////////////////////////
OW_SSLSocketImpl::~OW_SSLSocketImpl()
{
	if(m_sockfd != -1 && m_isConnected)
	{
		SSL_shutdown(m_ssl);
		SSL_free(m_ssl);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Select_t
OW_SSLSocketImpl::getSelectObj() const
{
	return m_sockfd;
}

//////////////////////////////////////////////////////////////////////////////
void 
OW_SSLSocketImpl::connect(const OW_SocketAddress& addr)
	/*throw (OW_SocketException)*/
{
	OW_SocketBaseImpl::connect(addr);
	connectSSL();
}

//////////////////////////////////////////////////////////////////////////////
void 
OW_SSLSocketImpl::connectSSL()
{
#ifdef OW_USE_GNU_PTH
    pth_yield(NULL);
#endif

	m_isConnected = false;
	m_ssl = SSL_new(OW_SSLCtxMgr::getSSLCtxClient());
	m_sbio = BIO_new_socket(m_sockfd, BIO_NOCLOSE);
	SSL_set_bio(m_ssl, m_sbio, m_sbio);
	if (SSL_connect(m_ssl) <= 0)
    {
		OW_THROW(OW_SSLException, "SSL connect error");
    }
	if (!OW_SSLCtxMgr::checkServerCert(m_ssl, m_peerAddress.getName()))
    {
		OW_THROW(OW_SSLException, "Failed to validate peer certificate");
    }
	m_isConnected = true;
}


//////////////////////////////////////////////////////////////////////////////
void
OW_SSLSocketImpl::disconnect()
{
#ifdef OW_USE_GNU_PTH
    pth_yield(NULL);
#endif

	if(m_sockfd != -1 && m_isConnected)
	{
		SSL_shutdown(m_ssl);
		SSL_free(m_ssl);
	}
	OW_SocketBaseImpl::disconnect();
}

//////////////////////////////////////////////////////////////////////////////
int 
OW_SSLSocketImpl::writeAux(const void* dataOut, int dataOutLen)
	/*throw (OW_SocketException)*/
{
#ifdef OW_USE_GNU_PTH
    pth_yield(NULL);
#endif

	return OW_SSLCtxMgr::sslWrite(m_ssl, static_cast<const char*>(dataOut), 
			dataOutLen);
}

//////////////////////////////////////////////////////////////////////////////
int 
OW_SSLSocketImpl::readAux(void* dataIn, int dataInLen)
	/*throw (OW_SocketException)*/
{
#ifdef OW_USE_GNU_PTH
    pth_yield(NULL);
#endif

	return OW_SSLCtxMgr::sslRead(m_ssl, static_cast<char*>(dataIn), 
			dataInLen);
}

//////////////////////////////////////////////////////////////////////////////

#endif // #ifdef OW_HAVE_OPENSSL

