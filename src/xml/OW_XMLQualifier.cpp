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

#include "OW_config.h"
#include "OW_XMLQualifier.hpp"
#include "OW_CIMFlavor.hpp"
#include "OW_CIMScope.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMValue.hpp"
#include "OW_XMLCIMFactory.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMXMLParser.hpp"
#include "OW_CIMName.hpp"

namespace OW_NAMESPACE
{

namespace XMLQualifier
{

void
processQualifierDecl(CIMXMLParser& parser,
	CIMQualifierType& cimQualifier)
{
	if (!parser.tokenIsId(CIMXMLParser::E_QUALIFIER_DECLARATION))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER, "Expected but did not find <QUALIFIER.DECLARATION>");
	}
	CIMName superClassName;
	CIMName inClassName;
	CIMName qualName = parser.mustGetAttribute(CIMXMLParser::A_NAME);
	cimQualifier.setName(qualName);
	String qualType = parser.mustGetAttribute(CIMXMLParser::A_TYPE);
	CIMDataType dt = CIMDataType::getDataType(qualType);
	String qualISARRAY = parser.getAttribute(CIMXMLParser::A_ISARRAY);
	if (qualISARRAY.equalsIgnoreCase("true"))
	{
		String qualArraySize = parser.getAttribute(CIMXMLParser::A_ARRAYSIZE);
		if (!qualArraySize.empty())
		{
			try
			{
				dt.setToArrayType(qualArraySize.toInt32());
			}
			catch (const StringConversionException&)
			{
				OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
					"Invalid array size");
			}
		}
		else
		{
			dt.setToArrayType(-1);  // unlimited array type
		}
	}
	cimQualifier.setDataType(dt);
	String qualFlavor = parser.getAttribute("OVERRIDABLE");
	if (qualFlavor.equalsIgnoreCase("false"))
	{
		cimQualifier.addFlavor(CIMFlavor(CIMFlavor::DISABLEOVERRIDE));
	}
	else
	{
		cimQualifier.addFlavor(CIMFlavor(CIMFlavor::ENABLEOVERRIDE));
	}
	qualFlavor = parser.getAttribute("TOSUBCLASS");
	if (qualFlavor.equalsIgnoreCase("false"))
	{
		cimQualifier.addFlavor(CIMFlavor(CIMFlavor::RESTRICTED));
	}
	else
	{
		cimQualifier.addFlavor(CIMFlavor(CIMFlavor::TOSUBCLASS));
	}
	//qualFlavor = parser.getAttribute("TOINSTANCE");
	//if (qualFlavor.equalsIgnoreCase("true"))
	//{
	//	cimQualifier.addFlavor(CIMFlavor(CIMFlavor::TOINSTANCE));
	//}
	qualFlavor = parser.getAttribute("TRANSLATABLE");
	if (qualFlavor.equalsIgnoreCase("true"))
	{
		cimQualifier.addFlavor(CIMFlavor(CIMFlavor::TRANSLATE));
	}
	parser.getNextTag();
	if (parser.tokenIsId(CIMXMLParser::E_SCOPE))
	{
		// process optional scope child
		processScope(parser,cimQualifier,"CLASS",CIMScope::CLASS);
		processScope(parser,cimQualifier,"ASSOCIATION",CIMScope::ASSOCIATION);
		processScope(parser,cimQualifier,"REFERENCE",CIMScope::REFERENCE);
		processScope(parser,cimQualifier,"PROPERTY",CIMScope::PROPERTY);
		processScope(parser,cimQualifier,"METHOD",CIMScope::METHOD);
		processScope(parser,cimQualifier,"PARAMETER",CIMScope::PARAMETER);
		processScope(parser,cimQualifier,"INDICATION",CIMScope::INDICATION);
		if (cimQualifier.hasScope(CIMScope::CLASS) &&
			cimQualifier.hasScope(CIMScope::ASSOCIATION) &&
			cimQualifier.hasScope(CIMScope::REFERENCE) &&
			cimQualifier.hasScope(CIMScope::PROPERTY) &&
			cimQualifier.hasScope(CIMScope::METHOD) &&
			cimQualifier.hasScope(CIMScope::PARAMETER) &&
			cimQualifier.hasScope(CIMScope::INDICATION))
		{
			cimQualifier.addScope(CIMScope::ANY); // This will erase all the others.
		}
		parser.mustGetNextTag();
		parser.mustGetEndTag();
	}
	else
	{
		// The SCOPE subelement ... if absent it implies that there is no
		// restriction on the scope at which the Qualifier may be applied
		// (so that it has 'any' scop in the terminology of CIM)
		cimQualifier.addScope(CIMScope::ANY);
	}
	if (parser.tokenIsId(CIMXMLParser::E_VALUE) ||
		parser.tokenIsId(CIMXMLParser::E_VALUE_ARRAY))
	{
		// process optional value or value.array child
		CIMValue val = XMLCIMFactory::createValue(parser,qualType, XMLCIMFactory::E_VALUE_NOT_EMBEDDED_OBJECT);
		cimQualifier.setDefaultValue(val);
	}
	parser.mustGetEndTag(); // pass </QUALIFIER.DECLARATION>
}
//////////////////////////////////////////////////////////////////////////////
void
processScope(CIMXMLParser& parser,
		CIMQualifierType& cqt, const char* attrName,
		CIMScope::Scope scopeValue)
{
	String scope = parser.getAttribute(attrName);
	if (scope.empty())
	{
		return;
	}

	if (scope.equalsIgnoreCase("true"))
	{
		cqt.addScope(CIMScope(scopeValue));
	}
	else if (!scope.equalsIgnoreCase("false"))
	{
		OW_THROWCIM(CIMException::FAILED);
	}
}
//////////////////////////////////////////////////////////////////////////////
String
getQualifierName(CIMXMLParser& parser)
{
	if (!parser.tokenIsId(CIMXMLParser::E_IPARAMVALUE))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
			"Expected IPARAMVALUE to lead into QualifierName");
	}
	String propertyName = parser.mustGetAttribute(CIMXMLParser::A_NAME);
	if (!propertyName.equalsIgnoreCase(CIMXMLParser::P_QualifierName))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
				"Cannot find qualifier name");
	}
	parser.getChild();
	if (!parser.tokenIsId(CIMXMLParser::E_VALUE))
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
				"Cannot find value for qualifier name");
	}
	parser.mustGetNext();
	if (!parser.isData())
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
				"Cannot find value for qualifier name");
	}
	String name = parser.getData();
	parser.mustGetNextTag();
	parser.mustGetEndTag();
	return name;
}

} // end namespace XMLQualifier
} // end namespace OW_NAMESPACE

