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

/**
 * @name		OW_NwIface.hpp
 * @author	Jon M. Carey
 * @author Dan Nuffer
 *
 * @description
 *		This is the interface file for the OW_NwIface class.
 *		The purpose of the OW_NwIface class is to provide useful utility
 *		functions pertaining to the network interface.
 */
#ifndef OW_NWIFACE_HPP_INCLUDE_GUARD_
#define OW_NWIFACE_HPP_INCLUDE_GUARD_
#include "OW_config.h"

#if !defined(OW_WIN32) && !defined(OW_NETWARE)

#include "OW_SocketException.hpp"
#include "OW_String.hpp"
#include "OW_NetworkTypes.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

class OW_DEPRECATED NwIface
{
public:
	/**
	 * @throws SocketException
	 */
	NwIface();
	String getName();
	unsigned long getIPAddress();
	String getIPAddressString();
	unsigned long getBroadcastAddress();
	String getBroadcastAddressString();
//	String getMACAddressString();
	unsigned long getNetmask();
	String getNetmaskString();
	bool sameNetwork(unsigned long addr);
	bool sameNetwork(const String& straddr);
	static unsigned long stringToAddress(const String& straddr);
private:
	/**
	 * @throws SocketException
	 */
	void getInterfaceName(SocketHandle_t sockfd);
	unsigned long m_addr;
	unsigned long m_bcastAddr;
	unsigned long m_netmask;
//	String m_macAddress;
	String m_name;
};

} // end namespace OW_NAMESPACE

#endif	// #if !defined(OW_WIN32) && !defined(OW_NETWARE)

#endif
