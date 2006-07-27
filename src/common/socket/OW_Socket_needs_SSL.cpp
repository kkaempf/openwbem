/*******************************************************************************
* Copyright (C) 2005 Vintela, Inc. All rights reserved.
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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_Socket.hpp"
#include "OW_SocketImpl.hpp"
#include "OW_SSLException.hpp"
#include "OW_SSLSocketImpl.hpp"

// This code was pulled out of OW_Socket.cpp so that programs that use
// Socket but don't need SSL don't have to pull in the SSL libraries.

namespace OW_NAMESPACE
{

//////////////////////////////////////////////////////////////////////////////
Socket::Socket(const SSLClientCtxRef& sslCtx)
{
	if (sslCtx)
	{
#ifndef OW_NO_SSL
		m_impl = SocketBaseImplRef(new SSLSocketImpl(sslCtx));
#else
		OW_THROW(SSLException, "Not built with SSL");
#endif // #ifndef OW_NO_SSL
	}
	else
	{
		m_impl = SocketBaseImplRef(new SocketImpl);
	}
}

//////////////////////////////////////////////////////////////////////////////
Socket::Socket(SocketFlags::ESSLFlag isSSL)
{
	if (isSSL == SocketFlags::E_SSL)
	{
#ifndef OW_NO_SSL
		m_impl = SocketBaseImplRef(new SSLSocketImpl);
#else
		OW_THROW(SSLException, "Not built with SSL");
#endif // #ifndef OW_NO_SSL
	}
	else
	{
		m_impl = SocketBaseImplRef(new SocketImpl);
	}
}
//////////////////////////////////////////////////////////////////////////////
Socket::Socket(SocketHandle_t fd,
	SocketAddress::AddressType addrType, SocketFlags::ESSLFlag isSSL)
{
	if (isSSL == SocketFlags::E_SSL)
	{
#ifndef OW_NO_SSL
		m_impl = SocketBaseImplRef(new SSLSocketImpl(fd, addrType));
#else
		OW_THROW(SSLException, "Not built with SSL");
#endif // #ifndef OW_NO_SSL
	}
	else
	{
		m_impl = SocketBaseImplRef(new SocketImpl(fd, addrType));
	}
}
//////////////////////////////////////////////////////////////////////////////
// Used by ServerSocket2::accept()
Socket::Socket(SocketHandle_t fd,
	SocketAddress::AddressType addrType, const SSLServerCtxRef& sslCtx)
{
	if (sslCtx)
	{
#ifndef OW_NO_SSL
		m_impl = SocketBaseImplRef(new SSLSocketImpl(fd, addrType, sslCtx));
#else
		OW_THROW(SSLException, "Not built with SSL");
#endif // #ifndef OW_NO_SSL
	}
	else
	{
		m_impl = SocketBaseImplRef(new SocketImpl(fd, addrType));
	}
}
//////////////////////////////////////////////////////////////////////////////
Socket::Socket(const SocketAddress& addr, SocketFlags::ESSLFlag isSSL)
{
	if (isSSL == SocketFlags::E_SSL)
	{
#ifndef OW_NO_SSL
		m_impl = SocketBaseImplRef(new SSLSocketImpl(addr));
#else
		OW_THROW(SSLException, "Not built with SSL");
#endif // #ifndef OW_NO_SSL
	}
	else
	{
		m_impl = SocketBaseImplRef(new SocketImpl(addr));
	}
}

#ifndef OW_NO_SSL
SSL*
Socket::getSSL() const
{
	IntrusiveReference<SSLSocketImpl> sslsock = m_impl.cast_to<SSLSocketImpl>(); 
	if (!sslsock)
	{
		return 0; 
	}
	return sslsock->getSSL(); 
}

//////////////////////////////////////////////////////////////////////////////
bool
Socket::peerCertVerified() const
{
    IntrusiveReference<SSLSocketImpl> sslsock = m_impl.cast_to<SSLSocketImpl>(); 
    if (!sslsock)
    {
            return false; 
    }
    return sslsock->peerCertVerified(); 
}
#endif

} // namespace OW_NAMESPACE
