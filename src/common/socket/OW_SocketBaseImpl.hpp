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
 * @name		OW_SocketBaseImpl.hpp
 * @author	Jon M. Carey
 * @author Dan Nuffer
 *
 * @description
 *		Interface file for the OW_SocketBaseImpl class
 */
#ifndef OW_SOCKETBASEIMPL_HPP_INCLUDE_GUARD_
#define OW_SOCKETBASEIMPL_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_SelectableIFC.hpp"
#include "OW_IntrusiveReference.hpp"
#include "OW_SocketException.hpp"
#include "OW_String.hpp"
#include "OW_SocketAddress.hpp"
#include "OW_Types.hpp"
#include "OW_SocketStreamBuffer.hpp"
#include "OW_IOIFC.hpp"
#include "OW_Timeout.hpp"

#if defined(OW_HAVE_ISTREAM) && defined(OW_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

class OW_COMMON_API SocketBaseImpl : public SelectableIFC, public IOIFC
{
public:
	SocketBaseImpl();
	SocketBaseImpl(SocketHandle_t fd, SocketAddress::AddressType addrType);
	SocketBaseImpl(const SocketAddress& addr);
	virtual ~SocketBaseImpl();
	virtual void connect(const SocketAddress& addr);
	virtual void disconnect();
	void setReceiveTimeout(const Timeout& timeout) { m_recvTimeout = timeout; }
	Timeout getReceiveTimeout() const { return m_recvTimeout; }
	void setSendTimeout(const Timeout& timeout) { m_sendTimeout = timeout; }
	Timeout getSendTimeout() const { return m_sendTimeout; }
	void setConnectTimeout(const Timeout& timeout) { m_connectTimeout = timeout; }
	Timeout getConnectTimeout() const { return m_connectTimeout; }
	void setTimeouts(const Timeout& timeout) { m_recvTimeout = m_sendTimeout = m_connectTimeout = timeout; }
	bool receiveTimeOutExpired() const { return m_recvTimeoutExprd; }
	int write(const void* dataOut, int dataOutLen,
			ErrorAction errorAsException = E_RETURN_ON_ERROR);
	int read(void* dataIn, int dataInLen,
			ErrorAction errorAsException = E_RETURN_ON_ERROR);
	virtual bool waitForInput(const Timeout& timeout);
	bool waitForOutput(const Timeout& timeout);
	std::istream& getInputStream();
	std::ostream& getOutputStream();
	std::iostream& getIOStream();
	SocketAddress getLocalAddress() const { return m_localAddress; }
	SocketAddress getPeerAddress() const { return m_peerAddress; }
	SocketHandle_t getfd() const { return m_sockfd; }
	Select_t getSelectObj() const;
	bool isConnected() const { return m_isConnected; }
	static void setDumpFiles(const String& in, const String& out);
protected:
	virtual int readAux(void* dataIn, int dataInLen) = 0;
	virtual int writeAux(const void* dataOut, int dataOutLen) = 0;

	bool m_isConnected;
	SocketHandle_t m_sockfd;
	SocketAddress m_localAddress;
	SocketAddress m_peerAddress;
#if defined(OW_WIN32)
	HANDLE m_event;
#endif

private:
	void fillInetAddrParms();
#if !defined(OW_WIN32)
	void fillUnixAddrParms();
#endif
	SocketBaseImpl(const SocketBaseImpl& arg);
	SocketBaseImpl& operator= (const SocketBaseImpl& arg);
#if defined(OW_WIN32)
	static int waitForEvent(HANDLE event, int secsToTimeout=-1);
#endif

	bool m_recvTimeoutExprd;
	SocketStreamBuffer m_streamBuf;
	std::istream m_in;
	std::ostream m_out;
	std::iostream m_inout;
	Timeout m_recvTimeout;
	Timeout m_sendTimeout;
	Timeout m_connectTimeout;
	
	static String m_traceFileOut;
	static String m_traceFileIn;
};
OW_EXPORT_TEMPLATE(OW_COMMON_API, IntrusiveReference, SocketBaseImpl);

} // end namespace OW_NAMESPACE

#endif
