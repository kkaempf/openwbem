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
#ifndef OW_CIMPROTOCOL_HPP_INCLUDE_GUARD_
#define OW_CIMPROTOCOL_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Exception.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_Reference.hpp"
#include "OW_ClientAuthCBIFC.hpp"
#include "OW_SocketAddress.hpp"
#include "OW_CIMProtocolIStreamIFC.hpp"
#include <iosfwd>

namespace OpenWBEM
{

DECLARE_EXCEPTION(CIMProtocol);
class CIMProtocolIFC
{
public:
	virtual ~CIMProtocolIFC();
	virtual Reference<std::iostream> beginRequest(
			const String& methodName, const String& nameSpace) = 0;
	/**
	 * Establishes a connection (if not already connected) to the
	 * CIMOM and sends a request.  An istream& is returned containing
	 * the response from the CIMOM, after protocol processing is done.
	 * @param request An istream& containing the request to be send to
	 * 	the CIMOM.
	 * @param methodName The CIM method that corresponds to the request.
	 * @nameSpace the namespace the request applies to.
	 * @return an istream& containing the response from the server
	 * @exception HTTPException
	 * @exception SocketException
	 *
	 */
	virtual Reference<CIMProtocolIStreamIFC> endRequest(
		Reference<std::iostream> request,
			const String& methodName, const String& nameSpace) = 0;
	/**
	 * Get the supported features of a CIMOM
	 * @return a CIMFeatures object listing the features of the CIMOM.
	 */
	virtual CIMFeatures getFeatures() = 0;
	
	/**
	 * Assigns a login callback object.
	 * @param loginCB A Reference to a ClientAuthCB object
	 * 	containing the callback method.
	 */
	void setLoginCallBack(ClientAuthCBIFCRef loginCB)
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
protected:
	ClientAuthCBIFCRef m_loginCB;
	String m_contentType;
};
typedef Reference<CIMProtocolIFC> CIMProtocolIFCRef;

} // end namespace OpenWBEM

#endif
