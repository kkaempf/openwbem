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

//
// OW_XMLOperationGeneric.hpp
//
//
//

#ifndef __OW_XMLOPERATIONGENERIC_HPP__
#define __OW_XMLOPERATIONGENERIC_HPP__


#include "OW_config.h"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_Param.hpp"
#include "OW_CIMFwd.hpp"
#include "OW_XMLParameters.hpp"

class OW_CIMXMLParser;

class OW_XMLOperationGeneric : public OW_XMLParameters
{
	public:
		static const char* const CIMVERSION;
		static const char* const DTDVERSION;
		static const char* const MSG_ID;
		static const char* const PROTOCOLVERSION;
		static const char* const CIMVERSION_VALUE;
		static const char* const DTDVERSION_VALUE;
		static const char* const PROTOCOLVERSION_VALUE;

		static void extrinsicMethod(std::ostream& ostr,
				OW_CIMXMLParser& parser,
				OW_String& method,
				OW_CIMOMHandleIFC& hdl) /*throw (OW_CIMException, OW_CIMMalformedUrlException)*/;

		//static OW_XMLNode XMLGetCIMElement(OW_XMLNode& node) /*throw (OW_CIMException)*/;


	private:
		static const char* const paramErrorCode;
		static const char* const paramErrorDescription;

	protected:

//         static OW_XMLNode processReturnStream(  OW_XMLNode reply,
//                 OW_Bool extrinsic,
//                 const OW_String& operation,
//                 const OW_String& origMsgID) /*throw (OW_CIMException)*/;
};

#endif
