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
 *
 *
 *
 */

#include "OW_config.h"
#include "OW_Socket.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_Assertion.hpp"
#include "OW_MutexLock.hpp"
#include "OW_SSLException.hpp"
#include "OW_Exception.hpp"
#include "OW_IOException.hpp"

OW_UnnamedPipeRef OW_Socket::m_pUpipe;

OW_Socket::OW_Socket(OW_Bool isSSL)
{
	if (isSSL)
	{
#ifndef OW_NO_SSL
		m_impl = OW_SocketBaseImplRef(new OW_SSLSocketImpl);
#else
		OW_THROW(OW_SSLException, "Not built with SSL");
#endif // #ifndef OW_NO_SSL
	}
	else
	{
		m_impl = OW_SocketBaseImplRef(new OW_SocketImpl);
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Socket::OW_Socket(OW_SocketHandle_t fd,
	OW_SocketAddress::AddressType addrType, OW_Bool isSSL)
		/*throw (OW_SocketException)*/
{
	if (isSSL)
	{
#ifndef OW_NO_SSL
		m_impl = OW_SocketBaseImplRef(new OW_SSLSocketImpl(fd, addrType));
#else
		OW_THROW(OW_SSLException, "Not built with SSL");
#endif // #ifndef OW_NO_SSL
	}
	else
	{
		m_impl = OW_SocketBaseImplRef(new OW_SocketImpl(fd, addrType));
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_Socket::OW_Socket(const OW_SocketAddress& addr, OW_Bool isSSL)
		/*throw (OW_SocketException)*/
{
	if (isSSL)
#ifndef OW_NO_SSL
		m_impl = OW_SocketBaseImplRef(new OW_SSLSocketImpl(addr));
#else
		OW_THROW(OW_SSLException, "Not built with SSL");
#endif // #ifndef OW_NO_SSL
	else
	{
		m_impl = OW_SocketBaseImplRef(new OW_SocketImpl(addr));
	}
}

static OW_Bool b_gotShutDown = false;
static OW_Mutex shutdownMutex;

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_Socket::shutdownAllSockets()
{
	OW_MutexLock mlock(shutdownMutex);
	OW_ASSERT(m_pUpipe);
	b_gotShutDown = true;
	if (m_pUpipe->writeString("die!") == -1)
	{
		OW_THROW(OW_IOException, "Failed writing to socket shutdown pipe");
	}
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_Socket::createShutDownMechanism()
{
	OW_MutexLock mlock(shutdownMutex);
	OW_ASSERT(!m_pUpipe);
	m_pUpipe = OW_UnnamedPipe::createUnnamedPipe();
	b_gotShutDown = false;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
void
OW_Socket::deleteShutDownMechanism()
{
	OW_MutexLock mlock(shutdownMutex);
	OW_ASSERT(m_pUpipe);
	m_pUpipe = 0;
}

//////////////////////////////////////////////////////////////////////////////
// STATIC
OW_Bool
OW_Socket::gotShutDown()
{
	OW_MutexLock mlock(shutdownMutex);
	return b_gotShutDown;
}


