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



#ifndef __OW_INETADDRESS_HPP__
#define __OW_INETADDRESS_HPP__

#include "OW_config.h"
#include "OW_InetAddressImpl.hpp"
#include "OW_Array.hpp"
#include "OW_Reference.hpp"


class OW_InetAddress 
{
public:
	/**
	 * Do a DNS lookup on a hostname and return an InetAddress for that host
	 *
	 * @param host The hostname
	 * @param port The port
	 *
	 * @return An InetAddress for the host and port
	 */
	static OW_InetAddress getByName(const OW_String& host, unsigned short port = 0)
		/*throw (OW_UnknownHostException)*/;

	/**
	 * Do a DNS lookup on a hostname and return a list of all addresses
	 * that map to that hostname.
	 *
	 * @param host The hostname
	 * @param port The port
	 *
	 * @return An OW_Array of InetAddresses for the host and port
	 */
	//static OW_Array<OW_InetAddress> getAllByName(const OW_String& hostName,
	//		unsigned short port = 0);

	static OW_InetAddress getFromNativeForm(
		const OW_InetSocketAddress_t& nativeForm);

	static OW_InetAddress getFromNativeForm(const OW_InetAddress_t& nativeForm,
			unsigned short nativePort = 0);

	/**
	 * Get an OW_InetAddress appropriate for referring to the local host
	 * @param port The port
	 * @return An OW_InetAddress representing the local machine
	 */
	static OW_InetAddress getAnyLocalHost(unsigned short port = 0);

	/**
	 * Allocate an empty InetAddress.
	 * @return an empty address
	 */
	static OW_InetAddress allocEmptyAddress();

	/**
	 * Get the port associated with the address
	 * @return The port
	 */
	OW_UInt16 getPort() const;

	/**
	 * Set the port on the inet address
	 * @param port The port
	 */
	void setPort(OW_UInt16 port);

	OW_InetAddress();
	~OW_InetAddress() {}


	/**
	 * Returns the hostname (FQDN) of the address.
	 *
	 * @return The hostname of the address.
	 */
	const OW_String getName() const
	{
		 return m_impl->getName();
	}

	/**
	 * Returns the IP address of the host
	 *
	 * @return The IP address of the host
	 */

	const OW_String getAddress() const 
	{
		 return m_impl->getAddress();
	}

	const OW_SocketAddress_t* getNativeForm() const
	{
		 return m_impl->getNativeForm();
	}


	size_t getNativeFormSize() const
	{
		 return m_impl->getNativeFormSize();
	}

	size_t getNativeFormMaxSize() const
	{
		 return m_impl->getNativeFormMaxSize();
	}

	void assignFromNativeForm(OW_InetSocketAddress_t* address, size_t len)
	{
		 m_impl->assignFromNativeForm(address, len);
	}

	OW_InetAddressImpl* makeEmptyClone() const
	{
		 return m_impl->makeEmptyClone();
	}

	/** 
	 * Returns the IP address and the port with a colon in between.
	 *
	 * @return The IP and port seperated by a colon.
	 */
	const OW_String toString() const
	{
		return getAddress() + ":" + OW_String(OW_UInt32(getPort()));
	}

	operator void*() const { return (void*)m_impl; }

private:
	OW_InetAddress(OW_InetAddressRef impl);
	OW_InetAddressRef m_impl;
};


#endif
