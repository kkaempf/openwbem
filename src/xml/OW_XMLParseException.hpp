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
#ifndef OW_XMLPARSE_EXCEPTION_HPP_INCLUDE_GUARD_
#define OW_XMLPARSE_EXCEPTION_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Exception.hpp"

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

} // end namespace OpenWBEM


#endif


