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
#ifndef OW_XMLQWALIFIER_HPP_INCLUDE_GUARD_
#define OW_XMLQWALIFIER_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_XMLClass.hpp"
#include "OW_CIMScope.hpp"

class OW_CIMXMLParser;

class OW_XMLQualifier : public OW_XMLClass
{
public:
	static const char* const XMLP_QUALIFIERNAME;
	static const char* const XMLP_QUALIFIERDECL;
	static const char* const paramISARRAY;
	static const char* const paramQualifierFlavor;
	static const char* const paramArraySize;

	static void processQualifierDecl(OW_CIMXMLParser& result,
		OW_CIMQualifierType& cimQualifier);

protected:

	/**
	 * @throws OW_CIMException
	 */
	static OW_String getQualifierName(OW_CIMXMLParser& node);



private:

	/**
	 * @throws OW_CIMException
	 */
	static void processScope(OW_CIMXMLParser& parser,
		OW_CIMQualifierType& cqt, const char* attrName,
		OW_CIMScope::Scope scopeValue);

};

#endif

