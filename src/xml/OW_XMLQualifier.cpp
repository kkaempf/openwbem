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

/**
 *
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

// Static
void
OW_XMLQualifier::processQualifierDecl(OW_CIMXMLParser& parser,
	OW_CIMQualifierType& cimQualifier)
{
	if (!parser.tokenIs(OW_CIMXMLParser::E_QUALIFIER_DECLARATION))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Expected but did not find <QUALIFIER.DECLARATION>");
	}

	OW_String superClassName;
	OW_String inClassName;

	OW_String qualName = parser.mustGetAttribute(paramName);
	cimQualifier.setName(qualName);

	OW_String qualType = parser.mustGetAttribute(paramTypeAssign);
	OW_CIMDataType dt = OW_CIMDataType::getDataType(qualType);

	OW_String qualISARRAY = parser.getAttribute(paramISARRAY);
	if(qualISARRAY.equalsIgnoreCase("true"))
	{
		OW_String qualArraySize = parser.getAttribute(paramArraySize);
		if(!qualArraySize.empty())
		{
			try
			{
				dt.setToArrayType(qualArraySize.toInt32());
			}
			catch (const OW_StringConversionException&)
			{
				OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
					"Invalid array size");
			}
		}
		else
		{
			dt.setToArrayType(-1);  // unlimited array type
		}
	}
	cimQualifier.setDataType(dt);

	OW_String qualFlavor = parser.getAttribute("OVERRIDABLE");
	if(qualFlavor.equalsIgnoreCase("false"))
	{
		cimQualifier.addFlavor(OW_CIMFlavor(OW_CIMFlavor::DISABLEOVERRIDE));
	}
	else
	{
		cimQualifier.addFlavor(OW_CIMFlavor(OW_CIMFlavor::ENABLEOVERRIDE));
	}

	qualFlavor = parser.getAttribute("TOSUBCLASS");
	if(qualFlavor.equalsIgnoreCase("false"))
	{
		cimQualifier.addFlavor(OW_CIMFlavor(OW_CIMFlavor::RESTRICTED));
	}
	else
	{
		cimQualifier.addFlavor(OW_CIMFlavor(OW_CIMFlavor::TOSUBCLASS));
	}

	qualFlavor = parser.getAttribute("TOINSTANCE");
	if(qualFlavor.equalsIgnoreCase("true"))
	{
		cimQualifier.addFlavor(OW_CIMFlavor(OW_CIMFlavor::TOINSTANCE));
	}

	qualFlavor = parser.getAttribute("TRANSLATABLE");
	if(qualFlavor.equalsIgnoreCase("true"))
	{
		cimQualifier.addFlavor(OW_CIMFlavor(OW_CIMFlavor::TRANSLATE));
	}

	parser.getNextTag();
	if(parser.tokenIs(OW_CIMXMLParser::E_SCOPE))
	{
		// process optional scope child
		processScope(parser,cimQualifier,"CLASS",OW_CIMScope::CLASS);
		processScope(parser,cimQualifier,"ASSOCIATION",OW_CIMScope::ASSOCIATION);
		processScope(parser,cimQualifier,"REFERENCE",OW_CIMScope::REFERENCE);
		processScope(parser,cimQualifier,"PROPERTY",OW_CIMScope::PROPERTY);
		processScope(parser,cimQualifier,"METHOD",OW_CIMScope::METHOD);
		processScope(parser,cimQualifier,"PARAMETER",OW_CIMScope::PARAMETER);
		processScope(parser,cimQualifier,"INDICATION",OW_CIMScope::INDICATION);
		if (cimQualifier.hasScope(OW_CIMScope::CLASS) &&
			cimQualifier.hasScope(OW_CIMScope::ASSOCIATION) &&
			cimQualifier.hasScope(OW_CIMScope::REFERENCE) &&
			cimQualifier.hasScope(OW_CIMScope::PROPERTY) &&
			cimQualifier.hasScope(OW_CIMScope::METHOD) &&
			cimQualifier.hasScope(OW_CIMScope::PARAMETER) &&
			cimQualifier.hasScope(OW_CIMScope::INDICATION))
		{
			cimQualifier.addScope(OW_CIMScope::ANY); // This will erase all the others.
		}
		parser.mustGetNextTag();
		parser.mustGetEndTag();
	}

	if (parser.tokenIs(OW_CIMXMLParser::E_VALUE) ||
		parser.tokenIs(OW_CIMXMLParser::E_VALUE_ARRAY))
	{
		// process optional value or value.array child
		OW_CIMValue val = OW_XMLCIMFactory::createValue(parser,qualType);
		cimQualifier.setDefaultValue(val);
	}
	parser.mustGetEndTag(); // pass </QUALIFIER.DECLARATION>
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLQualifier::processScope(OW_CIMXMLParser& parser,
		OW_CIMQualifierType& cqt, const char* attrName,
		OW_CIMScope::Scope scopeValue)
{
	OW_String scope = parser.getAttribute(attrName);
	if(scope.empty())
	{
		return;
	}
	if(scope.equalsIgnoreCase("true"))
	{
		cqt.addScope(OW_CIMScope(scopeValue));
	}
	else if(!scope.equalsIgnoreCase("false"))
		OW_THROWCIM(OW_CIMException::FAILED);
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_XMLQualifier::getQualifierName(OW_CIMXMLParser& parser)
{
	if(!parser.tokenIs(OW_CIMXMLParser::E_IPARAMVALUE))
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"Expected IPARAMVALUE to lead into QualifierName");
	}
	OW_String propertyName = parser.mustGetAttribute(paramName);

	if(!propertyName.equalsIgnoreCase(XMLP_QUALIFIERNAME))
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				"Cannot find qualifier name");

	parser.getChild();
	if (!parser.tokenIs(OW_CIMXMLParser::E_VALUE))
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				"Cannot find value for qualifier name");
	parser.mustGetNext();
	if (!parser.isData())
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				"Cannot find value for qualifier name");
	}
	OW_String name = parser.getData();
	parser.mustGetNextTag();
	parser.mustGetEndTag();
	return name;
}

const char* const OW_XMLQualifier::XMLP_QUALIFIERNAME = "QualifierName";
const char* const OW_XMLQualifier::XMLP_QUALIFIERDECL = "QualifierDeclaration";
const char* const OW_XMLQualifier::paramISARRAY="ISARRAY";
const char* const OW_XMLQualifier::paramQualifierFlavor="QualifierFlavor";
const char* const OW_XMLQualifier::paramArraySize="ArraySize";
