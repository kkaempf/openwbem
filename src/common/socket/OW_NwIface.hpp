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
 * @name		OW_NwIface.hpp
 * @author	Jon M. Carey
 * @company	Caldera Systems, Inc.
 * @since	6/12/2000
 *
 * @description
 *		This is the interface file for the OW_NwIface class.
 *		The purpose of the OW_NwIface class is to provide useful utility
 *		functions pertaining to the network interface.
 */
#ifndef __OW_NWIFACE_HPP__
#define __OW_NWIFACE_HPP__

#include "OW_config.h"

#include "OW_SocketException.hpp"
#include "OW_String.hpp"
#include "OW_NetworkTypes.hpp"

class OW_NwIface
{
public:
	OW_NwIface() /*throw (OW_SocketException)*/;
	OW_String getName();
	unsigned long getIPAddress();
	OW_String getIPAddressString();
	unsigned long getBroadcastAddress();
	OW_String getBroadcastAddressString();
//	OW_String getMACAddressString();
	unsigned long getNetmask();
	OW_String getNetmaskString();
	OW_Bool sameNetwork(unsigned long addr);
	OW_Bool sameNetwork(const OW_String& straddr);
	static unsigned long stringToAddress(const OW_String& straddr);

private:
	void getInterfaceName(OW_SocketHandle_t sockfd) /*throw (OW_SocketException)*/;

	unsigned long m_addr;
	unsigned long m_bcastAddr;
	unsigned long m_netmask;
//	OW_String m_macAddress;
	OW_String m_name;
};


#endif	// __NWIFACE_HPP__


