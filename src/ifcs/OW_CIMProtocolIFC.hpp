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

#ifndef _OW_CIMPROTOCOL_HPP__
#define _OW_CIMPROTOCOL_HPP__

#include "OW_config.h"
#include "OW_Exception.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_Reference.hpp"
#include "OW_ClientAuthCBIFC.hpp"
#include <iosfwd>

DEFINE_EXCEPTION(CIMProtocol);


class OW_CIMProtocolIFC
{
public:
	virtual ~OW_CIMProtocolIFC() {}

	virtual OW_Reference<std::iostream> beginRequest(
			const OW_String& methodName, const OW_String& nameSpace) = 0;
	/**
	 * Establishes a connection (if not already connected) to the
	 * CIMOM and sends a request.  An istream& is returned containing
	 * the response from the CIMOM, after protocol processing is done.
	 * @param request An istream& containing the request to be send to
	 * 	the CIMOM.
	 * @param methodName The CIM method that corresponds to the request.
	 * @nameSpace the namespace the request applies to.
	 * @return an istream& containing the response from the server
	 * @exception OW_HTTPException
	 * @exception OW_SocketException
	 *
	 */
	virtual std::istream& endRequest(OW_Reference<std::iostream> request,
			const OW_String& methodName, const OW_String& nameSpace) = 0;


	//virtual OW_Reference<std::iostream> getStream() = 0;

	/**
	 * Get the supported features of a CIMOM
	 * @return a OW_CIMFeatures object listing the features of the CIMOM.
	 */
	virtual OW_CIMFeatures getFeatures() = 0;
	
	/**
	 * Assigns a login callback object.
	 * @param loginCB A OW_Reference to a OW_ClientAuthCB object
	 * 	containing the callback method.
	 */
	void setLoginCallBack(OW_ClientAuthCBIFCRef loginCB)
		{ m_loginCB = loginCB; }

protected:
	OW_ClientAuthCBIFCRef m_loginCB;

};

typedef OW_Reference<OW_CIMProtocolIFC> OW_CIMProtocolIFCRef;

#endif // _OW_CIMPROTOCOL_HPP__
