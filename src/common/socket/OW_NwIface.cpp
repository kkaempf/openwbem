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
extern "C"
{
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#ifdef OW_GNU_LINUX
#include <sys/ioctl.h>
#include <linux/if.h>
#include <string.h>
#elif defined (OW_OPENSERVER)
#include <string.h>
#include <stropts.h>
#include <net/if.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <paths.h>
#include <sys/mdi.h>
#else
#include <stropts.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/sockio.h>
#include <strings.h>
#include <fcntl.h>
#endif

} // extern "C"

// These need to be after the system includes because of some weird openserver
// include order problem
#include "OW_NwIface.hpp"
#include "OW_String.hpp"
#include "OW_Exception.hpp"
#include "OW_SocketUtils.hpp"

//////////////////////////////////////////////////////////////////////////////
OW_NwIface::OW_NwIface() /*throw (OW_SocketException)*/
{
	int s, lerrno;
	struct ifreq ifr;
	struct sockaddr_in *sin;

	if((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		OW_THROW(OW_SocketException, "socket");
	}

	getInterfaceName(s);

	bzero((char *)&ifr, sizeof(ifr));
	strncpy(ifr.ifr_name, m_name.c_str(), sizeof(ifr.ifr_name));

	////////////////////
	// Get IP address
	if(ioctl(s, SIOCGIFADDR, &ifr) < 0)
	{
		lerrno = errno;
		close(s);
		OW_THROW(OW_SocketException, "ioctl:SIOCGIFADDR");
	}
	sin = (struct sockaddr_in *) &ifr.ifr_addr;
	m_addr = sin->sin_addr.s_addr;

	////////////////////
	// Get the broadcast address
	// Testing
	if(ioctl(s, SIOCGIFBRDADDR, &ifr) < 0)
	{
		lerrno = errno;
		close(s);
		OW_THROW(OW_SocketException, "ioctl:SIOCGIFBRDADDR");
	}

	sin = (struct sockaddr_in*) &ifr.ifr_broadaddr;
	m_bcastAddr = sin->sin_addr.s_addr;

	////////////////////
	// Get net mask
	if(ioctl(s, SIOCGIFNETMASK, &ifr) < 0)
	{
		lerrno = errno;
		close(s);
		OW_THROW(OW_SocketException, "ioctl:SIOCGIFNETMASK");
	}

#ifdef OW_GNU_LINUX
	sin = (struct sockaddr_in *) &ifr.ifr_netmask;
#else
	sin = (struct sockaddr_in *) &ifr.ifr_broadaddr;
#endif
	m_netmask = sin->sin_addr.s_addr;
	close(s);
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_NwIface::getName()
{
	return m_name;
}

//////////////////////////////////////////////////////////////////////////////
unsigned long
OW_NwIface::getIPAddress()
{
	return m_addr;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_NwIface::getIPAddressString()
{
	return OW_SocketUtils::inetAddrToString(m_addr);
}
//////////////////////////////////////////////////////////////////////////////
unsigned long
OW_NwIface::getBroadcastAddress()
{
	return m_bcastAddr;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_NwIface::getBroadcastAddressString()
{
	return OW_SocketUtils::inetAddrToString(m_bcastAddr);
}
//////////////////////////////////////////////////////////////////////////////
/*
OW_String
OW_NwIface::getMACAddressString()
{
	return m_macAddress;
}
*/
//////////////////////////////////////////////////////////////////////////////
unsigned long
OW_NwIface::getNetmask()
{
	return m_netmask;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_NwIface::getNetmaskString()
{
	return OW_SocketUtils::inetAddrToString(m_netmask);
}
//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_NwIface::sameNetwork(unsigned long addr)
{
	return ((addr & m_netmask) == (m_addr & m_netmask));
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_NwIface::sameNetwork(const OW_String& straddr)
{
	return sameNetwork(stringToAddress(straddr));
}

//////////////////////////////////////////////////////////////////////////////
unsigned long
OW_NwIface::stringToAddress(const OW_String& straddr)
{
	return inet_addr(straddr.c_str());
}

//////////////////////////////////////////////////////////////////////////////
void
OW_NwIface::getInterfaceName(OW_SocketHandle_t sockfd) /*throw (OW_SocketException)*/
{
	char *p;
	int numreqs = 30;
	struct ifconf ifc;
	struct ifreq *ifr, ifrcopy;
	int n;
	int oldlen = -1;
	int lerrno = 0;
	const char* appliesTo;

	ifc.ifc_buf = NULL;
	for(;;)
	{
		ifc.ifc_len = sizeof(struct ifreq) * numreqs;
		if(ifc.ifc_buf == NULL)
		{
			ifc.ifc_buf = new char[ifc.ifc_len];
			if(ifc.ifc_buf == NULL)
			{
				lerrno = errno;
				appliesTo = "operator new";
				break;
			}
		}
		else
		{
 			p = new char[ifc.ifc_len];
			if(p == NULL)
			{
				lerrno = errno;
				appliesTo = "operator new";
				break;
			}

			memmove(p, ifc.ifc_buf, oldlen);
			delete [] ifc.ifc_buf;
			ifc.ifc_buf = p;
		}

		oldlen = ifc.ifc_len;

		if(ioctl(sockfd, SIOCGIFCONF, &ifc) < 0)
		{
			lerrno = errno;
			appliesTo = "ioctl:SIOCGIFCONF";
			break;
		}
		if(ifc.ifc_len == (int)(sizeof(struct ifreq) * numreqs))
		{
			/* assume it overflowed and try again */
			numreqs += 10;
			continue;
		}

		break;
	}

	if(lerrno == 0)
	{
		lerrno = ENODEV;
		appliesTo = "No interfaces found";
		ifr = ifc.ifc_req;

		for(n = 0; n < ifc.ifc_len; n += sizeof(struct ifreq))
		{
			ifrcopy = *ifr;

			if(ioctl(sockfd, SIOCGIFFLAGS, &ifrcopy) < 0)
			{
				lerrno = errno;
				appliesTo = "ioctl:SIOCGIFFLAGS";
				break;
			}

#ifdef OW_GNU_LINUX
			if((ifrcopy.ifr_flags & IFF_UP) && !(ifrcopy.ifr_flags & (IFF_LOOPBACK | IFF_DYNAMIC)))
#else
			if((ifrcopy.ifr_flags & IFF_UP))
#endif
			{
				m_name = ifr->ifr_name;
				lerrno = 0;
				break;
			}

			ifr++;
		}
	}

	if(ifc.ifc_buf != NULL)
	{
		delete [] ifc.ifc_buf;
	}

	if(lerrno != 0)
	{
		OW_THROW(OW_SocketException, "OW_NwIface::getInterfaceName");
	}
}

