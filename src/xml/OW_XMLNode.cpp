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
// OW_XMLNode.cpp
//

#include	"OW_config.h"
#include	"OW_Mutex.hpp"
#include	"OW_MutexLock.hpp"
#include	"OW_XMLParser.hpp"
#include "OW_XMLParameters.hpp"
#include	"OW_XMLNode.hpp"
#include "OW_XMLOperationGeneric.hpp"
#include "OW_Format.hpp"

static XMLElementMap g_elementMap;
static OW_Bool g_initElementMap(true);
static OW_Mutex   g_ElementMapMutex;

static void initElementMap();

//////////////////////////////////////////////////////////////////////////////
OW_XMLNodeImpl::OW_XMLNodeImpl(const OW_String& name,
	const OW_XMLAttrArray& attrArray) :
	m_nextNode(NULL), m_childNode(NULL), m_lastChildNode(NULL), m_iToken(0),
	m_iType(0), m_XMLAttrArray(attrArray), m_strName(name), m_strText()
{
	initElementMap();
	m_iToken = g_elementMap[m_strName];
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNodeImpl::OW_XMLNodeImpl(const OW_String& name) :
	m_nextNode(NULL), m_childNode(NULL), m_lastChildNode(NULL), m_iToken(0),
	m_iType(0), m_XMLAttrArray(), m_strName(name), m_strText()
{
	initElementMap();
	m_iToken = g_elementMap[m_strName];
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNodeImpl::OW_XMLNodeImpl() :
	m_nextNode(NULL), m_childNode(NULL), m_lastChildNode(NULL), m_iToken(0),
	m_iType(0), m_XMLAttrArray(), m_strName(), m_strText()
{
	initElementMap();
	m_iToken = g_elementMap[m_strName];
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNodeImpl::OW_XMLNodeImpl(const OW_XMLNodeImpl& arg) :
	m_nextNode(arg.m_nextNode), m_childNode(arg.m_childNode),
	m_lastChildNode(arg.m_lastChildNode), m_iToken(arg.m_iToken),
	m_iType(arg.m_iType), m_XMLAttrArray(arg.m_XMLAttrArray),
	m_strName(arg.m_strName), m_strText(arg.m_strText)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNodeImpl::~OW_XMLNodeImpl()
{
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNodeImpl&
OW_XMLNodeImpl::operator= (const OW_XMLNodeImpl & arg)
{

	m_nextNode = arg.m_nextNode;
	m_childNode = arg.m_childNode;
	m_lastChildNode = arg.m_lastChildNode;
	m_iToken = arg.m_iToken;
	m_iType = arg.m_iType;
	m_XMLAttrArray = arg.m_XMLAttrArray;
	m_strName = arg.m_strName;
	m_strText = arg.m_strText;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_XMLNodeImpl::getNodeType()
{
	return m_iType;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLNodeImpl::setNodeType(int nodeType)
{
	m_iType = nodeType;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_XMLNodeImpl::getNodeName()
{
	return m_strName;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLNodeImpl::assignText(const OW_String& text)
{
	m_strText = text;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLNodeImpl::appendText(const OW_String& text)
{
	m_strText += text;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_XMLNodeImpl::getAttribute(const OW_String& name, OW_Bool throwException)
{
	int arraySize = m_XMLAttrArray.size();

	for (int i = 0; i < arraySize; i++)
	{
		OW_XMLAttribute attr = m_XMLAttrArray[i];

		// TODO:  Should this be case insensentive?
		if (attr.getName().equalsIgnoreCase(name))
		{
			return attr.getValue();
		}
	}
	if (throwException)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
				format("OW_XMLNodeImpl::getAttribute failed to find a matching "
					"attribute for name: %1", name).c_str() );
	}

	return OW_String();
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLAttrArray
OW_XMLNodeImpl::getAttrs()
{
	return m_XMLAttrArray;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_XMLNodeImpl::getText()
{
	return m_strText;
}

//////////////////////////////////////////////////////////////////////////////
int
OW_XMLNodeImpl::getToken()
{
	return m_iToken;
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNodeImplRef
OW_XMLNodeImpl::findElement(int elementToken, OW_Bool throwException) /*throw (OW_CIMException)*/
{
	OW_XMLNodeImplRef tmpRef(new OW_XMLNodeImpl(*this));

	for ( ;!tmpRef.isNull(); tmpRef = tmpRef->m_nextNode)
	{
		if (tmpRef->getToken() == elementToken)
		{
			return tmpRef;
		}
	}
	if (throwException)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
				format("OW_XMLNodeImpl::findElement failed to find a matching "
					"elementToken.  Token id = %1", elementToken).c_str() );
	}
	return OW_XMLNodeImplRef(NULL);
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNodeImplRef
OW_XMLNodeImpl::nextElement(int elementToken, OW_Bool throwException) /*throw (OW_CIMException)*/
{
	if (m_nextNode.isNull())
	{
		if (throwException)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, "OW_XMLNodeImpl::nextElement found a NULL element");
		}
		else
		{
			return OW_XMLNodeImplRef(0);
		}
	}
	if (m_nextNode->m_iToken == elementToken)
	{
		return m_nextNode;
	}

	if (throwException)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
				format("OW_XMLNodeImpl::nextElement didn't match elementToken. "
					"Token id=%1, found tokenid=%2",
					elementToken, m_nextNode->m_iToken ).c_str() );
	}

	return OW_XMLNodeImplRef(NULL);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLNodeImpl::mustElement(int elementToken) /*throw (OW_CIMException)*/
{
	if (elementToken != m_iToken)
		OW_THROWCIMMSG(OW_CIMException::FAILED,
				format("OW_XMLNodeImpl::mustElement: elementToken did not match "
				"node. Token id=%1, found=%2", elementToken, m_iToken ).c_str() );
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNodeImplRef
OW_XMLNodeImpl::mustElementChild(int elementToken)	/*throw (OW_CIMException)*/
{
	mustElement(elementToken);

	if (m_childNode.isNull())
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
				format("OW_XMLNodeImpl::mustElementChild found a NULL child. "
					"Token id=%1",
					elementToken ).c_str() );
	}

	return m_childNode;
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNodeImplRef
OW_XMLNodeImpl::mustChildElement(int elementToken)	/*throw (OW_CIMException)*/
{
	if (m_childNode.isNull())
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
				format("OW_XMLNodeImpl::mustChildElement found a NULL child. "
					"Token id=%1",
					elementToken ).c_str() );
	}

	if (m_childNode->m_iToken != elementToken)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
				format("OW_XMLNodeImpl::mustChildElement: elementToken did not match "
				"child. "
				"Token id=%1, found tokenid=%2",
				elementToken, m_childNode->getToken() ).c_str() );
	}
	return m_childNode;
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNodeImplRef
OW_XMLNodeImpl::mustChildElementChild(int elementToken) /*throw (OW_CIMException)*/
{
	if (m_childNode.isNull())
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
				format("OW_XMLNodeImpl::mustChildElementChild found a NULL child. "
				"Token id=%1",
				elementToken ).c_str() );
	}

	if (m_childNode->m_iToken != elementToken)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
				format("OW_XMLNodeImpl::mustChildElementChild: elementToken did "
				"not match child. "
				"Token id=%1, found tokenid=%2",
				elementToken, m_childNode->getToken() ).c_str() );
	}

	if (m_childNode->m_childNode.isNull())
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
				format("OW_XMLNodeImpl::mustChildElementChild found a NULL child "
				"of child Node. "
				"Token id=%1, found tokenid=%2",
				elementToken, m_childNode->getToken() ).c_str() );
	}

	return(m_childNode->m_childNode);
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNodeImplRef
OW_XMLNodeImpl::mustChildFindElement(int elementToken) /*throw (OW_CIMException)*/
{
	if (m_childNode.isNull())
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
				format("OW_XMLNodeImpl::mustChildElementChild found a NULL child. "
				"Token id=%1",
				elementToken ).c_str() );
	}

	return(m_childNode->findElement(elementToken, true));
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNodeImplRef
OW_XMLNodeImpl::findElementChild(int elementToken, OW_Bool throwException)	/*throw (OW_CIMException)*/
{
	OW_XMLNodeImplRef tmpRef = findElement(elementToken, throwException);

	if (tmpRef.isNull())
		return tmpRef;
	else
		return tmpRef->m_childNode;
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNodeImplRef
OW_XMLNodeImpl::mustChildFindElementChild(int elementToken)	/*throw (OW_CIMException)*/
{
	return m_childNode->findElementChild(elementToken, true);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLNodeImpl::setNext(OW_XMLNodeImplRef node)
{
	m_nextNode = node;
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNodeImplRef
OW_XMLNodeImpl::getNext()
{
	return m_nextNode;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLNodeImpl::addChild(const OW_XMLNodeImplRef& node)
{
	if (m_childNode.isNull())
	{
		m_childNode=node;
		m_lastChildNode=node;
	}
	else
	{
		m_lastChildNode->m_nextNode=node;
		m_lastChildNode=node;
	}
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNodeImplRef
OW_XMLNodeImpl::mustGetChild() /*throw (OW_CIMException)*/
{
	if (m_childNode.isNull())
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
				"OW_XMLNodeImpl::mustGetChild found a NULL child");
	}

	return m_childNode;
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNodeImplRef
OW_XMLNodeImpl::getChild()
{
	return m_childNode;
}

//////////////////////////////////////////////////////////////////////////////
OW_StringArray
OW_XMLNodeImpl::extractParameterStringArray(const OW_String& value, OW_Bool& found)
/*throw (OW_CIMException)*/
{
	OW_StringArray strArray;

	OW_XMLNode node(OW_XMLNodeImplRef(new OW_XMLNodeImpl(*this)));
	found = false;

	for (; node; node = node.getNext())
	{
		OW_XMLAttribute nattr;
		OW_XMLAttrArray nodeattrs;

		if (node.getToken() != OW_XMLNode::XML_ELEMENT_IPARAMVALUE)
			continue;

		nodeattrs = node.getAttrs();
		if (nodeattrs.size() == 0)
			continue;

		for (unsigned int j = 0; j < nodeattrs.size(); j++)
		{
			nattr = nodeattrs[j];
			if (nattr.getName().equalsIgnoreCase(OW_XMLOperationGeneric::paramName)
				 && nattr.getValue().equalsIgnoreCase(value))
			{
				node = node.getChild();
				if (!node)
				{
					return strArray;
				}

				if (node.getToken() != OW_XMLNode::XML_ELEMENT_VALUE_ARRAY)
				{
					OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
						  "Could not find <VALUE.ARRAY> element for parameter ");
				}
				node = node.getChild();
				while(node)
				{
					OW_String value = node.getText();
					strArray.push_back(value);
					node = node.getNext();
				}
				found = true;
				return strArray;
			}
		}
	}
	return strArray;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_XMLNodeImpl::extractParameterValueAttr(const OW_String& iparamName,
	OW_XMLNode::tokenId tokenid, const OW_String& attrName)
{
	OW_XMLNode tmpNode(OW_XMLNodeImplRef(new OW_XMLNodeImpl(*this)));

	for (; tmpNode; tmpNode = tmpNode.getNext())
	{
		if (tmpNode.getToken() != OW_XMLNode::XML_ELEMENT_IPARAMVALUE)
		{
			continue;
		}

		OW_String attr = tmpNode.getAttribute(OW_XMLOperationGeneric::paramName);

		if (!attr.equalsIgnoreCase(iparamName))
		{
			continue;
		}

		tmpNode = tmpNode.getChild();

		if(!tmpNode)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				"Needed a child for <IPARAMVALUE>");
		}

		if (tmpNode.getToken() == tokenid)
		{
			return tmpNode.mustGetAttribute(attrName);
		}

		break;
	}

	return OW_String();
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_XMLNodeImpl::extractParameterValue(const OW_String& value,
	OW_Bool defaultValue)
/*throw (OW_CIMException)*/
{
	OW_String res=extractParameterValue(value,OW_String());

	if (res.length() == 0)
		return defaultValue;

	if (res.equalsIgnoreCase("TRUE"))
		return true;

	return false;

}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_XMLNodeImpl::extractParameterValue(const OW_String& value,
	const OW_String& defaultValue)
/*throw (OW_CIMException)*/
{
	OW_XMLNode node(OW_XMLNodeImplRef(new OW_XMLNodeImpl(*this)));

	for (; node; node = node.getNext())
	{
		OW_XMLAttribute nattr;
		OW_XMLAttrArray nodeattrs;

		if (node.getToken() != OW_XMLNode::XML_ELEMENT_IPARAMVALUE)
			continue;

		nodeattrs = node.getAttrs();
		if (nodeattrs.size() == 0)
			continue;

		for (unsigned int j = 0; j < nodeattrs.size(); j++)
		{
			nattr = nodeattrs[j];
			if (nattr.getName().equalsIgnoreCase(OW_XMLOperationGeneric::paramName)
				 && nattr.getValue().equalsIgnoreCase(value))
			{
				node = node.getChild();
				if (!node)
					return defaultValue;
				node = node.findElement(OW_XMLNode::XML_ELEMENT_VALUE);
				if (!node)
					OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER,
								  "Could not find <VALUE> element for parameter ");
				OW_String retvalue = node.getText();
				return retvalue;
			}
		}
	}
	return defaultValue;
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNodeImplRef
OW_XMLNodeImpl::findElementAndParameter(const OW_String& nameOfParameter)
/*throw (OW_CIMException)*/
{
	OW_XMLNode node(OW_XMLNodeImplRef(new OW_XMLNodeImpl(*this)));

	for (; node; node = node.getNext())
	{
		if (node.getToken()!=OW_XMLNode::XML_ELEMENT_IPARAMVALUE)
			continue;

		OW_XMLAttrArray nodeattrs=node.getAttrs();

		if (nodeattrs.size() == 0)
			continue;

		for (unsigned int j = 0; j < nodeattrs.size(); j++)
		{
			OW_XMLAttribute nattr = nodeattrs[j];
			if (nattr.getName().equalsIgnoreCase(OW_XMLOperationGeneric::paramName)
				 && nattr.getValue().equalsIgnoreCase(nameOfParameter))
				return(node.getChild().m_impl);
		}
	}
	return OW_XMLNodeImplRef(); // TODO will this work? (instead of null)
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_XMLNodeImpl::mustExtractParameterValue(const OW_String& value)
//throw(OW_CIMException)
{
	OW_String ret = extractParameterValue(value,OW_String());
	if (ret.length() == 0)
		OW_THROWCIMMSG(OW_CIMException::INVALID_PARAMETER, "Missing required parameter");
	return ret;
}

//*************************************************************************
// OW_XMLNode Implementation
//*************************************************************************

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode::OW_XMLNode(const OW_XMLNode& arg) :
	m_impl(arg.m_impl)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode::OW_XMLNode(const OW_String& name, const OW_XMLAttrArray& attrArray)
	: m_impl(new OW_XMLNodeImpl(name, attrArray))
{
	this->setNodeType(name);
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode::OW_XMLNode(OW_XMLNodeImplRef ref) : m_impl(ref)
{
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode::OW_XMLNode(const OW_String& name) : m_impl(new OW_XMLNodeImpl(name))
{
}


//////////////////////////////////////////////////////////////////////////////
OW_XMLNode::OW_XMLNode() : m_impl(NULL)
{
	initElementMap();
}

//////////////////////////////////////////////////////////////////////////////
// Put here to avoid dtor inline
OW_XMLNode::~OW_XMLNode()
{
}

//////////////////////////////////////////////////////////////////////////////
int
OW_XMLNode::getNodeType() const
{
	return m_impl->getNodeType();
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode&
OW_XMLNode::operator= (const OW_XMLNode & arg)
{
	m_impl = arg.m_impl;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLNode::setToNULL()
{
	OW_XMLNodeImplRef tmpRef(NULL);
	m_impl = tmpRef;
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_XMLNode::getNodeName() const
{
	return m_impl->getNodeName();
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLNode::assignText(const OW_String& text) const
{
	m_impl->assignText(text);
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLNode::appendText(const OW_String& text) const
{
	m_impl->appendText(text);
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_XMLNode::getAttribute(const OW_String& name) const
{
	return m_impl->getAttribute(name);
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_XMLNode::mustGetAttribute(const OW_String& name) const /*throw (OW_CIMException)*/
{
	return m_impl->getAttribute(name, true);
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLAttrArray
OW_XMLNode::getAttrs() const
{
	return m_impl->getAttrs();
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_XMLNode::getText() const
{
	return m_impl->getText();
}

//////////////////////////////////////////////////////////////////////////////
int
OW_XMLNode::getToken() const
{
	return m_impl->getToken();
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode
OW_XMLNode::findElement(int elementToken) const
{
	return OW_XMLNode(m_impl->findElement(elementToken));
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode
OW_XMLNode::mustFindElement(int elementToken) const  /*throw (OW_CIMException)*/
{
	return OW_XMLNode(m_impl->findElement(elementToken, true));
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode
OW_XMLNode::nextElement(int elementToken)
{
	return OW_XMLNode(m_impl->nextElement(elementToken));
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode
OW_XMLNode::mustNextElement(int elementToken) const /*throw (OW_CIMException)*/
{
	return OW_XMLNode(m_impl->nextElement(elementToken, true));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLNode::mustElement(int elementToken) const	/*throw (OW_CIMException)*/
{
	m_impl->mustElement(elementToken);
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode
OW_XMLNode::mustElementChild(int elementToken) const /*throw (OW_CIMException)*/
{
	return OW_XMLNode(m_impl->mustElementChild(elementToken));
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode
OW_XMLNode::mustChildElement(int elementToken) const /*throw (OW_CIMException)*/
{
	return OW_XMLNode(m_impl->mustChildElement(elementToken));
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode
OW_XMLNode::mustChildElementChild(int elementToken) const /*throw (OW_CIMException)*/
{
	return OW_XMLNode(m_impl->mustChildElementChild(elementToken));
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode
OW_XMLNode::mustChildFindElement(int elementToken) const	/*throw (OW_CIMException)*/
{
	return OW_XMLNode(m_impl->mustChildFindElement(elementToken));
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode
OW_XMLNode::findElementChild(int elementToken) const
{
	return OW_XMLNode(m_impl->findElementChild(elementToken));
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode
OW_XMLNode::mustFindElementChild(int elementToken) const	/*throw (OW_CIMException)*/
{
	return OW_XMLNode(m_impl->findElementChild(elementToken, true));
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode
OW_XMLNode::mustChildFindElementChild(int elementToken) const /*throw (OW_CIMException)*/
{
	return OW_XMLNode(m_impl->mustChildFindElementChild(elementToken));
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLNode::setNext(const OW_XMLNode& node) const
{
	m_impl->setNext(node.m_impl);
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode
OW_XMLNode::getNext() const
{
	return OW_XMLNode(m_impl->getNext());
}

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLNode::addChild(const OW_XMLNode& node) const
{
	m_impl->addChild(node.m_impl);
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode
OW_XMLNode::mustGetChild() const	/*throw (OW_CIMException)*/
{
	return OW_XMLNode(m_impl->mustGetChild());
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode
OW_XMLNode::getChild() const
{
	return OW_XMLNode(m_impl->getChild());
}

//////////////////////////////////////////////////////////////////////////////
OW_XMLNode
OW_XMLNode::findElementAndParameter(const OW_String& nameOfParameter)
/*throw (OW_CIMException)*/
{
	return OW_XMLNode(m_impl->findElementAndParameter(nameOfParameter));
}

//*************************************************************************
// OW_XMLNode Private Method Implementation
//*************************************************************************

//////////////////////////////////////////////////////////////////////////////
void
OW_XMLNode::setNodeType(OW_String nodeName)
{
	// We are going to search a global static so we need a little
	// thread protection here
	OW_MutexLock myLock(g_ElementMapMutex);

	m_impl->setNodeType(g_elementMap[nodeName]);
}

//////////////////////////////////////////////////////////////////////////////
OW_StringArray
OW_XMLNode::extractParameterStringArray(const OW_String& value, OW_Bool& found)
{
	found = false;
	return m_impl->extractParameterStringArray(value, found);
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_XMLNode::extractParameterValueAttr(const OW_String& iparamName,
												  tokenId tokenid, const OW_String& attrName)
/*throw (OW_CIMException)*/
{
	return m_impl->extractParameterValueAttr(iparamName, tokenid,
														  attrName);
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_XMLNode::extractParameterValue(const OW_String& value, OW_Bool defaultValue)
/*throw (OW_CIMException)*/
{
	return m_impl->extractParameterValue(value, defaultValue);
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_XMLNode::extractParameterValue(const OW_String& value,
											 const OW_String& defaultValue) /*throw (OW_CIMException)*/
{
	return m_impl->extractParameterValue(value, defaultValue);
}

//////////////////////////////////////////////////////////////////////////////
OW_String
OW_XMLNode::mustExtractParameterValue(const OW_String& value)
//throw(OW_CIMException)
{
	return m_impl->mustExtractParameterValue(value);
}

//////////////////////////////////////////////////////////////////////////////
static void
initElementMap()
{
	// We are going to search a global static so we need a little
	// thread protection here
	OW_MutexLock myLock(g_ElementMapMutex);

	if (g_initElementMap)
	{
		g_elementMap["CIM"] = OW_XMLNode::XML_ELEMENT_CIM;
		g_elementMap["MESSAGE"] = OW_XMLNode::XML_ELEMENT_MESSAGE;
		g_elementMap["SIMPLEREQ"] = OW_XMLNode::XML_ELEMENT_SIMPLEREQ;
		g_elementMap["IMETHODCALL"] = OW_XMLNode::XML_ELEMENT_IMETHODCALL;
		g_elementMap["LOCALNAMESPACEPATH"] = OW_XMLNode::XML_ELEMENT_LOCALNAMESPACEPATH;
		g_elementMap["NAMESPACEPATH"] = OW_XMLNode::XML_ELEMENT_NAMESPACEPATH;
		g_elementMap["NAMESPACE"] = OW_XMLNode::XML_ELEMENT_NAMESPACE;
		g_elementMap["PARAMVALUE"] = OW_XMLNode::XML_ELEMENT_PARAMVALUE;
		g_elementMap["IMETHODRESPONSE"] = OW_XMLNode::XML_ELEMENT_IMETHODRESPONSE;
		g_elementMap["VALUE"] = OW_XMLNode::XML_ELEMENT_VALUE;
		g_elementMap["IRETURNVALUE"] = OW_XMLNode::XML_ELEMENT_IRETURNVALUE;
		g_elementMap["VALUE.NAMEDOBJECT"] = OW_XMLNode::XML_ELEMENT_VALUE_NAMEDOBJECT;
		g_elementMap["CLASS"] = OW_XMLNode::XML_ELEMENT_CLASS;
		g_elementMap["QUALIFIER"] = OW_XMLNode::XML_ELEMENT_QUALIFIER;
		g_elementMap["VALUE.ARRAY"] = OW_XMLNode::XML_ELEMENT_VALUE_ARRAY;
		g_elementMap["SIMPLERSP"] = OW_XMLNode::XML_ELEMENT_SIMPLERSP;
		g_elementMap["PROPERTY.ARRAY"] = OW_XMLNode::XML_ELEMENT_PROPERTY_ARRAY;
		g_elementMap["PROPERTY"] = OW_XMLNode::XML_ELEMENT_PROPERTY;
		g_elementMap["PARAMETER.REFERENCE"] = OW_XMLNode::XML_ELEMENT_PARAMETER_REFERENCE;
		g_elementMap["PARAMETER.ARRAY"] = OW_XMLNode::XML_ELEMENT_PARAMETER_ARRAY;
		g_elementMap["PARAMETER.REFARRAY"] = OW_XMLNode::XML_ELEMENT_PARAMETER_REFARRAY;
		g_elementMap["PARAMETER"] = OW_XMLNode::XML_ELEMENT_PARAMETER;
		g_elementMap["METHOD"] = OW_XMLNode::XML_ELEMENT_METHOD;
		g_elementMap["INSTANCE"] = OW_XMLNode::XML_ELEMENT_INSTANCE;
		g_elementMap["INSTANCENAME"] = OW_XMLNode::XML_ELEMENT_INSTANCENAME;
		g_elementMap["KEYBINDING"] = OW_XMLNode::XML_ELEMENT_KEYBINDING;
		g_elementMap["KEYVALUE"] = OW_XMLNode::XML_ELEMENT_KEYVALUE;
		g_elementMap["IMPLICITKEY"] = OW_XMLNode::XML_ELEMENT_IMPLICITKEY;
		g_elementMap["METHODCALL"] = OW_XMLNode::XML_ELEMENT_METHODCALL;
		g_elementMap["RETURNVALUE"] = OW_XMLNode::XML_ELEMENT_RETURNVALUE;
		g_elementMap["METHODRESPONSE"] = OW_XMLNode::XML_ELEMENT_METHODRESPONSE;
		g_elementMap["PROPERTY.REFERENCE"] = OW_XMLNode::XML_ELEMENT_PROPERTY_REF;
		g_elementMap["ERROR"] = OW_XMLNode::XML_ELEMENT_ERROR;
		g_elementMap["QUALIFIER.DECLARATION"] = OW_XMLNode::XML_ELEMENT_QUALIFIER_DECLARATION;
		g_elementMap["SCOPE"] = OW_XMLNode::XML_ELEMENT_SCOPE;

		g_elementMap["VALUE.OBJECT"] = OW_XMLNode::XML_ELEMENT_VALUE_OBJECT;
		g_elementMap["IPARAMVALUE"] = OW_XMLNode::XML_ELEMENT_IPARAMVALUE;
		g_elementMap["VALUE.REFERENCE"] = OW_XMLNode::XML_ELEMENT_VALUE_REFERENCE;
		g_elementMap["VALUE.REFARRAY"] = OW_XMLNode::XML_ELEMENT_VALUE_REFARRAY;
		g_elementMap["CLASSPATH"] = OW_XMLNode::XML_ELEMENT_CLASSPATH;
		g_elementMap["LOCALCLASSPATH"] = OW_XMLNode::XML_ELEMENT_LOCALCLASSPATH;
		g_elementMap["CLASSNAME"] = OW_XMLNode::XML_ELEMENT_CLASSNAME;
		g_elementMap["INSTANCEPATH"] = OW_XMLNode::XML_ELEMENT_INSTANCEPATH;
		g_elementMap["LOCALINSTANCEPATH"] = OW_XMLNode::XML_ELEMENT_LOCALINSTANCEPATH;
		g_elementMap["HOST"] = OW_XMLNode::XML_ELEMENT_HOST;

		g_elementMap["VALUE.OBJECTWITHLOCALPATH"] = OW_XMLNode::XML_ELEMENT_VALUE_OBJECTWITHLOCALPATH;
		g_elementMap["VALUE.OBJECTWITHPATH"] = OW_XMLNode::XML_ELEMENT_VALUE_OBJECTWITHPATH;
		g_elementMap["DECLARATION"] = OW_XMLNode::XML_ELEMENT_DECLARATION;
		g_elementMap["DECLGROUP"] = OW_XMLNode::XML_ELEMENT_DECLGROUP;
		g_elementMap["DECLGROUP.WITHNAME"] = OW_XMLNode::XML_ELEMENT_DECLGROUP_WITHNAME;
		g_elementMap["DECLGROUP.WITHPATH"] = OW_XMLNode::XML_ELEMENT_DECLGROUP_WITHPATH;
		g_elementMap["OBJECTPATH"] = OW_XMLNode::XML_ELEMENT_OBJECTPATH;
		g_elementMap["VALUE.NAMEDINSTANCE"] = OW_XMLNode::XML_ELEMENT_VALUE_NAMEDINSTANCE;

		g_elementMap["MULTIREQ"] = OW_XMLNode::XML_ELEMENT_MULTIREQ;
		g_elementMap["MULTIRSP"] = OW_XMLNode::XML_ELEMENT_MULTIRSP;

		g_elementMap["SIMPLEEXPREQ"] = OW_XMLNode::XML_ELEMENT_SIMPLEEXPREQ;
		g_elementMap["SIMPLEEXPRSP"] = OW_XMLNode::XML_ELEMENT_SIMPLEEXPRSP;
		g_elementMap["MULTIEXPREQ"] = OW_XMLNode::XML_ELEMENT_MULTIEXPREQ;
		g_elementMap["MULTIEXPRSP"] = OW_XMLNode::XML_ELEMENT_MULTIEXPRSP;
		g_elementMap["EXPMETHODCALL"] = OW_XMLNode::XML_ELEMENT_EXPMETHODCALL;
		g_elementMap["EXPMETHODRESPONSE"] = OW_XMLNode::XML_ELEMENT_EXPMETHODRESPONSE;

		g_initElementMap = false;
	}
}
