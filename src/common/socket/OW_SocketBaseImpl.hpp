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

/**
 * @name		OW_SocketBaseImpl.hpp
 * @author	Jon M. Carey
 *
 * @description
 *		Interface file for the OW_SocketBaseImpl class
 */
#ifndef OW_SOCKETBASEIMPL_HPP_INCLUDE_GUARD_
#define OW_SOCKETBASEIMPL_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_SelectableIFC.hpp"
#include "OW_Reference.hpp"
#include "OW_SocketException.hpp"
#include "OW_String.hpp"
#include "OW_SocketAddress.hpp"
#include "OW_Types.hpp"
#include "OW_SocketStreamBuffer.hpp"
#include "OW_IOIFC.hpp"
#if defined(OW_HAVE_ISTREAM) && defined(OW_HAVE_OSTREAM)
#include <istream>
#include <ostream>
#else
#include <iostream>
#endif


class OW_SocketBaseImpl;

typedef OW_Reference<OW_SocketBaseImpl> OW_SocketBaseImplRef;

class OW_SocketBaseImpl : public OW_SelectableIFC, public OW_IOIFC
{
public:

	OW_SocketBaseImpl();
	OW_SocketBaseImpl(OW_SocketHandle_t fd, OW_SocketAddress::AddressType addrType);
	OW_SocketBaseImpl(const OW_SocketAddress& addr);
	virtual ~OW_SocketBaseImpl();

	virtual void connect(const OW_SocketAddress& addr);
	virtual void disconnect();
	void setReceiveTimeout(int seconds) { m_recvTimeout = seconds; }
	int getReceiveTimeout() { return m_recvTimeout; }
	void setSendTimeout(int seconds) { m_sendTimeout = seconds; }
	int getSendTimeout() { return m_sendTimeout; }
	void setConnectTimeout(int seconds) { m_connectTimeout = seconds; }
	int getConnectTimeout() { return m_connectTimeout; }
	void setTimeouts(int seconds) { m_recvTimeout = m_sendTimeout = m_connectTimeout = seconds; }
	bool receiveTimeOutExpired() { return m_recvTimeoutExprd; }

	int write(const void* dataOut, int dataOutLen,
			bool errorAsException=false);
	int read(void* dataIn, int dataInLen,
			bool errorAsException=false);

	bool waitForInput(int timeOutSecs=-1);
	bool waitForOutput(int timeOutSecs=-1);
	std::istream& getInputStream();
	std::ostream& getOutputStream();
	std::iostream& getIOStream();
	OW_SocketAddress getLocalAddress() const { return m_localAddress; }
	OW_SocketAddress getPeerAddress() const { return m_peerAddress; }
	OW_SocketHandle_t getfd() { return m_sockfd; }

	OW_Select_t getSelectObj() const;

	static void setDumpFiles(const OW_String& in, const OW_String& out);
protected:
	virtual int readAux(void* dataIn, int dataInLen) = 0;
	virtual int writeAux(const void* dataOut, int dataOutLen) = 0;

	bool m_isConnected;
	OW_SocketHandle_t m_sockfd;
	OW_SocketAddress m_localAddress;
	OW_SocketAddress m_peerAddress;

private:
	void fillInetAddrParms();
	void fillUnixAddrParms();

	bool m_recvTimeoutExprd;
	OW_SocketStreamBuffer m_streamBuf;
	std::istream m_in;
	std::ostream m_out;
	std::iostream m_inout;
	int m_recvTimeout;
	int m_sendTimeout;
	int m_connectTimeout;
	
	static OW_String m_traceFileOut;
	static OW_String m_traceFileIn;

	OW_SocketBaseImpl(const OW_SocketBaseImpl& arg);
	OW_SocketBaseImpl& operator= (const OW_SocketBaseImpl& arg);
};

#endif


