/*******************************************************************************
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

/**
 * @author Dan Nuffer
 */


/* a simple lexical scanner to escape xml */
#include "OW_config.h"
#include "OW_XMLUnescape.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_XMLParseException.hpp"
#include "OW_Format.hpp"

#include <limits.h> // for CHAR_MAX
#include <stdlib.h> // for strtol

namespace OpenWBEM
{

String XMLUnescape(const String& escapedText)
{
	StringBuffer rval(escapedText.length());

	const char* begin = escapedText.c_str();
	const char* end = escapedText.c_str() + escapedText.length();
	const char* q;
	const char* thisTokStart = 0;

	#define YYCTYPE char
	#define YYCURSOR        begin
	#define YYLIMIT         end
	#define YYMARKER        q
	#define YYFILL(n)

start:
	/*!re2c
	END = [\000];
	any = [\001-\377];
	DIGIT = [0-9];

	"&gt;" { rval += ">"; thisTokStart = YYCURSOR; goto start; }
	"&lt;" { rval += "<"; thisTokStart = YYCURSOR; goto start; }
	"&amp;" { rval += "&"; thisTokStart = YYCURSOR; goto start; }
	"&quot;" { rval += "\""; thisTokStart = YYCURSOR; goto start; }
	"&apos;" { rval += "'"; thisTokStart = YYCURSOR; goto start; }
	"&#x" DIGIT+ ";"
	{
		long lval = strtol( thisTokStart + 3, NULL, 16 );
		if (lval > CHAR_MAX || lval < 0)
		{
			OW_THROWXML(XMLParseException::MALFORMED_REFERENCE, Format("XML escape code in unsupported range: %1", YYCURSOR - 1).c_str());
		}
		char val = lval;
		rval += val;
		thisTokStart = YYCURSOR; goto start;
	}
	"&#" DIGIT+ ";"
	{
		long lval = strtol( thisTokStart + 2, NULL, 10 );
		if (lval > CHAR_MAX || lval < 0)
		{
			OW_THROWXML(XMLParseException::MALFORMED_REFERENCE, Format("XML escape code in unsupported range: %1", YYCURSOR - 1).c_str());
		}
		char val = lval;
		rval += val;
		thisTokStart = YYCURSOR; goto start;
	}
	any { rval += *(YYCURSOR-1); thisTokStart = YYCURSOR; goto start; }
	END { return rval.toString(); }
	*/

	return rval.toString();
}

} // end namespace OpenWBEM
