/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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

#ifndef OW_XMLPARSE_EXCEPTION_HPP_INCLUDE_GUARD_
#define OW_XMLPARSE_EXCEPTION_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_Exception.hpp"

namespace OW_NAMESPACE
{

class OW_XML_API XMLParseException : public Exception
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
				const char* file,
				unsigned int line,
				Code code,
				const char* msg,
				unsigned int xmlline = 0);
		XMLParseException::Code getCode() const
		{
			return _code;
		}
		virtual XMLParseException* clone() const throw();
		virtual ~XMLParseException() throw();
		virtual const char* type() const;

	private:
		Code _code;
};

#define OW_THROWXML(code, message) throw XMLParseException(__FILE__, __LINE__, (code), (message));

#define OW_THROWXMLLINE(code, line) throw XMLParseException(__FILE__, __LINE__, (code), 0, (line));

#define OW_THROWXMLLINEMSG(code, line, message) throw XMLParseException(__FILE__, __LINE__, (code), (message), (line));

} // end namespace OW_NAMESPACE


#endif


