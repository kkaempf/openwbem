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



#ifndef __OW_SocketADDRESS_HPP__
#define __OW_SocketADDRESS_HPP__

#include "OW_config.h"
#include "OW_Array.hpp"
#include "OW_Types.h"
#include "OW_NetworkTypes.hpp"
#include "OW_String.hpp"
#include "OW_Exception.hpp"

DEFINE_EXCEPTION(UnknownHost);
DEFINE_EXCEPTION(SocketAddress);

class OW_SocketAddress
{
public:

	enum AddressType
	{
		UNSET,
		INET,
		UDS
	};


	AddressType getType() const { return m_type; }
	/**
	 * Do a DNS lookup on a hostname and return an SocketAddress for that host
	 *
	 * @param host The hostname
	 * @param port The port
	 *
	 * @return An SocketAddress for the host and port
	 */
	static OW_SocketAddress getByName(const OW_String& host, unsigned short port = 0)
		/*throw (OW_UnknownHostException)*/;

	/**
	 * Do a DNS lookup on a hostname and return a list of all addresses
	 * that map to that hostname.
	 *
	 * @param host The hostname
	 * @param port The port
	 *
	 * @return An OW_Array of SocketAddresses for the host and port
	 */
	//static OW_Array<OW_SocketAddress> getAllByName(const OW_String& hostName,
	//		unsigned short port = 0);


	static OW_SocketAddress getUDS(const OW_String& filename);

	/**
	 * Get an OW_SocketAddress appropriate for referring to the local host
	 * @param port The port
	 * @return An OW_SocketAddress representing the local machine
	 */
	static OW_SocketAddress getAnyLocalHost(OW_UInt16 port = 0);

	/**
	 * Allocate an empty SocketAddress.
	 * @return an empty address
	 */
	static OW_SocketAddress allocEmptyAddress(AddressType type);

	/**
	 * Get the port associated with the address
	 * @return The port
	 */
	OW_UInt16 getPort() const;

	~OW_SocketAddress() {}


	/**
	 * Returns the hostname (FQDN) of the address.
	 *
	 * @return The hostname of the address.
	 */
	const OW_String getName() const;

	/**
	 * Returns the IP address of the host
	 *
	 * @return The IP address of the host
	 */

	const OW_String getAddress() const;

	const OW_SocketAddress_t* getNativeForm() const;


	size_t getNativeFormSize() const;


	/**
	 * Returns the IP address and the port with a colon in between.
	 *
	 * @return The IP and port seperated by a colon.
	 */
	const OW_String toString() const;

	void assignFromNativeForm(const OW_InetSocketAddress_t* address, size_t len);
	void assignFromNativeForm(const OW_UnixSocketAddress_t* address, size_t len);

	OW_SocketAddress();
	OW_SocketAddress(const OW_SocketAddress& arg)
		: m_name(arg.m_name)
		, m_address(arg.m_address)
		, m_nativeSize(arg.m_nativeSize)
		, m_inetNativeAddress(arg.m_inetNativeAddress)
		, m_UDSNativeAddress(arg.m_UDSNativeAddress)
		, m_type(arg.m_type)
	{
	}

	OW_SocketAddress& operator=(const OW_SocketAddress& arg)
	{
		m_name = arg.m_name;
		m_address = arg.m_address;
		m_nativeSize = arg.m_nativeSize;
		m_inetNativeAddress = arg.m_inetNativeAddress;
		m_UDSNativeAddress = arg.m_UDSNativeAddress;
		m_type = arg.m_type;
		return *this;
	}

private:
	OW_SocketAddress(const OW_InetSocketAddress_t& nativeForm);
	OW_SocketAddress(const OW_UnixSocketAddress_t& nativeForm);

	OW_String m_name;
	OW_String m_address;
	size_t m_nativeSize;
	

	OW_InetSocketAddress_t m_inetNativeAddress;
	OW_UnixSocketAddress_t m_UDSNativeAddress;
	AddressType m_type;


	static OW_SocketAddress getFromNativeForm(const OW_InetAddress_t& nativeForm,
			OW_UInt16 nativePort, const OW_String& hostname );
	static OW_SocketAddress getFromNativeForm(
		const OW_InetSocketAddress_t& nativeForm);
	static OW_SocketAddress getFromNativeForm(
		const OW_UnixSocketAddress_t& nativeForm);

};


#endif
