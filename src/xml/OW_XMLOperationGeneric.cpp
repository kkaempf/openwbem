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

#include "OW_config.h"
#include "OW_Param.hpp"
#include "OW_XMLOperationGeneric.hpp"
#include "OW_Assertion.hpp"
#include "OW_CIMErrorException.hpp"
#include "OW_CIMXMLParser.hpp"

using std::istream;
using std::ostream;

void
OW_XMLOperationGeneric::XMLGetCIMElement(OW_CIMXMLParser& parser)
{
    if(!parser.tokenIs(OW_CIMXMLParser::E_CIM))
    {
		OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_loosely_valid);
    }

    OW_String str = parser.mustGetAttribute(OW_CIMXMLParser::A_CIMVERSION);

    if(!str.equals(OW_CIMXMLParser::AV_CIMVERSION_VALUE))
    {
        OW_THROW(OW_CIMErrorException,
            OW_CIMErrorException::unsupported_cim_version);
    }

    str = parser.mustGetAttribute(OW_CIMXMLParser::A_DTDVERSION);
    if(!str.equals(OW_CIMXMLParser::AV_DTDVERSION_VALUE))
    {
        OW_THROW(OW_CIMErrorException,
            OW_CIMErrorException::unsupported_dtd_version);
    }

    parser.getChild();
    if(!parser)
    {
		OW_THROW(OW_CIMErrorException, OW_CIMErrorException::request_not_loosely_valid);
    }

    return;
}




