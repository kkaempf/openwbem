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
#include <cctype>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "OW_XMLParser.hpp"
#include "OW_Format.hpp"

////////////////////////////////////////////////////////////////////////////////
//
// OW_XMLParseException
//
////////////////////////////////////////////////////////////////////////////////

static const char* _xmlMessages[] =
{
	"Bad opening element",
	"Bad closing element",
	"Bad attribute name",
	"Exepected equal sign",
	"Bad attribute value",
	"A \"--\" sequence found within comment",
	"Unterminated comment",
	"Unterminated CDATA block",
	"Unterminated DOCTYPE",
	"Too many attributes: parser only handles 10",
	"Malformed reference",
	"Expected a comment or CDATA following \"<!\" sequence",
	"Closing element does not match opening element",
	"One or more tags are still open",
	"More than one root element was encountered",
	"Validation error",
	"Semantic error"
};


OW_XMLParseException::OW_XMLParseException(
		OW_XMLParseException::Code code,
		unsigned int lineNumber)
: OW_Exception(format("%1: on line %2", _xmlMessages[code - 1], lineNumber).c_str())
{

}

OW_XMLParseException::OW_XMLParseException(
		OW_XMLParseException::Code code,
		unsigned int lineNumber,
		const char* message)
: OW_Exception(format("%1: on line %2: %3", _xmlMessages[code - 1], lineNumber,
	message).c_str())
{

}

////////////////////////////////////////////////////////////////////////////////
//
// OW_XMLParseValidationError
//
////////////////////////////////////////////////////////////////////////////////

OW_XMLParseValidationError::OW_XMLParseValidationError(
		unsigned int lineNumber,
		const char* message)
: OW_XMLParseException(OW_XMLParseException::VALIDATION_ERROR, lineNumber, message)
{

}

////////////////////////////////////////////////////////////////////////////////
//
// OW_XMLParseSemanticError
//
////////////////////////////////////////////////////////////////////////////////

OW_XMLParseSemanticError::OW_XMLParseSemanticError(
		unsigned int lineNumber,
		const char* message)
: OW_XMLParseException(OW_XMLParseException::SEMANTIC_ERROR, lineNumber, message)
{

}

////////////////////////////////////////////////////////////////////////////////
//
// OW_XMLParser
//
////////////////////////////////////////////////////////////////////////////////
OW_Bool OW_XMLParser::next(OW_XMLToken& entry)
{
	OW_IstreamBufIterator iterEOF;
	if (_current == iterEOF || *_current == 0)
	{
		if (!_stack.empty())
		{
			throw OW_XMLParseException(OW_XMLParseException::UNCLOSED_TAGS, _line);
		}

		return false;
	}

	// Either a "<...>" or content begins next:

	if (*_current == '<')
	{
		// Skip over any whitespace:

		_skipWhitespace();

		_current++;
		_getElement(entry);

		if (entry.type == OW_XMLToken::START_TAG)
		{
			if (_stack.empty() && _foundRoot)
				throw OW_XMLParseException(OW_XMLParseException::MULTIPLE_ROOTS, _line);

			_foundRoot = true;
			_stack.push(entry.text.toString());
		}
		else if (entry.type == OW_XMLToken::END_TAG)
		{
			if (_stack.empty())
				throw OW_XMLParseException(OW_XMLParseException::START_END_MISMATCH, _line);

			if (_stack.top() != entry.text.toString())
				throw OW_XMLParseException(OW_XMLParseException::START_END_MISMATCH, _line);

			_stack.pop();
		}

		return true;
	}
	else
	{
		entry.type = OW_XMLToken::CONTENT;
		_getContent(entry);

		return true;
	}
}

/*
void OW_XMLParser::putBack(OW_XMLToken& entry)
{
	_putBackStack.push(entry);
}
*/

void OW_XMLParser::_skipWhitespace()
{
	while (*_current && isspace(*_current))
	{
		if (*_current == '\n')
			++_line;

		++_current;
	}
}

OW_Bool OW_XMLParser::_getElementName(OW_XMLToken& entry)
{
	if (!isalpha(*_current) && *_current != '_')
		throw OW_XMLParseException(OW_XMLParseException::BAD_START_TAG, _line);

	entry.text.reset();
	while (*_current &&
			(isalnum(*_current) || *_current == '_' || *_current == '-' ||
			 *_current == ':' || *_current == '.'))
	{
		entry.text += *_current++;
	}

	// The next character might be a space:

	if (isspace(*_current))
	{
		_skipWhitespace();
	}

	if (*_current == '>')
	{
		++_current;
		return true;
	}

	return false;
}

OW_Bool OW_XMLParser::_getOpenElementName(OW_XMLToken& entry, OW_Bool& openCloseElement)
{
	openCloseElement = false;

	if (!isalpha(*_current) && *_current != '_')
		throw OW_XMLParseException(OW_XMLParseException::BAD_START_TAG, _line);

	entry.text.reset();
	while (*_current &&
			(isalnum(*_current) || *_current == '_' || *_current == '-' ||
			 *_current == ':' || *_current == '.'))
	{
		entry.text += *_current++;
	}

	// The next character must be a space:

	if (isspace(*_current))
	{
		_skipWhitespace();
	}

	if (*_current == '>')
	{
		++_current;
		return true;
	}

	if (*_current == '/')
	{
		++_current;
		if (*_current == '>')
		{
			openCloseElement = true;
			++_current;
			return true;
		}

	}

	return false;
}

void OW_XMLParser::_getAttributeNameAndEqual(OW_XMLToken::Attribute& att)
{
	if (!isalpha(*_current) && *_current != '_')
		throw OW_XMLParseException(OW_XMLParseException::BAD_ATTRIBUTE_NAME,
			_line, format("Expected alpha or _; got %1", *_current).c_str());

	att.name.reset();
	while (*_current &&
			(isalnum(*_current) || *_current == '_' || *_current == '-' ||
			 *_current == ':' || *_current == '.'))
	{
		att.name += *_current++;
	}

	_skipWhitespace();

	if (*_current != '=')
		throw OW_XMLParseException(OW_XMLParseException::BAD_ATTRIBUTE_NAME,
			_line, format("Expected =; got %1", *_current).c_str());

	_current++;

	_skipWhitespace();

}

void OW_XMLParser::_getAttributeValue(OW_XMLToken::Attribute& att)
{
	// ATTN-B: handle values contained in semiquotes:

	if (*_current != '"' && *_current != '\'')
		throw OW_XMLParseException(OW_XMLParseException::BAD_ATTRIBUTE_VALUE,
			_line, format("Expecting \" or '; got %1", *_current).c_str());

	char startChar = *_current++;

	att.value.reset();
	while (*_current && *_current != startChar)
	{
		att.value += *_current++;
	}
		

	if (*_current != startChar)
		throw OW_XMLParseException(OW_XMLParseException::BAD_ATTRIBUTE_VALUE,
			_line, format("Expecting %1; Got %2", startChar, (int)*_current).c_str());

	++_current;
}

void OW_XMLParser::_getComment()
{
	// Now p points to first non-whitespace character beyond "<--" sequence:

	for (; *_current; _current++)
	{
		if (*_current == '-')
		{
			++_current;
			if (*_current == '-')
			{
				++_current;
				if (*_current == '>')
				{
					++_current;
					return;
				}
				else
				{
					throw OW_XMLParseException(
							OW_XMLParseException::MINUS_MINUS_IN_COMMENT, _line);
				}
			}
		}
	}

	// If it got this far, then the comment is unterminated:

	throw OW_XMLParseException(OW_XMLParseException::UNTERMINATED_COMMENT, _line);
}

void OW_XMLParser::_getCData(OW_XMLToken& entry)
{
	// At this point _current points one past "<![CDATA[" sequence:

	entry.text.reset();
	for (; *_current; _current++)
	{
		if (*_current == ']')
		{
			++_current;
			if (*_current == ']')
			{
				++_current;
				if (*_current == '>')
				{
					++_current;
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

		if (*_current == '\n')
			++_line;

		entry.text += *_current++;

	}

	// If it got this far, then the cdata is unterminated:

	throw OW_XMLParseException(OW_XMLParseException::UNTERMINATED_CDATA, _line);
}

void OW_XMLParser::_getDocType()
{
	// Just ignore the DOCTYPE command for now:

	for (; *_current && *_current != '>'; ++_current)
	{
		if (*_current == '\n')
			++_line;
	}

	if (*_current != '>')
		throw OW_XMLParseException(OW_XMLParseException::UNTERMINATED_DOCTYPE, _line);

	_current++;
}

void OW_XMLParser::_getContent(OW_XMLToken& entry)
{
	entry.text.reset();
	while (*_current && *_current != '<')
	{
		if (*_current == '\n')
			++_line;

		entry.text += *_current++;
	}
}

void OW_XMLParser::_getElement(OW_XMLToken& entry)
{
	entry.attributeCount = 0;
	entry.text.reset();

	//--------------------------------------------------------------------------
	// Get the element name (expect one of these: '?', '!', [A-Za-z_])
	//--------------------------------------------------------------------------

	if (*_current == '?')
	{
		entry.type = OW_XMLToken::XML_DECLARATION;
		++_current;

		if (_getElementName(entry))
			return;
	}
	else if (*_current == '!')
	{
		_current++;

		// Expect a comment or CDATA:

		if (*_current == '-')
		{
			++_current;
			if (*_current == '-')
			{
				++_current;
				entry.type = OW_XMLToken::COMMENT;
				_getComment();
				return;
			}
		}
		else if (*_current == '[')
		{
			char string[] = "CDATA[";
			char *curChar = string;
			while (*curChar)
			{
				if (*curChar++ != *_current++)
					throw(OW_XMLParseException(OW_XMLParseException::EXPECTED_COMMENT_OR_CDATA, _line));
			}
			entry.type = OW_XMLToken::CDATA;
			_getCData(entry);
			return;
		}
		else if (*_current == 'D')
		{
			char string[] = "OCTYPE";
			char *curChar = string;
			while (*curChar)
			{
				if (*curChar++ != *_current++)
					throw(OW_XMLParseException(OW_XMLParseException::EXPECTED_COMMENT_OR_CDATA, _line));
			}
			entry.type = OW_XMLToken::DOCTYPE;
			_getDocType();
			return;
		}
		throw(OW_XMLParseException(OW_XMLParseException::EXPECTED_COMMENT_OR_CDATA, _line));
	}
	else if (*_current == '/')
	{
		entry.type = OW_XMLToken::END_TAG;
		++_current;

		if (!_getElementName(entry))
			throw(OW_XMLParseException(OW_XMLParseException::BAD_END_TAG, _line));

		return;
	}
	else if (isalpha(*_current) || *_current == '_')
	{
		entry.type = OW_XMLToken::START_TAG;

		OW_Bool openCloseElement;

		if (_getOpenElementName(entry, openCloseElement))
		{
			if (openCloseElement)
				entry.type = OW_XMLToken::EMPTY_TAG;
			return;
		}
	}
	else
		throw OW_XMLParseException(OW_XMLParseException::BAD_START_TAG, _line);

	//--------------------------------------------------------------------------
	// Grab all the attributes:
	//--------------------------------------------------------------------------

	for (;;)
	{
		_skipWhitespace();
		if (entry.type == OW_XMLToken::XML_DECLARATION)
		{
			if (*_current == '?')
			{
				++_current;
				if (*_current == '>')
				{
					++_current;
					return;
				}
				else
				{
					throw OW_XMLParseException(
						OW_XMLParseException::BAD_ATTRIBUTE_VALUE, _line,
						format("Expecting >; Got %1", *_current).c_str());
				}
			}
		}
		else if (entry.type == OW_XMLToken::START_TAG && *_current == '/')
		{
			++_current;
			if (*_current =='>')
			{
				entry.type = OW_XMLToken::EMPTY_TAG;
				++_current;
				return;
			}
			else
			{
				throw OW_XMLParseException(OW_XMLParseException::BAD_ATTRIBUTE_VALUE,
					_line, format("Expecting >; Got %1", *_current).c_str());
			}
		}
		else if (*_current == '>')
		{
			++_current;
			return;
		}

		++entry.attributeCount;
		OW_XMLToken::Attribute& attr = entry.attributes[entry.attributeCount - 1];
		_getAttributeNameAndEqual(attr);

		_getAttributeValue(attr);

		if (entry.attributeCount == OW_XMLToken::MAX_ATTRIBUTES)
			throw OW_XMLParseException(OW_XMLParseException::TOO_MANY_ATTRIBUTES, _line);

	}
}


