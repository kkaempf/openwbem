/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
 * @name		OW_SSLSocketImpl.cpp
 * @author	J. Bart Whiteley
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
#include <openssl/err.h>

namespace OpenWBEM
{

//////////////////////////////////////////////////////////////////////////////
SSLSocketImpl::SSLSocketImpl() 
	: SocketBaseImpl()
	, m_ssl(0)
	, m_sbio(0)
{
}
//////////////////////////////////////////////////////////////////////////////
SSLSocketImpl::SSLSocketImpl(SocketHandle_t fd, 
	SocketAddress::AddressType addrType) 
	: SocketBaseImpl(fd, addrType)
{
#ifdef OW_USE_GNU_PTH
	pth_yield(NULL);
#endif
	m_ssl = SSL_new(SSLCtxMgr::getSSLCtxServer());
	if (!m_ssl)
	{
		OW_THROW(SSLException, "SSL_new failed");
	}
	m_sbio = BIO_new_socket(fd, BIO_NOCLOSE);
	if (!m_sbio)
	{
		SSL_free(m_ssl);
		OW_THROW(SSLException, "BIO_new_socket failed");
	}
		
	SSL_set_bio(m_ssl, m_sbio, m_sbio);
	if (SSL_accept(m_ssl) <= 0)
	{
		SSL_shutdown(m_ssl);
		SSL_free(m_ssl);
		ERR_remove_state(0); // cleanup memory SSL may have allocated
		OW_THROW(SSLException, "SSL accept error");
	}
	if (!SSLCtxMgr::checkClientCert(m_ssl, m_peerAddress.getName()))
	{
		SSL_shutdown(m_ssl);
		SSL_free(m_ssl);
		ERR_remove_state(0); // cleanup memory SSL may have allocated
		OW_THROW(SSLException, "SSL failed to authenticate client");
	}
}
//////////////////////////////////////////////////////////////////////////////
SSLSocketImpl::SSLSocketImpl(const SocketAddress& addr) 
	: SocketBaseImpl(addr)
{
	connectSSL();
}
//////////////////////////////////////////////////////////////////////////////
SSLSocketImpl::~SSLSocketImpl()
{
	if(m_sockfd != -1 && m_isConnected)
	{
		if (m_ssl)
		{
			SSL_shutdown(m_ssl);
			SSL_free(m_ssl);
		}
		ERR_remove_state(0); // cleanup memory SSL may have allocated
	}
}
//////////////////////////////////////////////////////////////////////////////
Select_t
SSLSocketImpl::getSelectObj() const
{
	return m_sockfd;
}
//////////////////////////////////////////////////////////////////////////////
void 
SSLSocketImpl::connect(const SocketAddress& addr)
{
	SocketBaseImpl::connect(addr);
	connectSSL();
}
//////////////////////////////////////////////////////////////////////////////
void 
SSLSocketImpl::connectSSL()
{
#ifdef OW_USE_GNU_PTH
	pth_yield(NULL);
#endif
	m_isConnected = false;
	m_ssl = SSL_new(SSLCtxMgr::getSSLCtxClient());
	if (!m_ssl)
	{
		OW_THROW(SSLException, "SSL_new failed");
	}
	m_sbio = BIO_new_socket(m_sockfd, BIO_NOCLOSE);
	if (!m_sbio)
	{
		SSL_free(m_ssl);
		OW_THROW(SSLException, "BIO_new_socket failed");
	}
	SSL_set_bio(m_ssl, m_sbio, m_sbio);
	if (SSL_connect(m_ssl) <= 0)
	{
		OW_THROW(SSLException, "SSL connect error");
	}
	if (!SSLCtxMgr::checkServerCert(m_ssl, m_peerAddress.getName()))
	{
		OW_THROW(SSLException, "Failed to validate peer certificate");
	}
	m_isConnected = true;
}
//////////////////////////////////////////////////////////////////////////////
void
SSLSocketImpl::disconnect()
{
#ifdef OW_USE_GNU_PTH
	pth_yield(NULL);
#endif
	if(m_sockfd != -1 && m_isConnected)
	{
		if (m_ssl)
		{
			SSL_shutdown(m_ssl);
			SSL_free(m_ssl);
		}
		ERR_remove_state(0); // cleanup memory SSL may have allocated
	}
	SocketBaseImpl::disconnect();
}
//////////////////////////////////////////////////////////////////////////////
int 
SSLSocketImpl::writeAux(const void* dataOut, int dataOutLen)
{
#ifdef OW_USE_GNU_PTH
	pth_yield(NULL);
#endif
	return SSLCtxMgr::sslWrite(m_ssl, static_cast<const char*>(dataOut), 
			dataOutLen);
}
//////////////////////////////////////////////////////////////////////////////
int 
SSLSocketImpl::readAux(void* dataIn, int dataInLen)
{
#ifdef OW_USE_GNU_PTH
	pth_yield(NULL);
#endif
	return SSLCtxMgr::sslRead(m_ssl, static_cast<char*>(dataIn), 
			dataInLen);
}

} // end namespace OpenWBEM

//////////////////////////////////////////////////////////////////////////////
#endif // #ifdef OW_HAVE_OPENSSL

