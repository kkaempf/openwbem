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

#ifndef OW_SOCKET_HPP_INCLUDE_GUARD_
#define OW_SOCKET_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_SelectableIFC.hpp"
#include "OW_SocketImpl.hpp"
#include "OW_SSLSocketImpl.hpp"
#include "OW_SocketBaseImpl.hpp"
#include "OW_String.hpp"
#include "OW_Types.h"
#include "OW_UnnamedPipe.hpp"
#include "OW_SocketFlags.hpp"

#define OW_DOMAIN_SOCKET_NAME "/tmp/OW@LCL@APIIPC_72859_Xq47Bf_P9r761-5_J-7_Q"

DECLARE_EXCEPTION(SocketTimeout)

class OW_Socket : public OW_SelectableIFC, public OW_IOIFC
{
public:

	/** Allocate a new  Socket
	 * @param isSSL is it an ssl socket?
	 */
	OW_Socket(OW_SocketFlags::ESSLFlag isSSL = OW_SocketFlags::E_NOT_SSL);

	/**
	 * Allocate a new  Socket based on an existing handle.
	 * This is used by OW_ServerSocket::accept()
	 * @param fd a handle to the existing socket
	 * @param isSSL is it an SSL socket?
	 */
	OW_Socket(OW_SocketHandle_t fd, OW_SocketAddress::AddressType addrType,
		OW_SocketFlags::ESSLFlag isSSL = OW_SocketFlags::E_NOT_SSL);

	/**
	 * Allocate a new OW_Socket and connect it to a peer machine
	 * @param addr the address of the peer machine
	 * @isSSL is it an SSL socket?
	 * @exception OW_SocketException
	 */
	OW_Socket(const OW_SocketAddress& addr, OW_SocketFlags::ESSLFlag isSSL = OW_SocketFlags::E_NOT_SSL);

	/**
	 * Copy ctor
	 */
	OW_Socket(const OW_Socket& arg)
		: OW_SelectableIFC()
		, OW_IOIFC()
		, m_impl(arg.m_impl) {}


	OW_Socket& operator=(const OW_Socket& arg)
		{ m_impl = arg.m_impl; return *this; }

	/**
	 * Connect to a peer node
	 * @param addr The address of the machine to connect to.
	 * @exception OW_SocketException
	 */
	void connect(const OW_SocketAddress& addr)
		{ m_impl->connect(addr); }

	/**
	 * Disconnect the (presumably) open connection
	 */
	void disconnect() { m_impl->disconnect(); }

	/**
	 * Set the receive timeout on the socket
	 * @param seconds the number of seconds for the receive timeout
	 */
	void setReceiveTimeout(int seconds) { m_impl->setReceiveTimeout(seconds);}

	/**
	 * Get the receive timeout
	 * @return The number of seconds of the receive timeout
	 */
	int getReceiveTimeout() { return m_impl->getReceiveTimeout(); }

	/**
	 * Set the send timeout on the socket
	 * @param seconds the number of seconds for the send timeout
	 */
	void setSendTimeout(int seconds) { m_impl->setSendTimeout(seconds); }

	/**
	 * Get the send timeout
	 * @return The number of seconds of the send timeout
	 */
	int getSendTimeout() { return m_impl->getSendTimeout(); }

	/**
	 * Set the connect timeout on the socket
	 * @param seconds the number of seconds for the connect timeout
	 */
	void setConnectTimeout(int seconds) { m_impl->setConnectTimeout(seconds); }

	/**
	 * Get the connect timeout
	 * @return The number of seconds of the connect timeout
	 */
	int getConnectTimeout() { return m_impl->getConnectTimeout(); }

	/**
	 * Set all timeouts (send, receive, connect)
	 * @param seconds the number of seconds for the timeouts
	 */
	void setTimeouts(int seconds) { m_impl->setTimeouts(seconds); }

	/**
	 * Has the receive timeout expired?
	 * @return true if the receive timeout has expired.
	 */
	bool receiveTimeOutExpired() { return m_impl->receiveTimeOutExpired(); }

	/**
	 * Write some data to the socket.
	 * @param dataOut a pointer to the memory to be written to the socket.
	 * @param dataOutLen the length of the data to be written
	 * @param errorAsException true if errors should throw exceptions.
	 * @return the number of bytes written.
	 * @throws OW_SocketException
	 */
	int write(const void* dataOut, int dataOutLen, bool errorAsException=false)
		{ return m_impl->write(dataOut, dataOutLen, errorAsException); }

	/**
	 * Read from the socket
	 * @param dataIn a pointer to a buffer where data should be copied to
	 * @param dataInLen the number of bytes to read.
	 * @param errorAsException true if errors should throw exceptions.
	 * @return the number of bytes read.
	 * @throws OW_SocketException
	 */
	int read(void* dataIn, int dataInLen, bool errorAsException=false)
		{ return m_impl->read(dataIn, dataInLen, errorAsException); }

	/**
	 * Wait for input on the socket for a specified length of time.
	 * @param timeOutSecs the number of seconds to wait.
	 * 	-1 means infinite
	 * @return true if the timeout expired
	 * @throws OW_SocketException
	 */
	bool waitForInput(int timeOutSecs=-1)
		{ return m_impl->waitForInput(timeOutSecs); }

	/**
	 * Wait for output on the socket for a specified length of time.
	 * @param timeOutSecs the number of seconds to wait.
	 * 	-1 means infinite
	 * @return true if the timeout expired
	 * @throws OW_SocketException
	 */
	bool waitForOutput(int timeOutSecs=-1)
		{ return m_impl->waitForOutput(timeOutSecs); }

	/**
	 * Get the local address associated with the socket connection
	 * @return an OW_SocketAddress representing the local machine
	 */
	OW_SocketAddress getLocalAddress() const { return m_impl->getLocalAddress(); }

	/**
	 * Get the peer address associated with the socket connection
	 * @return an OW_SocketAddress representing the peer machine
	 */
	OW_SocketAddress getPeerAddress() const { return m_impl->getPeerAddress(); }

	/**
	 * Get an istream to read from the socket
	 * @return a istream& which can be used for socket input
	 * @throws OW_SocketException
	 */
	std::istream& getInputStream()
		{ return m_impl->getInputStream(); }

	/**
	 * Get an ostream to write to the socket
	 * @return an ostream& which can be used for socket output
	 * @throws OW_SocketException
	 */
	std::ostream& getOutputStream()
		{ return m_impl->getOutputStream(); }

	/**
	 * Get an iostream to read/write from/to the socket
	 * @return an iostream& which can be used for socket input and output.
	 * @throws OW_SocketException
	 */
	std::iostream& getIOStream()
		{ return m_impl->getIOStream(); }


	/**
	 * @return The OW_Select_t associated with this sockect.
	 */
	OW_Select_t getSelectObj() const { return m_impl->getSelectObj(); }

	/**
	 * Get the socket handle for the socket
	 * @return the socket handle
	 */
	OW_SocketHandle_t getfd() { return m_impl->getfd(); }

	static OW_UnnamedPipeRef m_pUpipe;

	static void createShutDownMechanism();

	/**
	 * Call this to shutdown all sockets.  This is usefull when the CIMOM
	 * is shutting down.  We want any outstanding connections to close
	 * immediately.
	 */
	static void shutdownAllSockets();

	/**
	 * Have the sockets been shutdown?
	 * @return true if the sockets have been shutdown
	 */
	static bool gotShutDown();
	static void deleteShutDownMechanism();

private:
	OW_SocketBaseImplRef m_impl;
};

#endif


