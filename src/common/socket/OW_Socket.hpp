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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#ifndef OW_SOCKET_HPP_INCLUDE_GUARD_
#define OW_SOCKET_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_SelectableIFC.hpp"
#include "OW_SocketBaseImpl.hpp"
#include "OW_String.hpp"
#include "OW_Types.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_SocketFlags.hpp"
#include "OW_NetworkTypes.hpp"
#include "OW_SocketAddress.hpp"
#include "OW_IntrusiveReference.hpp"


// TODO: This is duplicated in OW_ConfigOpts.hpp.  Figure out a way to merge the 2 without drastically increasing header dependencies.
#define OW_DOMAIN_SOCKET_NAME "/tmp/OW@LCL@APIIPC_72859_Xq47Bf_P9r761-5_J-7_Q"OW_PACKAGE_PREFIX
//#define OW_DOMAIN_SOCKET_NAME OW_DEFAULT_UDS_FILENAME

namespace OpenWBEM
{

class SocketBaseImpl;
typedef IntrusiveReference<SocketBaseImpl> SocketBaseImplRef;


OW_DECLARE_APIEXCEPTION(SocketTimeout, OW_COMMON_API)
class OW_COMMON_API Socket : public SelectableIFC, public IOIFC
{
public:
	/** Allocate a new  Socket
	 * @param isSSL is it an ssl socket?
	 */
	Socket(SocketFlags::ESSLFlag isSSL = SocketFlags::E_NOT_SSL);
	/**
	 * Allocate a new  Socket based on an existing handle.
	 * This is used by ServerSocket::accept()
	 * @param fd a handle to the existing socket
	 * @param isSSL is it an SSL socket?
	 */
	Socket(SocketHandle_t fd, SocketAddress::AddressType addrType,
		SocketFlags::ESSLFlag isSSL = SocketFlags::E_NOT_SSL);
	/**
	 * Allocate a new Socket and connect it to a peer machine
	 * @param addr the address of the peer machine
	 * @isSSL is it an SSL socket?
	 * @exception SocketException
	 */
	Socket(const SocketAddress& addr, SocketFlags::ESSLFlag isSSL = SocketFlags::E_NOT_SSL);
	/**
	 * Copy ctor
	 */
	Socket(const Socket& arg)
		: SelectableIFC()
		, IOIFC()
		, m_impl(arg.m_impl) {}
	Socket& operator=(const Socket& arg)
		{ m_impl = arg.m_impl; return *this; }
	/**
	 * Connect to a peer node
	 * @param addr The address of the machine to connect to.
	 * @exception SocketException
	 */
	void connect(const SocketAddress& addr)
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
	 * @throws SocketException
	 */
	int write(const void* dataOut, int dataOutLen, bool errorAsException=false)
		{ return m_impl->write(dataOut, dataOutLen, errorAsException); }
	/**
	 * Read from the socket
	 * @param dataIn a pointer to a buffer where data should be copied to
	 * @param dataInLen the number of bytes to read.
	 * @param errorAsException true if errors should throw exceptions.
	 * @return the number of bytes read.
	 * @throws SocketException
	 */
	int read(void* dataIn, int dataInLen, bool errorAsException=false)
		{ return m_impl->read(dataIn, dataInLen, errorAsException); }
	/**
	 * Wait for input on the socket for a specified length of time.
	 * @param timeOutSecs the number of seconds to wait.
	 * 	-1 means infinite
	 * @return true if the timeout expired
	 * @throws SocketException
	 */
	bool waitForInput(int timeOutSecs=-1)
		{ return m_impl->waitForInput(timeOutSecs); }
	/**
	 * Wait for output on the socket for a specified length of time.
	 * @param timeOutSecs the number of seconds to wait.
	 * 	-1 means infinite
	 * @return true if the timeout expired
	 * @throws SocketException
	 */
	bool waitForOutput(int timeOutSecs=-1)
		{ return m_impl->waitForOutput(timeOutSecs); }
	/**
	 * Get the local address associated with the socket connection
	 * @return an SocketAddress representing the local machine
	 */
	SocketAddress getLocalAddress() const { return m_impl->getLocalAddress(); }
	/**
	 * Get the peer address associated with the socket connection
	 * @return an SocketAddress representing the peer machine
	 */
	SocketAddress getPeerAddress() const { return m_impl->getPeerAddress(); }
	/**
	 * Get an istream to read from the socket
	 * @return a istream& which can be used for socket input
	 * @throws SocketException
	 */
	std::istream& getInputStream()
		{ return m_impl->getInputStream(); }
	/**
	 * Get an ostream to write to the socket
	 * @return an ostream& which can be used for socket output
	 * @throws SocketException
	 */
	std::ostream& getOutputStream()
		{ return m_impl->getOutputStream(); }
	/**
	 * Get an iostream to read/write from/to the socket
	 * @return an iostream& which can be used for socket input and output.
	 * @throws SocketException
	 */
	std::iostream& getIOStream()
		{ return m_impl->getIOStream(); }
	/**
	 * @return The Select_t associated with this sockect.
	 */
	Select_t getSelectObj() const { return m_impl->getSelectObj(); }
	/**
	 * Get the socket handle for the socket
	 * @return the socket handle
	 */
	SocketHandle_t getfd() { return m_impl->getfd(); }

	/**
	 * Get connected state
	 */
	bool isConnected() const { return m_impl->isConnected(); }

	static void createShutDownMechanism() OW_DEPRECATED; // deprecated in 3.1.0
	/**
	 * Call this to shutdown all sockets.  This is usefull when the CIMOM
	 * is shutting down.  We want any outstanding connections to close
	 * immediately.
	 */
	static void shutdownAllSockets() OW_DEPRECATED; // deprecated in 3.1.0
	/**
	 * Have the sockets been shutdown?
	 * @return true if the sockets have been shutdown
	 */
	static bool gotShutDown() OW_DEPRECATED; // deprecated in 3.1.0
	static void deleteShutDownMechanism() OW_DEPRECATED; // deprecated in 3.1.0

#if defined(OW_WIN32)
	typedef HANDLE ShutDownMechanism_t;
#else
	typedef UnnamedPipeRef ShutDownMechanism_t;
#endif

	static ShutDownMechanism_t getShutDownMechanism() // deprecated in 3.1.0 - not marked as OW_DEPRECATED because other code still calls this.
	{
		return s_shutDownMechanism;
	}

private:
	SocketBaseImplRef m_impl;

	static ShutDownMechanism_t s_shutDownMechanism;

};

} // end namespace OpenWBEM

#endif
