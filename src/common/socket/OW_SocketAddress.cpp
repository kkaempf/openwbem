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


#include "OW_config.h"
#include "OW_SocketAddress.hpp"
#include "OW_ByteSwap.hpp"
#include "OW_Assertion.hpp"

#include <netdb.h>
#include <iostream>

extern "C"
{
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/param.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <errno.h>
}


//static 
OW_SocketAddress 
OW_SocketAddress::getUDS(const OW_String& filename)
{
	OW_SocketAddress rval;
	rval.m_type = UDS;
	rval.m_name = filename;
	memset(&rval.m_UDSNativeAddress, 0, sizeof(rval.m_UDSNativeAddress));
	rval.m_UDSNativeAddress.sun_family = AF_LOCAL;
	strncpy(rval.m_UDSNativeAddress.sun_path, filename.c_str(), 
		sizeof(rval.m_UDSNativeAddress.sun_path) - 1);


#ifdef OW_SOLARIS
	rval.m_nativeSize = ::strlen(rval.m_UDSNativeAddress.sun_path) +
		offsetof(struct sockaddr_un, sun_path);
#elif defined OW_OPENUNIX
	rval.m_UDSNativeAddress.sun_len = sizeof(rval.m_UDSNativeAddress);
	rval.m_nativeSize = ::strlen(rval.m_UDSNativeAddress.sun_path) +
		offsetof(struct sockaddr_un, sun_path);
#else
	rval.m_nativeSize = sizeof(rval.m_UDSNativeAddress.sun_family) + 
		 ::strlen(rval.m_UDSNativeAddress.sun_path);
#endif
	return rval;
}

OW_SocketAddress::OW_SocketAddress()
	: m_nativeSize(0) , m_type(UNSET)
{
}


static OW_Mutex gethostbynameMutex;

//static 
OW_SocketAddress 
OW_SocketAddress::getByName(
		const OW_String& hostName, OW_UInt16 port)
	/*throw (OW_UnknownHostException)*/
{
#ifdef OW_HAVE_GETHOSTBYNAME_R
    hostent hostbuf;
    hostent* host = &hostbuf;
    char buf[2048];
    int h_err = 0;

    if (gethostbyname_r(hostName.c_str(), &hostbuf, buf, sizeof(buf),
                &host, &h_err) == -1)
        host = NULL;
#else
	hostent* host = NULL; 

    {
        OW_MutexLock mlock(gethostbynameMutex);
        host = gethostbyname(hostName.c_str());
    }

#endif
	if (!host) {
		OW_THROW(OW_UnknownHostException, hostName.c_str());
	}

	in_addr addr;
	memcpy(&addr, host->h_addr_list[0], sizeof(addr));
	return getFromNativeForm(addr, port, host->h_name);
}

//static
OW_SocketAddress 
OW_SocketAddress::getFromNativeForm( const OW_InetSocketAddress_t& nativeForm)
{
	return OW_SocketAddress(nativeForm);
}

//static
OW_SocketAddress 
OW_SocketAddress::getFromNativeForm( const OW_InetAddress_t& nativeForm, 
		OW_UInt16 nativePort, const OW_String& hostName)
{
	OW_InetSocketAddress_t addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = OW_hton16(nativePort);
	addr.sin_addr = nativeForm;

	OW_SocketAddress p = OW_SocketAddress(addr);
	p.m_type = INET;
	p.m_name = hostName;

	return p;
}

const OW_SocketAddress_t* OW_SocketAddress::getInetNativeForm() const
{
	return reinterpret_cast<const sockaddr*>(&m_inetNativeAddress);
}

OW_SocketAddress 
OW_SocketAddress::getAnyLocalHost(OW_UInt16 port)
{
	struct in_addr addr;
	addr.s_addr = OW_hton32(INADDR_ANY);
	OW_SocketAddress rval = getFromNativeForm(addr, port, "localhost");
	char buf[256];
	gethostname(buf, sizeof(buf));
    OW_String hname(buf);
    if (hname.indexOf('.') == -1)
    {
#ifdef OW_HAVE_GETHOSTBYNAME_R
        hostent hostbuf;
        hostent* hent = &hostbuf;
        char buf[2048];
        int h_err = 0;

        if (gethostbyname_r(hname.c_str(), &hostbuf, buf, sizeof(buf),
                    &hent, &h_err) == -1)
            hent = NULL;
#else
        hostent* hent = 0;
        {
            OW_MutexLock mlock(gethostbynameMutex);
            hent = gethostbyname(buf);
        }
#endif
        if (hent)
        {
            hname = OW_String(hent->h_name);
        }
    }
	rval.m_name = hname;
	return rval;
}

void OW_SocketAddress::assignFromNativeForm(
	const OW_InetSocketAddress_t* address, size_t /*size*/)
{
	m_type = INET;
	memcpy(&m_inetNativeAddress, address, sizeof(m_inetNativeAddress));
	m_address = inet_ntoa(m_inetNativeAddress.sin_addr);
	m_nativeSize = sizeof(m_inetNativeAddress);
}

OW_UInt16 OW_SocketAddress::getPort() const
{
	OW_ASSERT(m_type == INET);
	return OW_ntoh16(m_inetNativeAddress.sin_port);
}


OW_SocketAddress::OW_SocketAddress(const OW_InetSocketAddress_t& nativeForm)
	: m_nativeSize(0), m_type(INET)
{
	assignFromNativeForm(&nativeForm, sizeof(nativeForm));
}

const OW_String OW_SocketAddress::getName() const
{
	return m_name;
}

const OW_String OW_SocketAddress::getAddress() const
{
	return m_address;
}

size_t OW_SocketAddress::getNativeFormSize() const
{
	return m_nativeSize;
}


OW_SocketAddress OW_SocketAddress::allocEmptyAddress()
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	return OW_SocketAddress(OW_SocketAddress::getFromNativeForm(addr));
}

