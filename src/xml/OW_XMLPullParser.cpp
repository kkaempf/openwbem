/*******************************************************************************
* Copyright (C) 2001-3 Vintela, Inc. All rights reserved.
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
#include "OW_XMLPullParser.hpp"
#include "OW_Format.hpp"
#include "OW_Assertion.hpp"
#include "OW_XMLUnescape.hpp"
#include "OW_XMLParseException.hpp"
#include <algorithm> // for std::lower_bound

namespace OpenWBEM
{

///////////////////////////////////////////////////////////////////////////////
void
XMLPullParser::prime()
{
	if (!(m_good = m_parser.next(m_curTok)))
	{
		OW_THROWXML(XMLParseException::SEMANTIC_ERROR, "Empty XML");
	}
	if (m_curTok.type == XMLToken::XML_DECLARATION)
	{
		if (!(m_good = m_parser.next(m_curTok)))
		{
			OW_THROWXML(XMLParseException::SEMANTIC_ERROR, "Empty XML");
		}
		skipData();
	}
	if (m_curTok.type == XMLToken::DOCTYPE)
	{
		if (!(m_good = m_parser.next(m_curTok)))
		{
			OW_THROWXML(XMLParseException::SEMANTIC_ERROR, "Empty XML");
		}
		skipData();
	}
}
///////////////////////////////////////////////////////////////////////////////
XMLPullParser::XMLPullParser(const String& str)
	: m_ptfs(new TempFileStream())
	, m_parser()
	, m_curTok()
	, m_good(true)
{
	*m_ptfs << str;
	m_parser.setInput(*m_ptfs);
	prime();
}
///////////////////////////////////////////////////////////////////////////////
XMLPullParser::XMLPullParser(std::istream& istr)
	: m_ptfs()
	, m_parser(istr)
	, m_curTok()
	, m_good(true)
{
	prime();
}
///////////////////////////////////////////////////////////////////////////////
XMLPullParser::XMLPullParser()
	: m_good(false)
{
}

///////////////////////////////////////////////////////////////////////////////
String
XMLPullParser::getAttribute(const char* const attrId, bool throwIfError)
{
	OW_ASSERT(m_curTok.type == XMLToken::START_TAG);
	for (unsigned i = 0; i < m_curTok.attributeCount; i++)
	{
		XMLToken::Attribute& attr = m_curTok.attributes[i];
		// Should this be case insensentive? NO
		if (attr.name.equals(attrId))
		{
			return XMLUnescape(attr.value.c_str(), attr.value.length());
		}
	}
	if (throwIfError)
	{
		OW_THROWXML(XMLParseException::BAD_ATTRIBUTE_NAME,
					Format("Failed to find "
						"attribute: %1 in node: %2", attrId, m_curTok.text).c_str() );
	}
	return String();
}
//////////////////////////////////////////////////////////////////////////////
void
XMLPullParser::mustGetChild()
{
	if (!m_good)
	{
		m_good = false;
		OW_THROWXML(XMLParseException::SEMANTIC_ERROR,
			Format("XMLPullParser::mustGetChild() failed.  parser = %1",
				*this).c_str());
	}
	getChild();
	if (!m_good)
	{
		m_good = false;
		OW_THROWXML(XMLParseException::SEMANTIC_ERROR,
			Format("XMLPullParser::mustGetChild() failed.  parser = %1",
				*this).c_str());
	}
}
//////////////////////////////////////////////////////////////////////////////
void
XMLPullParser::getChild()
{
	if (!m_good)
	{
		return;
	}
	for(;;)
	{
		nextToken();
		if (!m_good)
		{
			return;
		}
		switch (m_curTok.type)
		{
			case XMLToken::END_TAG: // hit the end, no children
				m_good = false;
			case XMLToken::START_TAG: // valid token for a child
				return;
			default:
				break;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void
XMLPullParser::getNextTag(bool throwIfError)
{
	nextToken();
	skipData();
	if (!m_good && throwIfError)
	{
		OW_THROWXML(XMLParseException::SEMANTIC_ERROR,
			Format("XMLPullParser::getNext() failed.  parser = %1",
				*this).c_str());
	}
}
//////////////////////////////////////////////////////////////////////////////
void
XMLPullParser::getNext(bool throwIfError)
{
	nextToken();
	if (!m_good && throwIfError)
	{
		OW_THROWXML(XMLParseException::SEMANTIC_ERROR,
			Format("XMLPullParser::getNext() failed.  parser = %1",
				*this).c_str());
	}
}
//////////////////////////////////////////////////////////////////////////////
void
XMLPullParser::mustGetEndTag()
{
	skipData();
	if (m_curTok.type != XMLToken::END_TAG)
	{
		OW_THROWXML(XMLParseException::SEMANTIC_ERROR,
			Format("XMLPullParser::mustGetEndTag() failed.  parser = %1",
				*this).c_str());
	}
	getNext();
	skipData();
}
//////////////////////////////////////////////////////////////////////////////
String
XMLPullParser::getName() const
{
	OW_ASSERT(m_curTok.type == XMLToken::START_TAG || m_curTok.type == XMLToken::END_TAG);
	return XMLUnescape(m_curTok.text.c_str(), m_curTok.text.length());
}
//////////////////////////////////////////////////////////////////////////////
String
XMLPullParser::getData() const
{
	OW_ASSERT(m_curTok.type == XMLToken::CONTENT || m_curTok.type == XMLToken::CDATA);
	return XMLUnescape(m_curTok.text.c_str(), m_curTok.text.length());
}
//////////////////////////////////////////////////////////////////////////////
bool
XMLPullParser::isData() const
{
	return m_curTok.type == XMLToken::CONTENT || m_curTok.type == XMLToken::CDATA;
}
///////////////////////////////////////////////////////////////////////////////
void
XMLPullParser::nextToken()
{
	do
	{
		m_good = m_parser.next(m_curTok);
	} while (m_curTok.type == XMLToken::COMMENT && m_good);
}
//////////////////////////////////////////////////////////////////////////////
void
XMLPullParser::skipData()
{
	while (isData() && m_good)
	{
		nextToken();
	}
}
//////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& ostr, const XMLPullParser& p)
{
	ostr << "m_good = " << p.m_good << '\n';
	switch (p.m_curTok.type)
	{
		case XMLToken::INVALID:
			ostr << "*INVALID*\n";
			break;
		case XMLToken::XML_DECLARATION:
			ostr << "<xml>\n";
			break;
		case XMLToken::START_TAG:
			ostr << '<' << p.m_curTok.text << ' ';
			for (unsigned int x = 0; x < p.m_curTok.attributeCount; ++x)
			{
				ostr << p.m_curTok.attributes[x].name << "=\"" <<
					p.m_curTok.attributes[x].value << "\" ";
			}
			ostr << ">\n";
			break;
		case XMLToken::END_TAG:
			ostr << "</" << p.m_curTok.text << ">\n";
			break;
		case XMLToken::COMMENT:
			ostr << "<--" << p.m_curTok.text << "-->\n";
			break;
		case XMLToken::CDATA:
			ostr << "<CDATA[[" << p.m_curTok.text << "]]>\n";
			break;
		case XMLToken::DOCTYPE:
			ostr << "<DOCTYPE>\n";
			break;
		case XMLToken::CONTENT:
			ostr << "CONTENT: " << p.m_curTok.text << '\n';
			break;
		default:
			ostr << "Unknown token type\n";
	}
	return ostr;
}

} // end namespace OpenWBEM

