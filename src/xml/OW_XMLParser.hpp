/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef OW_XMLPARSER_HPP_
#define OW_XMLPARSER_HPP_
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

namespace OpenWBEM
{

class XMLParseException : public Exception
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
		XMLParseException(
				Code code,
				unsigned int lineNumber,
				const char* message);
		XMLParseException(
				Code code,
				unsigned int lineNumber);
		XMLParseException(
				const char* file,
				unsigned int line,
				const char* msg);
		XMLParseException::Code getCode() const
		{
			return _code;
		}
	private:
		Code _code;
};
class XMLParseValidationError : public XMLParseException
{
	public:
		XMLParseValidationError(unsigned int lineNumber, const char* message);
};
class XMLParseSemanticError : public XMLParseException
{
	public:
		XMLParseSemanticError(unsigned int lineNumber, const char* message);
};
struct XMLToken
{
	XMLToken() : type(INVALID), text(8096), attributeCount(0)
	{}
	enum XMLType
	{
		INVALID,
		XML_DECLARATION,
		START_TAG,
		//EMPTY_TAG,
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
		StringBuffer name;
		StringBuffer value;
	};
	enum { MAX_ATTRIBUTES = 10 };
	XMLType type;
	StringBuffer text;
	Attribute attributes[MAX_ATTRIBUTES];
	unsigned int attributeCount;
};
class  XMLParser
{
	public:
		XMLParser(std::istream& input) : _line(1), _current(), _foundRoot(false), _tagIsEmpty(false)
		{
			setInput(input);
		}
		XMLParser(): _line(1), _current(), _foundRoot(false), _tagIsEmpty(false)
		{
		}
		~XMLParser()
		{
		}
		void setInput(std::istream& input)
		{
			_current = IstreamBufIterator(input);
		}
		bool next(XMLToken& entry);
		unsigned int getLine() const
		{
			return _line;
		}
	private:
		void _skipWhitespace();
		bool _getElementName(XMLToken& entry);
		bool _getOpenElementName(XMLToken& entry, bool& openCloseElement);
		void _getAttributeNameAndEqual(XMLToken::Attribute& att);
		void _getAttributeValue(XMLToken::Attribute& att);
		void _getComment();
		void _getCData(XMLToken& entry);
		void _getDocType();
		void _getContent(XMLToken& entry);
		//void _getContent(XMLToken& entry, bool& isWhiteSpace);
		void _getElement(XMLToken& entry);
		unsigned int _line;
		IstreamBufIterator _current;
		char _restoreChar;
		// used to verify elements' begin and end tags match.
		std::stack<String> _stack;
		bool _foundRoot;
		bool _tagIsEmpty;
};

} // end namespace OpenWBEM

typedef OpenWBEM::XMLParseException OW_XMLParseException;
typedef OpenWBEM::XMLParseValidationError OW_XMLParseValidationError;
typedef OpenWBEM::XMLParseSemanticError OW_XMLParseSemanticError;
typedef OpenWBEM::XMLToken OW_XMLToken;
typedef OpenWBEM::XMLParser OW_XMLParser;

#endif
