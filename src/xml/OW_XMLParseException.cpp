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

#include "OW_config.h"
#include "OW_XMLParseException.hpp"
#include "OW_ExceptionIds.hpp"
#include "OW_Format.hpp"

namespace OpenWBEM
{

////////////////////////////////////////////////////////////////////////////////
//
// XMLParseException
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
////////////////////////////////////////////////////////////////////////////////
XMLParseException::XMLParseException(
	const char* file,
	unsigned int line,
	Code code,
	const char* msg,
	unsigned int xmlline)
: Exception(ExceptionIds::XMLParseExceptionId, file, line, Format("Line %1: %2: %3", xmlline, _xmlMessages[code - 1],
	msg != 0 ? msg : "").c_str(), code)
{
}

////////////////////////////////////////////////////////////////////////////////
XMLParseException* 
XMLParseException::clone() const throw()
{
	return new(std::nothrow) XMLParseException(*this);
}

////////////////////////////////////////////////////////////////////////////////
XMLParseException::~XMLParseException() throw()
{
}

////////////////////////////////////////////////////////////////////////////////
const char* 
XMLParseException::type() const
{
	return "XMLParseException";
}


} // end namespace OpenWBEM

