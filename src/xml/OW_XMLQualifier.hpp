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

#ifndef OW_XMLQWALIFIER_HPP_INCLUDE_GUARD_
#define OW_XMLQWALIFIER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_XMLClass.hpp"
#include "OW_CIMScope.hpp"
#include "OW_XMLFwd.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

// TODO: Get rid of this file.  Move the function into XMLCIMFactory
namespace OpenWBEM
{

namespace XMLQualifier
{
	OW_XML_API void processQualifierDecl(CIMXMLParser& result,
		CIMQualifierType& cimQualifier);

	/**
	 * @throws CIMException
	 */
	OW_XML_API String getQualifierName(CIMXMLParser& node);

	/**
	 * @throws CIMException
	 */
	OW_XML_API void processScope(CIMXMLParser& parser,
		CIMQualifierType& cqt, const char* attrName,
		CIMScope::Scope scopeValue);
} // end namespace XMLQualifiers

} // end namespace OpenWBEM

#endif
