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
// OW_XMLOperationGeneric.cpp
//

#include "OW_config.h"
#include "OW_Param.hpp"
#include "OW_XMLOperationGeneric.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMErrorException.hpp"

using std::istream;
using std::ostream;

void
OW_XMLOperationGeneric::extrinsicMethod(ostream& ostr, OW_XMLNode& node,
	OW_String& method, OW_CIMOMHandleIFC& hdl)
{
	(void)ostr;
	(void)node;
	(void)method;
	(void)hdl;
	OW_ASSERT(0);
}

// OW_XMLNode
// OW_XMLOperationGeneric::XMLGetCIMElement(OW_XMLNode& node)
// {
//     if(node.getToken() != OW_XMLNode::XML_ELEMENT_CIM)
//     {
//         OW_THROWCIMMSG(OW_CIMException::FAILED, "Could not find <CIM> element");
//     }
//
//     OW_String str = node.mustGetAttribute(CIMVERSION);
//
//     if(!str.equals(CIMVERSION_VALUE))
//     {
//         OW_THROW(OW_CIMErrorException,
//             OW_CIMErrorException::unsupported_cim_version);
//     }
//
//     str = node.mustGetAttribute(DTDVERSION);
//     if(!str.equals(DTDVERSION_VALUE))
//     {
//         OW_THROW(OW_CIMErrorException,
//             OW_CIMErrorException::unsupported_dtd_version);
//     }
//
//     node = node.getChild();
//     if(!node)
//     {
//         OW_THROWCIMMSG(OW_CIMException::FAILED, "No child for <CIM> element");
//     }
//
//     return(node);
// }
//
//
const char* const OW_XMLOperationGeneric::CIMVERSION = "CIMVERSION";
const char* const OW_XMLOperationGeneric::DTDVERSION = "DTDVERSION";
const char* const OW_XMLOperationGeneric::MSG_ID = "ID";
const char* const OW_XMLOperationGeneric::PROTOCOLVERSION = "PROTOCOLVERSION";
const char* const OW_XMLOperationGeneric::CIMVERSION_VALUE = "2.0";
const char* const OW_XMLOperationGeneric::DTDVERSION_VALUE = "2.0";
const char* const OW_XMLOperationGeneric::PROTOCOLVERSION_VALUE = "1.0";
const char* const OW_XMLOperationGeneric::paramErrorCode = "CODE";
const char* const OW_XMLOperationGeneric::paramErrorDescription = "DESCRIPTION";


