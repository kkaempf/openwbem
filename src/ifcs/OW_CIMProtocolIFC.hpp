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

#ifndef OW_CIMPROTOCOL_HPP_INCLUDE_GUARD_
#define OW_CIMPROTOCOL_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Exception.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_IntrusiveReference.hpp"
#include "OW_IntrusiveCountableBase.hpp"
#include "OW_ClientAuthCBIFC.hpp"
#include "OW_SocketAddress.hpp"
#include "OW_CIMProtocolIStreamIFC.hpp"
#include "OW_Reference.hpp"
#include "OW_IfcsFwd.hpp"
#include <iosfwd>

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OpenWBEM
{

OW_DECLARE_APIEXCEPTION(CIMProtocol, OW_COMMON_API);
class OW_COMMON_API CIMProtocolIFC : public IntrusiveCountableBase
{
public:
	virtual ~CIMProtocolIFC();
	virtual Reference<std::iostream> beginRequest(
			const String& methodName, const String& nameSpace) = 0;

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
	 * @return a CIMProtocolIstream (istream) reference containing the
	 *	response from the server.
	 * @exception HTTPException
	 * @exception SocketException
	 *
	 */
	virtual CIMProtocolIStreamIFCRef endRequest(
			const Reference<std::iostream>& request,
			const String& methodName, const String& cimObject, ERequestType requestType) = 0;
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
	void setContentType(const String& ct)
		{ m_contentType = ct; }
	/**
	 * Gets the address of the local machine
	 * @return An SocketAddress corresponding to the local machine.
	 */
	virtual SocketAddress getLocalAddress() const = 0;
	/**
	 * Gets the address of the peer connection
	 * @return An SocketAddress corresponding to the peer connection
	 */
	virtual SocketAddress getPeerAddress()  const = 0;

	/**
	 * Close the connetion to the CIMOM. This will free resources used for the
	 * client session.
	 */
	virtual void close() = 0;

protected:

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	ClientAuthCBIFCRef m_loginCB;

#ifdef OW_WIN32
#pragma warning (pop)
#endif

	String m_contentType;
};

} // end namespace OpenWBEM

#endif
