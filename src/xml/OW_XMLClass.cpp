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
#include "OW_XMLCIMFactory.hpp"
#include "OW_XMLClass.hpp"
#include "OW_XMLAttribute.hpp"
#include "OW_String.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMUrl.hpp"

//////////////////////////////////////////////////////////////////////////////		
OW_String
OW_XMLClass::getNameSpace(const OW_XMLNode& localNameNodeArg)
{
	OW_String nameSpace;
	OW_Bool firstTime = true;
	OW_XMLNode localNameNode = localNameNodeArg;
	for(; localNameNode; localNameNode = localNameNode.getNext())
	{
		if (localNameNode.getToken() == OW_XMLNode::XML_ELEMENT_NAMESPACE)
		{
			OW_String pname = localNameNode.getAttribute(OW_XMLAttribute::NAME);
			if(pname.length())
			{
				if (firstTime)
				{
					firstTime=false;
					nameSpace += pname;
				}
				else
				{
					nameSpace += "/" + pname;
				}
			}
		}
	}
	return(nameSpace);
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMObjectPath
OW_XMLClass::getObjectWithPath(OW_XMLNode& node, OW_CIMClassArray& cArray,
	OW_CIMInstanceArray& iArray)
{
	int token=node.getToken();

	OW_XMLNode nextNode = node.getChild();
	
	if(!nextNode)
		OW_THROWCIMMSG(OW_CIMException::FAILED, "Missing element in object with path declaration");
	
	if (token == OW_XMLNode::XML_ELEMENT_VALUE_OBJECTWITHPATH)
	{
		OW_CIMObjectPath tmpcop = OW_XMLCIMFactory::createObjectPath(nextNode);
			
		token = nextNode.getToken();
		
		if (token == OW_XMLNode::XML_ELEMENT_CLASSPATH)
		{
		    nextNode = nextNode.mustNextElement(OW_XMLNode::XML_ELEMENT_CLASS);
		    cArray.append(readClass(nextNode,tmpcop));
		}
		else if (token==OW_XMLNode::XML_ELEMENT_INSTANCEPATH)
		{
		    nextNode=nextNode.mustNextElement(OW_XMLNode::XML_ELEMENT_INSTANCE);
		    iArray.append(readInstance(nextNode,tmpcop));
		}
		else
		{
		    OW_THROWCIMMSG(OW_CIMException::FAILED, "Require instance or class in object with path declaration");
		}
		
		return(tmpcop);
	}
	else if (token==OW_XMLNode::XML_ELEMENT_VALUE_OBJECTWITHLOCALPATH)
	{
	    OW_CIMObjectPath tmpcop = OW_XMLCIMFactory::createObjectPath(nextNode);
			
	    token = nextNode.getToken();
	    if (token == OW_XMLNode::XML_ELEMENT_LOCALCLASSPATH)
		{
			nextNode = nextNode.mustNextElement(OW_XMLNode::XML_ELEMENT_CLASS);
		    cArray.append(readClass(nextNode,tmpcop));
	    }
		else if (token == OW_XMLNode::XML_ELEMENT_LOCALINSTANCEPATH)
		{
			nextNode = nextNode.mustNextElement(OW_XMLNode::XML_ELEMENT_INSTANCE);
		    iArray.append(readInstance(nextNode,tmpcop));
	    }
		else
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				"Require instance or class in object with path declaration");
		}
	    return(tmpcop);
	}
	return OW_CIMObjectPath();
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMClass
OW_XMLClass::readClass(OW_XMLNode& childNode, OW_CIMObjectPath& path)
{
	if(childNode.getToken() == OW_XMLNode::XML_ELEMENT_CLASS)
	{
		OW_CIMClass cimClass = OW_XMLCIMFactory::createClass(childNode);
	
		path.setObjectName(cimClass.getName());
	
		return(cimClass);
	}
	OW_THROWCIMMSG(OW_CIMException::FAILED, "Needed <CLASS>, found "+childNode.getToken());
}

//////////////////////////////////////////////////////////////////////////////		
OW_CIMInstance
OW_XMLClass::readInstance(OW_XMLNode& childNode, OW_CIMObjectPath& path)
{
	(void)path;
	if (childNode.getToken() == OW_XMLNode::XML_ELEMENT_INSTANCE)
	{
		OW_CIMInstance cimInstance = OW_XMLCIMFactory::createInstance(childNode);
	    return(cimInstance);
	}
	OW_THROWCIMMSG(OW_CIMException::FAILED, "Needed <INSTANCE>, found "
		+ childNode.getToken());
}

//////////////////////////////////////////////////////////////////////////////		
void
OW_XMLClass::getInstanceName(OW_XMLNode& result,
								OW_CIMObjectPath& cimPath)
{
	OW_CIMPropertyArray propertyArray;
	OW_CIMProperty cp;
	result = result.mustFindElement(OW_XMLNode::XML_ELEMENT_INSTANCENAME);

	OW_String thisClassName = result.getAttribute(OW_XMLAttribute::CLASS_NAME);
	cimPath.setObjectName(thisClassName);

	OW_XMLNode sub = result.getChild();
	int token = sub.getToken();

	if (token == OW_XMLNode::XML_ELEMENT_KEYBINDING)
	{
		for(; sub; sub = sub.getNext())
		{
			OW_CIMValue value;
			OW_String name;
			OW_XMLNode keyval;
		
			sub.mustElement(OW_XMLNode::XML_ELEMENT_KEYBINDING);
			name = sub.getAttribute(OW_XMLAttribute::NAME);

			keyval = sub.mustGetChild();
			token = keyval.getToken();

			switch(token)
			{
				case OW_XMLNode::XML_ELEMENT_KEYVALUE:
					value = OW_CIMValue(keyval.getText());
					break;
				case OW_XMLNode::XML_ELEMENT_VALUE_REFERENCE:
					value = OW_CIMValue(OW_XMLCIMFactory::createObjectPath(keyval.getChild()));
					break;
				default:
					OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
						"Not a valid instance declaration");
			}

			cp = OW_CIMProperty(name, value);
			propertyArray.push_back(cp);
		}
	}
	else if (token == OW_XMLNode::XML_ELEMENT_KEYVALUE)
	{
		//-------------------------------------
		// HOW DO WE GET THE PROPERTY NAME?
		//-------------------------------------
		OW_CIMValue value(sub.getText());
		cp = OW_CIMProperty(OW_Bool(true));
		cp.setDataType(OW_CIMDataType(OW_CIMDataType::STRING));
		cp.setValue(value);
		propertyArray.push_back(cp);
	}
	else if (token == OW_XMLNode::XML_ELEMENT_VALUE_REFERENCE)
	{
		//-------------------------------------
		// HOW DO WE GET THE PROPERTY NAME?
		//-------------------------------------
		OW_CIMObjectPath cop = OW_XMLCIMFactory::createObjectPath(sub);
		OW_CIMValue value(cop);
		cp = OW_CIMProperty(OW_Bool(true));
		cp.setDataType(OW_CIMDataType(OW_CIMDataType::REFERENCE));
		cp.setValue(value);
		propertyArray.push_back(cp);
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
			"not a valid instance declaration");
	}

	cimPath.setKeys(propertyArray);
}



