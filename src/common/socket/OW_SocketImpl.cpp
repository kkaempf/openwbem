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

/**
 * @name		OW_SocketImpl.cpp
 * @author	J. Bart Whiteley
 *
 * @description
 *		Implementation file for the OW_SocketImpl class.
 */
#include "OW_config.h"
#include "OW_SocketImpl.hpp"

#ifdef OW_USE_GNU_PTH
extern "C"
{
#include <pth.h>
}
#endif


//////////////////////////////////////////////////////////////////////////////
OW_SocketImpl::OW_SocketImpl() 
	: OW_SocketBaseImpl()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_SocketImpl::OW_SocketImpl(OW_SocketHandle_t fd, OW_SocketAddress::AddressType addrType)
	: OW_SocketBaseImpl(fd, addrType)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_SocketImpl::OW_SocketImpl(const OW_SocketAddress addr) 
	: OW_SocketBaseImpl(addr)
{
}
//////////////////////////////////////////////////////////////////////////////
OW_SocketImpl::~OW_SocketImpl()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_Select_t
OW_SocketImpl::getSelectObj() const
{
	return m_sockfd;
}

//////////////////////////////////////////////////////////////////////////////
int OW_SocketImpl::readAux(void* dataIn, int dataInLen) 
{
#ifdef OW_USE_GNU_PTH
	return ::read(m_sockfd, dataIn, dataInLen);
#else
	return ::read(m_sockfd, dataIn, dataInLen);
#endif
}

//////////////////////////////////////////////////////////////////////////////
int OW_SocketImpl::writeAux(const void* dataOut, int dataOutLen)
{
#ifdef OW_USE_GNU_PTH
	return ::write(m_sockfd, dataOut, dataOutLen);
#else
	return ::write(m_sockfd, dataOut, dataOutLen);
#endif
}

//////////////////////////////////////////////////////////////////////////////

