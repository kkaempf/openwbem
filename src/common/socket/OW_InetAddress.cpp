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
 */

#include "OW_config.h"

#include <netdb.h>

#include "OW_InetAddress.hpp"

OW_InetAddress::OW_InetAddress() { }

OW_InetAddress::OW_InetAddress(OW_InetAddressRef impl)
	: m_impl(impl) { }

OW_InetAddress OW_InetAddress::getByName(const OW_String& hostName,
		unsigned short port) /*throw (OW_UnknownHostException)*/
{
	return OW_InetAddress(OW_InetAddressImpl::getByName(hostName, port));
}

OW_InetAddress OW_InetAddress::getFromNativeForm(const OW_InetSocketAddress_t& nativeForm)
{
	return OW_InetAddress(OW_InetAddressImpl::getFromNativeForm(nativeForm));
}

OW_InetAddress OW_InetAddress::getFromNativeForm(const OW_InetAddress_t& nativeForm,
		unsigned short nativePort)
{
	return OW_InetAddress(OW_InetAddressImpl::getFromNativeForm(nativeForm,
				nativePort));
}

OW_InetAddress OW_InetAddress::getAnyLocalHost(unsigned short port)
{
	return OW_InetAddress(OW_InetAddressImpl::getAnyLocalHost(port));
}

/*
OW_Array<OW_InetAddress> OW_InetAddress::getAllByName(
		const OW_String& hostName,
		unsigned short port) 
{
	hostent hostbuf;
	hostent* host;
	char buf[2048];
	int h_err;

	if (gethostbyname_r(hostName.c_str(), &hostbuf, buf, sizeof(buf), &host, 
				&h_err) == -1)
		host = NULL;
	if (!host)
	{
		OW_THROW(OW_UnknownHostException, hostName.c_str());
	}

	OW_Array<OW_InetAddress> addrVect;

	in_addr addr;
	for (int i = 0; host->h_addr_list[i]; i++)
	{
		memcpy(&addr, host->h_addr_list[i], sizeof(addr));
		OW_InetAddressRef p = OW_InetAddressImpl::getFromNativeForm(addr,
				port, host->h_name);
		addrVect.push_back(OW_InetAddress(p));
	}
	return addrVect;
}
*/

OW_InetAddress OW_InetAddress::allocEmptyAddress()
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	return OW_InetAddress(OW_InetAddressImpl::getFromNativeForm(addr));
}

OW_UInt16 OW_InetAddress::getPort() const
{
	return m_impl->getPort();
}

void OW_InetAddress::setPort(OW_UInt16 port)
{
	m_impl->setPort(port);
}


