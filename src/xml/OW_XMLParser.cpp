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
// OW_XMLParser.cpp
//
//
#include	<OW_config.h>
#include	<OW_StringBuffer.hpp>
#include	<OW_XMLAttribute.hpp>
#include	"OW_XMLParser.hpp"
#include "OW_XMLException.hpp"
#include "OW_Format.hpp"
#include "OW_XMLUnescape.hpp"
#include "OW_Assertion.hpp"
#include <fstream>

using std::ifstream;
using std::istream;

OW_XMLParser::OW_XMLParser(const OW_String& fileName) :
	m_currentNode(), m_topNode(), m_nodeArray(), m_SAXParser(),
	m_pIStream(NULL), m_fileName(fileName), m_isFile(true)
{
}

OW_XMLParser::OW_XMLParser(istream *data) :
	m_currentNode(), m_topNode(), m_nodeArray(), m_SAXParser(),
	m_pIStream(data), m_fileName(),m_isFile(false)
{
}

OW_XMLParser::OW_XMLParser() :
	m_currentNode(), m_topNode(), m_nodeArray(), m_SAXParser(),
	m_pIStream(NULL), m_fileName(),m_isFile(false)
{
}

OW_XMLParser::~OW_XMLParser()
{
}

OW_XMLNode
OW_XMLParser::parse(const OW_String& fileName)
{
	try
	{
		ifstream istr(fileName.c_str());
		doParse(istr);
	}

	catch (const OW_XMLParseException& toCatch)
	{
		// In this case, nothing is going to happen so m_topNode will be null
	}

	return m_topNode;
}

	OW_XMLNode
OW_XMLParser::parse(istream *data)
{
	try
	{
		doParse(*data);
	}

	catch (const OW_XMLParseException& toCatch)
	{
		// In this case, nothing is going to happen so m_topNode will be null
	}

	return m_topNode;
}

	OW_XMLNode
OW_XMLParser::parse()
{
	if(m_isFile)
	{
		return(parse(m_fileName));
	}
	else
	{
		if(m_pIStream != NULL)
		{
			return(parse(m_pIStream));
		}
	}

	return m_topNode;
}

	void
OW_XMLParser::endDocument()
{
}

	void
OW_XMLParser::endElement(const OW_StringBuffer& sb)
{
	(void)sb;
	// at the end of the element, we just need to pop a node
	// off the stack
	m_nodeArray.pop_back();
}

	void
OW_XMLParser::characters(const OW_StringBuffer& chars)
{
	if(m_nodeArray.size() > 0)
	{
		OW_XMLNode curNode = m_nodeArray[m_nodeArray.size() - 1];
		if(curNode != 0)
		{
			OW_String utxt = OW_XMLUnescape(chars.toString());
			curNode.appendText(utxt);
		}
	}
}

	void
OW_XMLParser::startDocument()
{
	// OK, this lets us know the document is starting
}

void
OW_XMLParser::startElement(const OW_XMLToken& entry)
{
	// Build up an OW_XMLAttrArray to hand into the XMLNode on creation
	OW_XMLAttrArray newAttrArray;
	unsigned int len = entry.attributeCount;
	for (unsigned int index = 0; index < len; index++)
	{
		OW_String nodeName = entry.attributes[index].name.toString();
		OW_String nodeValue = OW_XMLUnescape(entry.attributes[index].value.toString());

		OW_XMLAttribute newAttribute(nodeName, nodeValue);
		newAttrArray.append(newAttribute);
	}

	OW_XMLNode newNode(entry.text.toString(), newAttrArray);

	if(newNode)
	{
		if(!m_topNode)
		{
			m_topNode = newNode;
		}

		// If there is anything in the array, this is the child of that last guy
		if(m_nodeArray.size() > 0)
		{
			OW_XMLNode parent = m_nodeArray[m_nodeArray.size() - 1];
			parent.addChild(newNode);
		}

		m_nodeArray.push_back(newNode);
	}
}

void
OW_XMLParser::doParse(istream& istr)
{
	m_SAXParser.setInput(istr);

	OW_XMLToken entry;
	startDocument();
	try
	{
		while (m_SAXParser.next(entry))
		{
			switch(entry.type)
			{
				case OW_XMLToken::INVALID:
					break;
				case OW_XMLToken::XML_DECLARATION:
					break;
				case OW_XMLToken::START_TAG:
					startElement(entry);
					break;
				case OW_XMLToken::EMPTY_TAG:
					startElement(entry);
					endElement(entry.text);
					break;
				case OW_XMLToken::END_TAG:
					endElement(entry.text);
					break;
				case OW_XMLToken::COMMENT:
					break;
				case OW_XMLToken::CDATA:
					characters(entry.text);
					break;
				case OW_XMLToken::DOCTYPE:
					break;
				case OW_XMLToken::CONTENT:
					characters(entry.text);
					break;
				default:
					break;
			}
		}
	}
	catch (OW_XMLParseException& e)
	{
		fatalError(e);
	}

	endDocument();

}

	void
OW_XMLParser::warning(const OW_XMLParseException& /*exception*/)
{
	// TODO remove or change this later????
	//cerr << "\nWarning at file " << exception.getFile()
	//	<< ", line " << exception.getLine()
	//	<< "\n  Message: " << exception.getMessage() << endl;
}

	void
OW_XMLParser::error(const OW_XMLParseException& exception)
{
	OW_THROW(OW_XMLException, format("Error in XML: "
				"line %1, Message: %3",
				exception.getLine(),
				exception.getMessage()).c_str());
}

	void
OW_XMLParser::fatalError(const OW_XMLParseException& exception)
{
	OW_THROW(OW_XMLException, format("Fatal error in XML: "
				"line %1, Message: %2",
				exception.getLine(),
				exception.getMessage()).c_str());
}


