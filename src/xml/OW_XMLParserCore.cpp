/******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#include "OW_XMLParserCore.hpp"
#include "OW_Format.hpp"
#include "OW_XMLParseException.hpp"

#include <cctype>

namespace OpenWBEM
{

// Note that we don't use the OW_THROW macro in this file to throw
// an XMLParseException.  This is because it needs extra information not
// available with OW_THROW.

////////////////////////////////////////////////////////////////////////////////
//
// XMLParserCore
//
////////////////////////////////////////////////////////////////////////////////
bool XMLParserCore::next(XMLToken& entry)
{
	IstreamBufIterator iterEOF;
	if (m_current == iterEOF || *m_current == 0)
	{
		if (!m_stack.empty())
		{
			OW_THROWXMLLINE(XMLParseException::UNCLOSED_TAGS, m_line);
		}
		return false;
	}
	// if the last tag was a <.../> then set the next token to END_TAG so that
	// the caller doesn't need to worry about <.../>, it will look like
	// <...></...>
	if (m_tagIsEmpty)
	{
		m_tagIsEmpty = false;
		entry.type = XMLToken::END_TAG;
		entry.attributeCount = 0;
		entry.text.reset();
		return true;
	}
	// Either a "<...>" or content begins next:
	if (*m_current == '<')
	{
		// Skip over any whitespace:
		skipWhitespace();
		m_current++;
		getElement(entry);
		if (entry.type == XMLToken::START_TAG)
		{
			if (m_stack.empty() && m_foundRoot)
				OW_THROWXMLLINE(XMLParseException::MULTIPLE_ROOTS, m_line);
			m_foundRoot = true;
			if (!m_tagIsEmpty)
			{
				m_stack.push(entry.text.toString());
			}
		}
		else if (entry.type == XMLToken::END_TAG)
		{
			if (m_stack.empty())
				OW_THROWXMLLINE(XMLParseException::START_END_MISMATCH, m_line);
			if (m_stack.top() != entry.text.toString())
				OW_THROWXMLLINE(XMLParseException::START_END_MISMATCH, m_line);
			m_stack.pop();
		}
		return true;
	}
	else
	{
		entry.type = XMLToken::CONTENT;
		//bool isSpaces;
		//getContent(entry, isSpaces);
		getContent(entry);
		//if (isSpaces)
		//{
			// content is entirely white space, so just skip it.
		//	return next(entry);
		//}
		return true;
	}
}
/*
void XMLParserCore::putBack(XMLToken& entry)
{
	_putBackStack.push(entry);
}
*/
void XMLParserCore::skipWhitespace()
{
	while (isspace(*m_current))
	{
		if (*m_current == '\n')
			++m_line;
		++m_current;
	}
}

inline bool isNameChar(char c)
{
	return isalnum(c) || c == '_' || c == '-' ||
			 c == ':' || c == '.';
}

bool XMLParserCore::getElementName(XMLToken& entry)
{
	if (!isalpha(*m_current) && *m_current != '_')
		OW_THROWXMLLINE(XMLParseException::BAD_START_TAG, m_line);
	entry.text.reset();
	while (isNameChar(*m_current))
	{
		entry.text += *m_current++;
	}
	// The next character might be a space:
	skipWhitespace();
	
	if (*m_current == '>')
	{
		++m_current;
		return true;
	}
	return false;
}

bool XMLParserCore::getOpenElementName(XMLToken& entry, bool& openCloseElement)
{
	openCloseElement = false;
	if (getElementName(entry))
	{
		return true;
	}
	if (*m_current == '/')
	{
		++m_current;
		if (*m_current == '>')
		{
			openCloseElement = true;
			++m_current;
			return true;
		}
	}
	return false;
}
void XMLParserCore::getAttributeNameAndEqual(XMLToken::Attribute& att)
{
	if (!isalpha(*m_current) && *m_current != '_')
		OW_THROWXMLLINEMSG(XMLParseException::BAD_ATTRIBUTE_NAME,
			m_line, format("Expected alpha or _; got %1", *m_current).c_str());
	att.name.reset();
	while (isalnum(*m_current) || *m_current == '_' || *m_current == '-' ||
			 *m_current == ':' || *m_current == '.')
	{
		att.name += *m_current++;
	}
	skipWhitespace();
	if (*m_current != '=')
		OW_THROWXMLLINEMSG(XMLParseException::BAD_ATTRIBUTE_NAME,
			m_line, format("Expected =; got %1", *m_current).c_str());
	m_current++;
	skipWhitespace();
}
void XMLParserCore::getAttributeValue(XMLToken::Attribute& att)
{
	// ATTN-B: handle values contained in semiquotes:
	if (*m_current != '"' && *m_current != '\'')
		OW_THROWXMLLINEMSG(XMLParseException::BAD_ATTRIBUTE_VALUE,
			m_line, format("Expecting \" or '; got %1", *m_current).c_str());
	char startChar = *m_current++;
	att.value.reset();
	while (*m_current && *m_current != startChar)
	{
		att.value += *m_current++;
	}
		
	if (*m_current != startChar)
		OW_THROWXMLLINEMSG(XMLParseException::BAD_ATTRIBUTE_VALUE,
			m_line, format("Expecting %1; Got %2", startChar, static_cast<int>(*m_current)).c_str());
	++m_current;
}
void XMLParserCore::getComment()
{
	// Now p points to first non-whitespace character beyond "<--" sequence:
	for (; *m_current; m_current++)
	{
		if (*m_current == '-')
		{
			++m_current;
			if (*m_current == '-')
			{
				++m_current;
				if (*m_current == '>')
				{
					++m_current;
					return;
				}
				else
				{
					OW_THROWXMLLINE(
							XMLParseException::MINUS_MINUS_IN_COMMENT, m_line);
				}
			}
		}
	}
	// If it got this far, then the comment is unterminated:
	OW_THROWXMLLINE(XMLParseException::UNTERMINATED_COMMENT, m_line);
}
void XMLParserCore::getCData(XMLToken& entry)
{
	// At this point m_current points one past "<![CDATA[" sequence:
	entry.text.reset();
	for (; *m_current; m_current++)
	{
		if (*m_current == ']')
		{
			++m_current;
			if (*m_current == ']')
			{
				++m_current;
				if (*m_current == '>')
				{
					++m_current;
					return;
				}
				else
				{
					entry.text += ']';
					entry.text += ']';
				}
			}
			else
			{
				entry.text += ']';
			}
		}
		if (*m_current == '\n')
			++m_line;
		entry.text += *m_current;
	}
	// If it got this far, then the cdata is unterminated:
	OW_THROWXMLLINE(XMLParseException::UNTERMINATED_CDATA, m_line);
}
void XMLParserCore::getDocType()
{
	// Just ignore the DOCTYPE command for now:
	for (; *m_current && *m_current != '>'; ++m_current)
	{
		if (*m_current == '\n')
			++m_line;
	}
	if (*m_current != '>')
		OW_THROWXMLLINE(XMLParseException::UNTERMINATED_DOCTYPE, m_line);
	m_current++;
}
//void XMLParserCore::getContent(XMLToken& entry, bool& isWhiteSpace)
void XMLParserCore::getContent(XMLToken& entry)
{
	entry.text.reset();
	//isWhiteSpace = true;
	while (*m_current && *m_current != '<')
	{
		if (*m_current == '\n')
			++m_line;
		//isWhiteSpace &= isspace(*m_current);
		//if (isWhiteSpace)
		//{
		//	isWhiteSpace = isspace(*m_current);
		//}
		entry.text += *m_current++;
	}
}
void XMLParserCore::getElement(XMLToken& entry)
{
	entry.attributeCount = 0;
	entry.text.reset();
	//--------------------------------------------------------------------------
	// Get the element name (expect one of these: '?', '!', [A-Za-z_])
	//--------------------------------------------------------------------------
	if (*m_current == '?')
	{
		entry.type = XMLToken::XML_DECLARATION;
		++m_current;
		if (getElementName(entry))
			return;
	}
	else if (*m_current == '!')
	{
		m_current++;
		// Expect a comment or CDATA:
		if (*m_current == '-')
		{
			++m_current;
			if (*m_current == '-')
			{
				++m_current;
				entry.type = XMLToken::COMMENT;
				getComment();
				return;
			}
		}
		else if (*m_current == '[')
		{
			char string[] = "CDATA[";
			char *curChar = string;
			m_current++;
			while (*curChar)
			{
				if (*curChar++ != *m_current++)
					OW_THROWXMLLINE(XMLParseException::EXPECTED_COMMENT_OR_CDATA, m_line);
			}
			entry.type = XMLToken::CDATA;
			getCData(entry);
			return;
		}
		else if (*m_current == 'D')
		{
			char string[] = "OCTYPE";
			char *curChar = string;
			while (*curChar)
			{
				if (*curChar++ != *m_current++)
					OW_THROWXMLLINE(XMLParseException::EXPECTED_COMMENT_OR_CDATA, m_line);
			}
			entry.type = XMLToken::DOCTYPE;
			getDocType();
			return;
		}
		OW_THROWXMLLINE(XMLParseException::EXPECTED_COMMENT_OR_CDATA, m_line);
	}
	else if (*m_current == '/')
	{
		entry.type = XMLToken::END_TAG;
		++m_current;
		if (!getElementName(entry))
			OW_THROWXMLLINE(XMLParseException::BAD_END_TAG, m_line);
		return;
	}
	else if (isalpha(*m_current) || *m_current == '_')
	{
		entry.type = XMLToken::START_TAG;
		bool openCloseElement;
		if (getOpenElementName(entry, openCloseElement))
		{
			if (openCloseElement)
			{
				entry.type = XMLToken::START_TAG;
				m_tagIsEmpty = true;
			}
			return;
		}
	}
	else
		OW_THROWXMLLINE(XMLParseException::BAD_START_TAG, m_line);
	//--------------------------------------------------------------------------
	// Grab all the attributes:
	//--------------------------------------------------------------------------
	for (;;)
	{
		skipWhitespace();
		if (entry.type == XMLToken::XML_DECLARATION)
		{
			if (*m_current == '?')
			{
				++m_current;
				if (*m_current == '>')
				{
					++m_current;
					return;
				}
				else
				{
					OW_THROWXMLLINEMSG(
						XMLParseException::BAD_ATTRIBUTE_VALUE, m_line,
						format("Expecting >; Got %1", *m_current).c_str());
				}
			}
		}
		else if (entry.type == XMLToken::START_TAG && *m_current == '/')
		{
			++m_current;
			if (*m_current =='>')
			{
				entry.type = XMLToken::START_TAG;
				m_tagIsEmpty = true;
				++m_current;
				return;
			}
			else
			{
				OW_THROWXMLLINEMSG(XMLParseException::BAD_ATTRIBUTE_VALUE,
					m_line, format("Expecting >; Got %1", *m_current).c_str());
			}
		}
		else if (*m_current == '>')
		{
			++m_current;
			return;
		}
		++entry.attributeCount;
		XMLToken::Attribute& attr = entry.attributes[entry.attributeCount - 1];
		getAttributeNameAndEqual(attr);
		getAttributeValue(attr);
		if (entry.attributeCount == XMLToken::MAX_ATTRIBUTES)
			OW_THROWXMLLINE(XMLParseException::TOO_MANY_ATTRIBUTES, m_line);
	}
}

} // end namespace OpenWBEM

