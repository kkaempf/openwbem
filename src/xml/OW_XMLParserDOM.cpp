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

/**
 * @author Dan Nuffer
 */


#include "OW_config.h"
#include "OW_XMLParserDOM.hpp"
#include "OW_XMLParserSAX.hpp"
#include "OW_XMLNode.hpp"
#include "OW_XMLParserCore.hpp" // for XMLToken
#include "OW_XMLParseException.hpp"
#include "OW_Format.hpp"
#include "OW_ExceptionIds.hpp"

namespace OW_NAMESPACE
{

OW_DEFINE_EXCEPTION_WITH_ID(DOM);

using std::istream;

namespace
{

class DOMDocumentBuilder : public XMLParserSAX::SAXDocumentHandler
{
public:
	virtual void endDocument()
	{
	}
	virtual void startElement(const XMLToken &entry)
	{
		XMLAttributeArray newAttrArray;
		unsigned int len = entry.attributeCount;
		for (unsigned int index = 0; index < len; index++)
		{
			String nodeName = entry.attributes[index].name.toString();
			String nodeValue = entry.attributes[index].value.toString();
	
			XMLAttribute newAttribute(nodeName, nodeValue);
			newAttrArray.append(newAttribute);
		}
	
		XMLNode newNode(entry.text.toString(), newAttrArray);
	
		if (newNode)
		{
			if (!m_topNode)
			{
				m_topNode = newNode;
			}
	
			// If there is anything in the array, this is the child of that last guy
			if (m_nodeArray.size() > 0)
			{
				XMLNode parent = m_nodeArray[m_nodeArray.size() - 1];
				parent.addChild(newNode);
			}
	
			m_nodeArray.push_back(newNode);
		}
	}
	
	// name is not unescaped (implementation has to do it if necessary)
	virtual void endElement(const StringBuffer &name)
	{
		// at the end of the element, we just need to pop a node
		// off the stack
		m_nodeArray.pop_back();
	}
	// chars is not unescaped (implementation has to do it if necessary)
	virtual void characters(const StringBuffer &chars)
	{
		if (m_nodeArray.size() > 0)
		{
			XMLNode curNode = m_nodeArray[m_nodeArray.size() - 1];
			if (curNode != 0)
			{
				String utxt = chars.toString();
				curNode.appendText(utxt);
			}
		}
	}
	virtual void startDocument()
	{
	}

	XMLNode getDocument() const
	{
		return m_topNode;
	}

private :
	
	XMLNode m_currentNode;
	XMLNode m_topNode;
	XMLNodeArray m_nodeArray;

};

class DOMErrorHandler : public XMLParserSAX::SAXErrorHandler
{
public:
	virtual void warning(const XMLParseException &exception)
	{
	}
	virtual void error(const XMLParseException &exception)
	{
		OW_THROW(DOMException, Format("Error in XML: "
					"line %1, Message: %3",
					exception.getLine(),
					exception.getMessage()).c_str());
	}
	virtual void fatalError(const XMLParseException &exception)
	{
		OW_THROW(DOMException, Format("Fatal error in XML: "
					"line %1, Message: %2",
					exception.getLine(),
					exception.getMessage()).c_str());
	}
};

} // end unnamed namespace

namespace XMLParserDOM
{

XMLNode 
parse(const String& xmlData)
{
	DOMDocumentBuilder docHandler;
	DOMErrorHandler errHandler;
	XMLParserSAX::parse(xmlData, docHandler, errHandler);
	XMLNode rv = docHandler.getDocument();
	if (!rv)
	{
		OW_THROW(DOMException, "No document found");
	}
	return rv;
}

XMLNode
parse(istream& data)
{
	DOMDocumentBuilder docHandler;
	DOMErrorHandler errHandler;
	XMLParserSAX::parse(data, docHandler, errHandler);
	XMLNode rv = docHandler.getDocument();
	if (!rv)
	{
		OW_THROW(DOMException, "No document found");
	}
	return rv;
}

} // end namespace XMLParserDOM
} // end namespace OW_NAMESPACE


