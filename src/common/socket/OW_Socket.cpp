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
#include "OW_config.h"
#include "OW_Socket.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_Assertion.hpp"
#include "OW_MutexLock.hpp"
#include "OW_SSLException.hpp"
#include "OW_Exception.hpp"
#include "OW_IOException.hpp"

namespace OpenWBEM
{

OW_DEFINE_EXCEPTION(Socket)
OW_DEFINE_EXCEPTION(SocketTimeout)
UnnamedPipeRef Socket::m_pUpipe;
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
Socket::Socket(const SocketAddress& addr, SocketFlags::ESSLFlag isSSL)
{
	if (isSSL == SocketFlags::E_SSL)
#ifndef OW_NO_SSL
		m_impl = SocketBaseImplRef(new SSLSocketImpl(addr));
#else
		OW_THROW(SSLException, "Not built with SSL");
#endif // #ifndef OW_NO_SSL
	else
	{
		m_impl = SocketBaseImplRef(new SocketImpl(addr));
	}
}
static bool b_gotShutDown = false;
static Mutex shutdownMutex;
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
Socket::shutdownAllSockets()
{
	MutexLock mlock(shutdownMutex);
	OW_ASSERT(m_pUpipe);
	b_gotShutDown = true;
	if (m_pUpipe->writeString("die!") == -1)
	{
		OW_THROW(IOException, "Failed writing to socket shutdown pipe");
	}
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
Socket::createShutDownMechanism()
{
	MutexLock mlock(shutdownMutex);
	OW_ASSERT(!m_pUpipe);
	m_pUpipe = UnnamedPipe::createUnnamedPipe();
	m_pUpipe->setBlocking(UnnamedPipe::E_NONBLOCKING);
	b_gotShutDown = false;
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
void
Socket::deleteShutDownMechanism()
{
	MutexLock mlock(shutdownMutex);
	OW_ASSERT(m_pUpipe);
	m_pUpipe = 0;
}
//////////////////////////////////////////////////////////////////////////////
// STATIC
bool
Socket::gotShutDown()
{
	MutexLock mlock(shutdownMutex);
	return b_gotShutDown;
}

} // end namespace OpenWBEM

