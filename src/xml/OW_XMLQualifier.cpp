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

// Static
OW_XMLNode
OW_XMLQualifier::processQualifierDecl(const OW_XMLNode& result,
	OW_CIMQualifierType& cimQualifier)
/*throw (OW_CIMException)*/
{
	OW_String superClassName;
	OW_String inClassName;
	OW_XMLNode next;

	OW_String qualName = result.getAttribute(paramName);
	if(qualName.length() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				"No qualifier name");
	}
	cimQualifier.setName(qualName);

	OW_String qualType = result.getAttribute(paramTypeAssign);
	if(qualType.length() == 0)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				"No type for qualifier");
	}
	OW_CIMDataType dt = OW_CIMDataType::getDataType(qualType);

	OW_String qualISARRAY = result.getAttribute(paramISARRAY);
	if(qualISARRAY.equalsIgnoreCase("true"))
	{
		OW_String qualArraySize = result.getAttribute(paramArraySize);
		if(qualArraySize.length() > 0)
		{
			dt.setToArrayType(qualArraySize.toInt32());
		}
		else
		{
			dt.setToArrayType(-1);  // unlimited array type
		}
	}
	cimQualifier.setDataType(dt);

	OW_String qualFlavor = result.getAttribute("OVERRIDABLE");
	if(qualFlavor.equalsIgnoreCase("false"))
	{
		cimQualifier.addFlavor(OW_CIMFlavor(OW_CIMFlavor::DISABLEOVERRIDE));
	}
	else
	{
		cimQualifier.addFlavor(OW_CIMFlavor(OW_CIMFlavor::ENABLEOVERRIDE));
	}

	qualFlavor = result.getAttribute("TOSUBCLASS");
	if(qualFlavor.equalsIgnoreCase("false"))
	{
		cimQualifier.addFlavor(OW_CIMFlavor(OW_CIMFlavor::RESTRICTED));
	}
	else
	{
		cimQualifier.addFlavor(OW_CIMFlavor(OW_CIMFlavor::TOSUBCLASS));
	}

	qualFlavor = result.getAttribute("TOINSTANCE");
	if(qualFlavor.equalsIgnoreCase("true"))
	{
		cimQualifier.addFlavor(OW_CIMFlavor(OW_CIMFlavor::TOINSTANCE));
	}

	qualFlavor = result.getAttribute("TRANSLATABLE");
	if(qualFlavor.equalsIgnoreCase("true"))
	{
		cimQualifier.addFlavor(OW_CIMFlavor(OW_CIMFlavor::TRANSLATE));
	}

	OW_XMLNode scope = result.getChild();
	if(!scope)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				"Could not find child element for scope");
	}

  	OW_XMLNode scopeNode = scope.findElement(OW_XMLNode::XML_ELEMENT_SCOPE);

	if(!scopeNode)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				"Could not find scope");
	}

	processScope(scope,cimQualifier,"CLASS",OW_CIMScope::CLASS);
	processScope(scope,cimQualifier,"INSTANCE",OW_CIMScope::INSTANCE);
	processScope(scope,cimQualifier,"ASSOCIATION",OW_CIMScope::ASSOCIATION);
	processScope(scope,cimQualifier,"REFERENCE",OW_CIMScope::REFERENCE);
	processScope(scope,cimQualifier,"PROPERTY",OW_CIMScope::PROPERTY);
	processScope(scope,cimQualifier,"METHOD",OW_CIMScope::METHOD);
	processScope(scope,cimQualifier,"PARAMETER",OW_CIMScope::PARAMETER);
	processScope(scope,cimQualifier,"INDICATION",OW_CIMScope::INDICATION);

	OW_XMLNode valueNode = scope.findElement(OW_XMLNode::XML_ELEMENT_VALUE);
	if(!scopeNode) // TODO is this right? test scopeNode?
	{
		valueNode = scope.findElement(OW_XMLNode::XML_ELEMENT_VALUE_ARRAY);
		if(!scopeNode) // TODO is this right? test scopeNode?
		{
			OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
					"Could not find value or array or values");
		}
	}
	return result.getNext();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLQualifier::processScope(const OW_XMLNode& result,
		OW_CIMQualifierType& cqt, const OW_String& attrName,
		int scopeValue)
		/*throw (OW_CIMException)*/
{
	OW_String scope = result.getAttribute(attrName);
	if(scope.length() == 0)
	{
		return;
	}
	if(scope.equalsIgnoreCase("true"))
	{
		//cqt.addScope(OW_CIMScope::getScope()); //TODO arg? // scopeValue));
		cqt.addScope(OW_CIMScope(scopeValue));
	}
	else if(!scope.equalsIgnoreCase("false"))
		OW_THROWCIM(OW_CIMException::FAILED);
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_XMLQualifier::getQualifierName(const OW_XMLNode& node_arg)
	/*throw (OW_CIMException)*/
{
	OW_XMLNode node = node_arg; // TODO is this better? not changing param?
	OW_String qualifierName;
	if(node.getToken() != OW_XMLNode::XML_ELEMENT_IPARAMVALUE)
	{
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
			"Expected IPARAMVALUE to lead into QualifierName");
	}
	OW_String propertyName = node.getAttribute(paramName);
	if(propertyName.length() == 0)
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				"Cannot find property Name");

	if(!propertyName.equalsIgnoreCase(XMLP_QUALIFIERNAME))
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				"Cannot find qualifier name");

	node = node.getChild();
	node = node.findElement(OW_XMLNode::XML_ELEMENT_VALUE);
	if(!node)
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
				"Cannot find value for property name");
	return node.getText();
}

const char* const OW_XMLQualifier::XMLP_QUALIFIERNAME = "QualifierName";
const char* const OW_XMLQualifier::XMLP_QUALIFIERDECL = "QualifierDeclaration";
const char* const OW_XMLQualifier::paramISARRAY="ISARRAY";
const char* const OW_XMLQualifier::paramQualifierFlavor="QualifierFlavor";
const char* const OW_XMLQualifier::paramArraySize="ArraySize"; 
