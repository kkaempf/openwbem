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


#ifndef __OW_INETADDRESSIMPL_HPP__
#define __OW_INETADDRESSIMPL_HPP__



#include "OW_config.h"
#include "OW_NetworkTypes.hpp"
#include "OW_Reference.hpp"
#include "OW_Mutex.hpp"
#include "OW_Exception.hpp"
#include "OW_Types.h"
#include "OW_String.hpp"

class OW_UnknownHostException : public OW_Exception
{
public:
	OW_UnknownHostException() : OW_Exception() {}
	OW_UnknownHostException(const char* file, int line, const char* msg)
		: OW_Exception(file, line, msg){}
	OW_UnknownHostException(const char* msg) : OW_Exception(msg) {}
	virtual const char* type() const { return "OW_UnknownHostException"; }
};


class OW_InetAddressImpl;
typedef OW_Reference<OW_InetAddressImpl> OW_InetAddressRef;

class OW_InetAddressImpl
{
public:
	~OW_InetAddressImpl();
	static OW_InetAddressRef getByName(const OW_String& hostName,
			OW_UInt16 port)
		/*throw (OW_UnknownHostException)*/;

	static OW_InetAddressRef getFromNativeForm(
		const OW_InetSocketAddress_t& nativeForm);

	static OW_InetAddressRef getFromNativeForm(const OW_InetAddress_t& nativeForm,
			OW_UInt16 nativePort, const OW_String& hostname = "");

	static OW_InetAddressRef getAnyLocalHost(OW_UInt16 port);

	const OW_SocketAddress_t* getNativeForm() const;

	void assignFromNativeForm(const OW_InetSocketAddress_t* addr, size_t size);

	OW_InetAddressImpl* makeEmptyClone() const;

	OW_UInt16 getPort() const;
	void setPort(OW_UInt16 port);

	const OW_String& getName() const;

	const OW_String& getAddress() const;

	size_t getNativeFormSize() const;

	size_t getNativeFormMaxSize() const;

private:
	OW_InetAddressImpl();
	OW_InetAddressImpl(const OW_InetSocketAddress_t& nativeForm);

	OW_String m_name;
	OW_String m_address;
	size_t m_nativeSize;
	size_t m_nativeMaxSize;

	OW_InetSocketAddress_t m_nativeAddress;
};

#endif
