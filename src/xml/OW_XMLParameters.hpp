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
//
// OW_XMLParameters.hpp
//
//
//
#ifndef OW_XMLPARAMETERS_HPP_INCLUDE_GUARD_
#define OW_XMLPARAMETERS_HPP_INCLUDE_GUARD_
#include	"OW_config.h"

namespace OpenWBEM
{

class XMLParameters
{
public:
	static const char* const paramName;
	static const char* const paramSuperName;
	static const char* const paramClassOrigin;
	static const char* const paramErrorCode;
	static const char* const paramErrorDescription;
	static const char* const paramPropagated 	;
	static const char* const paramArraySize     ;
	static const char* const paramRefClass      ;
	static const char* const paramTypeAssign 	;
	static const char* const paramOverridable   ;
	static const char* const paramToSubClass 	;
	static const char* const paramTranslatable  ;
	static const char* const paramReferenceClass;
	static const char* const classParamName     ;
	static const char* const XMLP_CLASSNAME     ;
	static const char* const XMLP_PROPERTYLIST  ;
	static const char* const XMLP_DEEP       	;
	static const char* const XMLP_LOCAL         ;
	static const char* const XMLP_INCLUDEQUALIFIERS       	;
	static const char* const XMLP_INCLUDECLASSORIGIN     	;
	static const char* const XMLP_MODIFIED_CLASS;
	static const char* const XMLP_MODIFIED_INSTANCE;
	static const char* const XMLP_INSTANCENAME;
	static const char* const XMLP_PROPERTYNAME;
	static const char* const XMLP_NEWVALUE		;
	static const char* const XMLP_ROLE			;
	static const char* const XMLP_RESULTROLE	;
	static const char* const XMLP_OBJECTNAME	;
	static const char* const XMLP_ASSOCCLASS	;
	static const char* const XMLP_RESULTCLASS	;
	static const char* const XMLP_QUERYLANGUAGE;
	static const char* const XMLP_QUERY;
};

} // end namespace OpenWBEM

typedef OpenWBEM::XMLParameters OW_XMLParameters;

#endif
