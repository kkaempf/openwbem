/*******************************************************************************
* Copyright (C) 2004 Vintela, Inc. All rights reserved.
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

#include "OW_config.h"
#include "OW_XMLNode.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_StringStream.hpp"
#include "OW_XMLEscape.hpp"

#ifdef OW_HAVE_OSTREAM
#include <ostream>
#else
#include <iostream>
#endif

namespace OpenWBEM
{

//////////////////////////////////////////////////////////////////////////////
XMLNodeImpl::XMLNodeImpl(const String& name,
	const XMLAttributeArray& attrArray) :
	m_nextNode(NULL), m_childNode(NULL), m_lastChildNode(NULL),
	m_XMLAttributeArray(attrArray), m_strName(name), m_strText()
{
}

//////////////////////////////////////////////////////////////////////////////
XMLNodeImpl::XMLNodeImpl(const String& name) :
	m_nextNode(NULL), m_childNode(NULL), m_lastChildNode(NULL),
	m_XMLAttributeArray(), m_strName(name), m_strText()
{
}

//////////////////////////////////////////////////////////////////////////////
XMLNodeImpl::XMLNodeImpl() :
	m_nextNode(NULL), m_childNode(NULL), m_lastChildNode(NULL),
	m_XMLAttributeArray(), m_strName(), m_strText()
{
}

//////////////////////////////////////////////////////////////////////////////
void
XMLNodeImpl::assignText(const String& text)
{
	m_strText = text;
}

//////////////////////////////////////////////////////////////////////////////
void
XMLNodeImpl::appendText(const String& text)
{
	m_strText += text;
}

//////////////////////////////////////////////////////////////////////////////
String
XMLNodeImpl::getAttribute(const String& name, bool throwException) const
{
	int arraySize = m_XMLAttributeArray.size();

	for (int i = 0; i < arraySize; i++)
	{
		XMLAttribute attr = m_XMLAttributeArray[i];

		// Should this be case insensentive? NO
		if (attr.getName().equals(name))
		{
			return attr.getValue();
		}
	}
	if (throwException)
	{
		OW_THROWCIMMSG(CIMException::INVALID_PARAMETER,
				Format("Failed to find "
					"attribute: %1 in node: %2", name, m_strName).c_str() );
	}

	return String();
}

//////////////////////////////////////////////////////////////////////////////
XMLAttributeArray
XMLNodeImpl::getAttrs() const
{
	return m_XMLAttributeArray;
}

//////////////////////////////////////////////////////////////////////////////
String
XMLNodeImpl::getText() const
{
	return m_strText;
}

//////////////////////////////////////////////////////////////////////////////
String 
XMLNodeImpl::getName() const
{
	return m_strName;
}

//////////////////////////////////////////////////////////////////////////////
XMLNodeImplRef
XMLNodeImpl::findElement(const char* elementName, bool throwException) const
{
	XMLNodeImplRef tmpRef(new XMLNodeImpl(*this));

	for ( ;!tmpRef.isNull(); tmpRef = tmpRef->m_nextNode)
	{
		if (tmpRef->getName() == elementName)
		{
			return tmpRef;
		}
	}
	if (throwException)
	{
		OW_THROWCIMMSG(CIMException::FAILED,
				Format("XMLNodeImpl::findElement failed to find a matching "
					"elementName.  Token id = %1", elementName).c_str() );
	}
	return XMLNodeImplRef(NULL);
}

//////////////////////////////////////////////////////////////////////////////
XMLNodeImplRef
XMLNodeImpl::nextElement(const char* elementName, bool throwException) const
{
	if (m_nextNode.isNull())
	{
		if (throwException)
		{
			OW_THROWCIMMSG(CIMException::FAILED, "XMLNodeImpl::nextElement found a NULL element");
		}
		else
		{
			return XMLNodeImplRef(0);
		}
	}
	if (m_nextNode->getName() == elementName)
	{
		return m_nextNode;
	}

	if (throwException)
	{
		OW_THROWCIMMSG(CIMException::FAILED,
				Format("XMLNodeImpl::nextElement didn't match elementName. "
					"Token id=%1, found tokenid=%2",
					elementName, m_nextNode->getName() ).c_str() );
	}

	return XMLNodeImplRef(NULL);
}

//////////////////////////////////////////////////////////////////////////////
void
XMLNodeImpl::mustElement(const char* elementName) const
{
	if (elementName != getName())
		OW_THROWCIMMSG(CIMException::FAILED,
				Format("XMLNodeImpl::mustElement: elementName did not match "
				"node. Token id=%1, found=%2", elementName, getName() ).c_str() );
}

//////////////////////////////////////////////////////////////////////////////
XMLNodeImplRef
XMLNodeImpl::mustElementChild(const char* elementName)	const
{
	mustElement(elementName);

	if (m_childNode.isNull())
	{
		OW_THROWCIMMSG(CIMException::FAILED,
				Format("XMLNodeImpl::mustElementChild found a NULL child. "
					"Token id=%1",
					elementName ).c_str() );
	}

	return m_childNode;
}

//////////////////////////////////////////////////////////////////////////////
XMLNodeImplRef
XMLNodeImpl::mustChildElement(const char* elementName)	const
{
	if (m_childNode.isNull())
	{
		OW_THROWCIMMSG(CIMException::FAILED,
				Format("XMLNodeImpl::mustChildElement found a NULL child. "
					"Token id=%1",
					elementName ).c_str() );
	}

	if (m_childNode->getName() != elementName)
	{
		OW_THROWCIMMSG(CIMException::FAILED,
				Format("XMLNodeImpl::mustChildElement: elementName did not match "
				"child. "
				"Token id=%1, found tokenid=%2",
				elementName, m_childNode->getName() ).c_str() );
	}
	return m_childNode;
}

//////////////////////////////////////////////////////////////////////////////
XMLNodeImplRef
XMLNodeImpl::mustChildElementChild(const char* elementName) const
{
	if (m_childNode.isNull())
	{
		OW_THROWCIMMSG(CIMException::FAILED,
				Format("XMLNodeImpl::mustChildElementChild found a NULL child. "
				"Token id=%1",
				elementName ).c_str() );
	}

	if (m_childNode->getName() != elementName)
	{
		OW_THROWCIMMSG(CIMException::FAILED,
				Format("XMLNodeImpl::mustChildElementChild: elementName did "
				"not match child. "
				"Token id=%1, found tokenid=%2",
				elementName, m_childNode->getName() ).c_str() );
	}

	if (m_childNode->m_childNode.isNull())
	{
		OW_THROWCIMMSG(CIMException::FAILED,
				Format("XMLNodeImpl::mustChildElementChild found a NULL child "
				"of child Node. "
				"Token id=%1, found tokenid=%2",
				elementName, m_childNode->getName() ).c_str() );
	}

	return(m_childNode->m_childNode);
}

//////////////////////////////////////////////////////////////////////////////
XMLNodeImplRef
XMLNodeImpl::mustChildFindElement(const char* elementName) const
{
	if (m_childNode.isNull())
	{
		OW_THROWCIMMSG(CIMException::FAILED,
				Format("XMLNodeImpl::mustChildElementChild found a NULL child. "
				"Token id=%1",
				elementName ).c_str() );
	}

	return(m_childNode->findElement(elementName, true));
}

//////////////////////////////////////////////////////////////////////////////
XMLNodeImplRef
XMLNodeImpl::findElementChild(const char* elementName, bool throwException)	const
{
	XMLNodeImplRef tmpRef = findElement(elementName, throwException);

	if (tmpRef.isNull())
		return tmpRef;
	else
		return tmpRef->m_childNode;
}

//////////////////////////////////////////////////////////////////////////////
XMLNodeImplRef
XMLNodeImpl::mustChildFindElementChild(const char* elementName)	const
{
	return m_childNode->findElementChild(elementName, true);
}

//////////////////////////////////////////////////////////////////////////////
void
XMLNodeImpl::setNext(XMLNodeImplRef node)
{
	m_nextNode = node;
}

//////////////////////////////////////////////////////////////////////////////
XMLNodeImplRef
XMLNodeImpl::getNext() const
{
	return m_nextNode;
}

//////////////////////////////////////////////////////////////////////////////
void
XMLNodeImpl::addChild(const XMLNodeImplRef& node)
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
XMLNodeImplRef
XMLNodeImpl::mustGetChild() const
{
	if (m_childNode.isNull())
	{
		OW_THROWCIMMSG(CIMException::FAILED,
				"XMLNodeImpl::mustGetChild found a NULL child");
	}

	return m_childNode;
}

//////////////////////////////////////////////////////////////////////////////
XMLNodeImplRef
XMLNodeImpl::getChild() const
{
	return m_childNode;
}

//////////////////////////////////////////////////////////////////////////////
XMLNodeArray
XMLNodeImpl::getChildren() const
{
	XMLNodeArray ar;
	if( m_childNode.isNull() )
		return ar;
	XMLNodeImplRef r = m_childNode;
	do
	{
		ar.push_back(r);
		r = r->m_nextNode;
	} while( !r.isNull() );

	return ar;
}

//////////////////////////////////////////////////////////////////////////////
void
XMLNodeImpl::printNode( std::ostream& ostr ) const
{
	String name = getName();
	XMLAttributeArray aa = getAttrs();

	ostr << "<" << name;
	for( XMLAttributeArray::const_iterator aiter = aa.begin();
			aiter != aa.end(); ++aiter )
	{
		ostr << " " << aiter->getName() << "=\"" << XMLEscape(aiter->getValue()) << '"';
	}
	ostr << ">";
	ostr << XMLEscape(getText());
	XMLNode curChild = getChild();
	while (curChild)
	{
		curChild.printNode(ostr);
		curChild = curChild.getNext();
	}
	ostr << "</" << name << ">";
}

//////////////////////////////////////////////////////////////////////////////
String
XMLNodeImpl::toString() const
{
	OStringStream ss;
	printNode( ss );
	return ss.releaseString();
}


//*************************************************************************
// XMLNode Implementation
//*************************************************************************

//////////////////////////////////////////////////////////////////////////////
XMLNode::XMLNode(const XMLNode& arg) :
	m_impl(arg.m_impl)
{
}

//////////////////////////////////////////////////////////////////////////////
XMLNode::XMLNode(const String& name, const XMLAttributeArray& attrArray)
	: m_impl(new XMLNodeImpl(name, attrArray))
{
}

//////////////////////////////////////////////////////////////////////////////
XMLNode::XMLNode(XMLNodeImplRef ref) : m_impl(ref)
{
}

//////////////////////////////////////////////////////////////////////////////
XMLNode::XMLNode(const String& name) : m_impl(new XMLNodeImpl(name))
{
}


//////////////////////////////////////////////////////////////////////////////
XMLNode::XMLNode() : m_impl(NULL)
{
}

//////////////////////////////////////////////////////////////////////////////
// Put here to avoid dtor inline
XMLNode::~XMLNode()
{
}

//////////////////////////////////////////////////////////////////////////////
XMLNode&
XMLNode::operator= (const XMLNode & arg)
{
	m_impl = arg.m_impl;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
void
XMLNode::setToNULL()
{
	XMLNodeImplRef tmpRef(NULL);
	m_impl = tmpRef;
}

//////////////////////////////////////////////////////////////////////////////
void
XMLNode::assignText(const String& text) const
{
	m_impl->assignText(text);
}

//////////////////////////////////////////////////////////////////////////////
void
XMLNode::appendText(const String& text) const
{
	m_impl->appendText(text);
}

//////////////////////////////////////////////////////////////////////////////
String
XMLNode::getAttribute(const String& name) const
{
	return m_impl->getAttribute(name);
}

//////////////////////////////////////////////////////////////////////////////
String
XMLNode::mustGetAttribute(const String& name) const 
{
	return m_impl->getAttribute(name, true);
}

//////////////////////////////////////////////////////////////////////////////
XMLAttributeArray
XMLNode::getAttrs() const
{
	return m_impl->getAttrs();
}

//////////////////////////////////////////////////////////////////////////////
String
XMLNode::getText() const
{
	return m_impl->getText();
}

//////////////////////////////////////////////////////////////////////////////
String
XMLNode::getName() const
{
	return m_impl->getName();
}

//////////////////////////////////////////////////////////////////////////////
XMLNode
XMLNode::findElement(const char* elementName) const
{
	return XMLNode(m_impl->findElement(elementName));
}

//////////////////////////////////////////////////////////////////////////////
XMLNode
XMLNode::mustFindElement(const char* elementName) const  
{
	return XMLNode(m_impl->findElement(elementName, true));
}

//////////////////////////////////////////////////////////////////////////////
XMLNode
XMLNode::nextElement(const char* elementName)
{
	return XMLNode(m_impl->nextElement(elementName));
}

//////////////////////////////////////////////////////////////////////////////
XMLNode
XMLNode::mustNextElement(const char* elementName) const 
{
	return XMLNode(m_impl->nextElement(elementName, true));
}

//////////////////////////////////////////////////////////////////////////////
void
XMLNode::mustElement(const char* elementName) const	
{
	m_impl->mustElement(elementName);
}

//////////////////////////////////////////////////////////////////////////////
XMLNode
XMLNode::mustElementChild(const char* elementName) const 
{
	return XMLNode(m_impl->mustElementChild(elementName));
}

//////////////////////////////////////////////////////////////////////////////
XMLNode
XMLNode::mustChildElement(const char* elementName) const 
{
	return XMLNode(m_impl->mustChildElement(elementName));
}

//////////////////////////////////////////////////////////////////////////////
XMLNode
XMLNode::mustChildElementChild(const char* elementName) const 
{
	return XMLNode(m_impl->mustChildElementChild(elementName));
}

//////////////////////////////////////////////////////////////////////////////
XMLNode
XMLNode::mustChildFindElement(const char* elementName) const	
{
	return XMLNode(m_impl->mustChildFindElement(elementName));
}

//////////////////////////////////////////////////////////////////////////////
XMLNode
XMLNode::findElementChild(const char* elementName) const
{
	return XMLNode(m_impl->findElementChild(elementName));
}

//////////////////////////////////////////////////////////////////////////////
XMLNode
XMLNode::mustFindElementChild(const char* elementName) const	
{
	return XMLNode(m_impl->findElementChild(elementName, true));
}

//////////////////////////////////////////////////////////////////////////////
XMLNode
XMLNode::mustChildFindElementChild(const char* elementName) const 
{
	return XMLNode(m_impl->mustChildFindElementChild(elementName));
}

//////////////////////////////////////////////////////////////////////////////
void
XMLNode::setNext(const XMLNode& node) const
{
	m_impl->setNext(node.m_impl);
}

//////////////////////////////////////////////////////////////////////////////
XMLNode
XMLNode::getNext() const
{
	return XMLNode(m_impl->getNext());
}

//////////////////////////////////////////////////////////////////////////////
void
XMLNode::addChild(const XMLNode& node) const
{
	m_impl->addChild(node.m_impl);
}

//////////////////////////////////////////////////////////////////////////////
XMLNode
XMLNode::mustGetChild() const
{
	return XMLNode(m_impl->mustGetChild());
}

//////////////////////////////////////////////////////////////////////////////
XMLNode
XMLNode::getChild() const
{
	return XMLNode(m_impl->getChild());
}

//////////////////////////////////////////////////////////////////////////////
XMLNodeArray
XMLNode::getChildren() const
{
	return m_impl->getChildren();
}

//////////////////////////////////////////////////////////////////////////////
void
XMLNode::printNode( std::ostream& ostr ) const 
{
	m_impl->printNode( ostr );
}

//////////////////////////////////////////////////////////////////////////////
String
XMLNode::toString() const
{
	return m_impl->toString();
}


//////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& ostr, const XMLNode& node)
{
	node.printNode(ostr);
	return ostr;
}


} // end namespace OpenWBEM

