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

#ifndef OW_XMLPARSERSAX_HPP_
#define OW_XMLPARSERSAX_HPP_

#include "OW_config.h"
#include "OW_Exception.hpp"
#include "OW_String.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_IstreamBufIterator.hpp"


#ifdef OW_NEW
#undef new
#endif

#include <stack>
#include <iosfwd>

#ifdef OW_NEW
#define new OW_NEW
#endif

class OW_XMLParseException : public OW_Exception
{
	public:

		enum Code
		{
			BAD_START_TAG = 1,
			BAD_END_TAG,
			BAD_ATTRIBUTE_NAME,
			EXPECTED_EQUAL_SIGN,
			BAD_ATTRIBUTE_VALUE,
			MINUS_MINUS_IN_COMMENT,
			UNTERMINATED_COMMENT,
			UNTERMINATED_CDATA,
			UNTERMINATED_DOCTYPE,
			TOO_MANY_ATTRIBUTES,
			MALFORMED_REFERENCE,
			EXPECTED_COMMENT_OR_CDATA,
			START_END_MISMATCH,
			UNCLOSED_TAGS,
			MULTIPLE_ROOTS,
			VALIDATION_ERROR,
			SEMANTIC_ERROR
		};

		OW_XMLParseException(
				Code code,
				unsigned int lineNumber,
				const char* message);

		OW_XMLParseException(
				Code code,
				unsigned int lineNumber);

		OW_XMLParseException::Code getCode() const
		{
			return _code;
		}

	private:

		Code _code;
};

class OW_XMLParseValidationError : public OW_XMLParseException
{
	public:

		OW_XMLParseValidationError(unsigned int lineNumber, const char* message);
};

class OW_XMLParseSemanticError : public OW_XMLParseException
{
	public:

		OW_XMLParseSemanticError(unsigned int lineNumber, const char* message);
};

struct OW_XMLToken
{
	OW_XMLToken() : type(INVALID), text(8096), attributeCount(0)
	{}

	enum XMLType
	{
		INVALID,
		XML_DECLARATION,
		START_TAG,
		EMPTY_TAG,
		END_TAG,
		COMMENT,
		CDATA,
		DOCTYPE,
		CONTENT
	};

	struct Attribute
	{
		Attribute(): name(64), value(512)
		{
		}

		OW_StringBuffer name;
		OW_StringBuffer value;
	};

	enum { MAX_ATTRIBUTES = 10 };

	XMLType type;
	OW_StringBuffer text;
	Attribute attributes[MAX_ATTRIBUTES];
	unsigned int attributeCount;

};

class  OW_XMLParserSax
{
	public:

		OW_XMLParserSax(std::istream& input) : _line(1), _current(), _foundRoot(false)
		{
			setInput(input);
		}

		OW_XMLParserSax(): _line(1), _current(), _foundRoot(false)
		{
		}

		~OW_XMLParserSax()
		{
		}

		void setInput(std::istream& input)
		{
			_current = OW_IstreamBufIterator(input);
		}

		OW_Bool next(OW_XMLToken& entry);

		unsigned int getLine() const
		{
			return _line;
		}

	private:

		void _skipWhitespace();

		OW_Bool _getElementName(OW_XMLToken& entry);

		OW_Bool _getOpenElementName(OW_XMLToken& entry, OW_Bool& openCloseElement);

		void _getAttributeNameAndEqual(OW_XMLToken::Attribute& att);

		void _getAttributeValue(OW_XMLToken::Attribute& att);

		void _getComment();

		void _getCData(OW_XMLToken& entry);

		void _getDocType();

		void _getContent(OW_XMLToken& entry);

		void _getElement(OW_XMLToken& entry);

		unsigned int _line;
		OW_IstreamBufIterator _current;
		char _restoreChar;

		// used to verify elements' begin and end tags match.
		std::stack<OW_String> _stack;
		OW_Bool _foundRoot;
};


#endif

