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
#ifndef OW_SocketADDRESS_HPP_INCLUDE_GUARD_
#define OW_SocketADDRESS_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Array.hpp"
#include "OW_Types.hpp"
#include "OW_NetworkTypes.hpp"
#include "OW_String.hpp"
#include "OW_Exception.hpp"

namespace OpenWBEM
{

DECLARE_EXCEPTION(UnknownHost);
DECLARE_EXCEPTION(SocketAddress);
class SocketAddress
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
	 * @throws UnknownHostException
	 */
	static SocketAddress getByName(const String& host, unsigned short port = 0);
	/**
	 * Do a DNS lookup on a hostname and return a list of all addresses
	 * that map to that hostname.
	 *
	 * @param host The hostname
	 * @param port The port
	 *
	 * @return An Array of SocketAddresses for the host and port
	 */
	//static Array<SocketAddress> getAllByName(const String& hostName,
	//		unsigned short port = 0);
	static SocketAddress getUDS(const String& filename);
	/**
	 * Get an SocketAddress appropriate for referring to the local host
	 * @param port The port
	 * @return An SocketAddress representing the local machine
	 */
	static SocketAddress getAnyLocalHost(UInt16 port = 0);
	/**
	 * Allocate an empty SocketAddress.
	 * @return an empty address
	 */
	static SocketAddress allocEmptyAddress(AddressType type);
	/**
	 * Get the port associated with the address
	 * @return The port
	 */
	UInt16 getPort() const;
	~SocketAddress() {}
	/**
	 * Returns the hostname (FQDN) of the address.
	 *
	 * @return The hostname of the address.
	 */
	const String getName() const;
	/**
	 * Returns the IP address of the host
	 *
	 * @return The IP address of the host
	 */
	const String getAddress() const;
	const SocketAddress_t* getNativeForm() const;
	size_t getNativeFormSize() const;
	/**
	 * Returns the IP address and the port with a colon in between.
	 *
	 * @return The IP and port seperated by a colon.
	 */
	const String toString() const;
	void assignFromNativeForm(const InetSocketAddress_t* address, size_t len);
#if !defined(OW_WIN32)
	void assignFromNativeForm(const UnixSocketAddress_t* address, size_t len);
#endif
	SocketAddress();
private:
	SocketAddress(const InetSocketAddress_t& nativeForm);
#if !defined(OW_WIN32)
	SocketAddress(const UnixSocketAddress_t& nativeForm);
#endif
	String m_name;
	String m_address;
	size_t m_nativeSize;
	
	InetSocketAddress_t m_inetNativeAddress;
#if !defined(OW_WIN32)
	UnixSocketAddress_t m_UDSNativeAddress;
#endif
	AddressType m_type;
	static SocketAddress getFromNativeForm(const InetAddress_t& nativeForm,
			UInt16 nativePort, const String& hostname );
	static SocketAddress getFromNativeForm(
		const InetSocketAddress_t& nativeForm);
#if !defined(OW_WIN32)
	static SocketAddress getFromNativeForm(
		const UnixSocketAddress_t& nativeForm);
#endif
};

} // end namespace OpenWBEM

#endif
