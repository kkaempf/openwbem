/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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

#ifndef OW_CIMPROTOCOL_HPP_INCLUDE_GUARD_
#define OW_CIMPROTOCOL_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Exception.hpp"
#include "OW_CIMFeatures.hpp"
#include "blocxx/IntrusiveReference.hpp"
#include "blocxx/IntrusiveCountableBase.hpp"
#include "OW_ClientAuthCBIFC.hpp"
#include "blocxx/SocketAddress.hpp"
#include "blocxx/Reference.hpp"
#include "OW_IfcsFwd.hpp"
#include <iosfwd>

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

OW_DECLARE_APIEXCEPTION(CIMProtocol, OW_COMMON_API);
class OW_COMMON_API CIMProtocolIFC : virtual public blocxx::IntrusiveCountableBase
{
public:
	virtual ~CIMProtocolIFC();
	virtual blocxx::Reference<std::ostream> beginRequest(
			const blocxx::String& methodName, const blocxx::String& nameSpace) = 0;

	/**
	 * These request types correspond to the various types of operations
	 * identified in the CIM Operations Over HTTP 1.1 Sec. 3
	 */
	enum ERequestType
	{
		E_CIM_OPERATION_REQUEST,
		E_CIM_EXPORT_REQUEST,
		E_CIM_BATCH_OPERATION_REQUEST,
		E_CIM_BATCH_EXPORT_REQUEST,
		E_CIM_OPERATION_RESPONSE,
		E_CIM_EXPORT_RESPONSE
	};
	/**
	 * Establishes a connection (if not already connected) to the
	 * CIMOM and sends a request.  An istream& is returned containing
	 * the response from the CIMOM, after protocol processing is done.
	 * @param request An istream& containing the request to be send to
	 *	the CIMOM.
	 * @param methodName The CIM method that corresponds to the request.
	 * @cimObject the CIM object the request applies to.  If this is an
	 * 	intrinsic method, it must be a namespace.  If an extrinsic
	 *	method is being invoked, it must be a class or instance path in
	 *	ObjectPath format.
	 * @param requestType The type of request, currently must be one of
	 *	E_CIM_OPERATION_REQUEST, or E_CIM_EXPORT_REQUEST
	 * @param cimProtocolVersion The version of the CIM Protocol in use
	 * @return a CIMProtocolIstream (istream) reference containing the
	 *	response from the server.
	 * @exception HTTPException
	 * @exception SocketException
	 *
	 */
	virtual blocxx::Reference<std::istream> endRequest(
		const blocxx::Reference<std::ostream>& request,
		const blocxx::String& methodName,
		const blocxx::String& cimObject,
		ERequestType requestType,
		const blocxx::String& cimProtocolVersion) = 0;

	/**
	 * Called when the response stream (returned from endRequest) has been
	 * fully read.  endResponse() may do additional processing and/or error
	 * detection.  In the HTTPClient case it will read trailers, and prime
	 * the connection to begin a new response.
	 *
	 * @param istr The return value from endRequest()
	 */
	virtual void endResponse(std::istream & istr) = 0;

	/**
	 * Get the supported features of a CIMOM
	 * @return a CIMFeatures object listing the features of the CIMOM.
	 */
	virtual CIMFeatures getFeatures() = 0;

	/**
	 * Assigns a login callback object.
	 * @param loginCB A Reference to a ClientAuthCB object containing the
	 *	callback method.
	 */
	void setLoginCallBack(const ClientAuthCBIFCRef& loginCB)
		{ m_loginCB = loginCB; }
	void setContentType(const blocxx::String& ct)
		{ m_contentType = ct; }
	/**
	 * Gets the address of the local machine
	 * @return An SocketAddress corresponding to the local machine.
	 */
	virtual blocxx::SocketAddress getLocalAddress() const = 0;
	/**
	 * Gets the address of the peer connection
	 * @return An SocketAddress corresponding to the peer connection
	 */
	virtual blocxx::SocketAddress getPeerAddress()  const = 0;

	/**
	 * Close the connetion to the CIMOM. This will free resources used for the
	 * client session.
	 */
	virtual void close() = 0;

	static const int INFINITE_TIMEOUT OW_DEPRECATED = -1; // in 4.0.0

	/**
	 * Set the receive timeout on the socket
	 * @param timeout The timeout to use when waiting for data
	 */
	virtual void setReceiveTimeout(const blocxx::Timeout& timeout) = 0;
	void setReceiveTimeout(int seconds) OW_DEPRECATED; // in 4.0.0
	/**
	 * Get the receive timeout
	 * @return The receive timeout
	 */
	virtual blocxx::Timeout getReceiveTimeout() const = 0;
	/**
	 * Set the send timeout on the socket
	 * @param timeout The timeout to use when waiting to send data
	 */
	virtual void setSendTimeout(const blocxx::Timeout& timeout) = 0;
	void setSendTimeout(int seconds) OW_DEPRECATED; // in 4.0.0
	/**
	 * Get the send timeout
	 * @return The number of seconds of the send timeout
	 */
	virtual blocxx::Timeout getSendTimeout() const = 0;
	/**
	 * Set the connect timeout on the socket
	 * @param timeout The connect timeout
	 */
	virtual void setConnectTimeout(const blocxx::Timeout& timeout) = 0;
	void setConnectTimeout(int seconds) OW_DEPRECATED; // in 4.0.0
	/**
	 * Get the connect timeout
	 * @return The number of seconds of the connect timeout
	 */
	virtual blocxx::Timeout getConnectTimeout() const = 0;
	/**
	 * Set all timeouts (send, receive, connect)
	 * @param timeout The timeouts.
	 */
	virtual void setTimeouts(const blocxx::Timeout& timeout) = 0;
	void setTimeouts(int seconds) OW_DEPRECATED; // in 4.0.0

protected:

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	ClientAuthCBIFCRef m_loginCB;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

	blocxx::String m_contentType;
};

} // end namespace OW_NAMESPACE

#endif
