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
*  - Neither the name of Center 7, Inc nor the names of its
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

/* a simple lexical scanner to escape xml */
#include "OW_config.h"
#include "OW_XMLEscape.hpp"
#include "OW_StringBuffer.hpp"

namespace OpenWBEM
{

String XMLEscape(const String& escapedText)
{
	StringBuffer rval(escapedText.length());

	const char* begin = escapedText.c_str();
	const char* q;

	#define YYCTYPE char
	#define YYCURSOR        begin
	#define YYLIMIT         begin
	#define YYMARKER        q
	#define YYFILL(n)

start:
	/*!re2c
	END = [\000];
	any = [\001-\377];

	">" { rval += "&gt;"; goto start; }
	"<" { rval += "&lt;"; goto start; }
	"&" { rval += "&amp;"; goto start; }
	"\"" { rval += "&quot;"; goto start; }
	"'" { rval += "&apos;"; goto start; }
	"\r" { rval += "&#13;"; goto start; }
	"\n" { rval += "&#10;"; goto start; }
	"\t" { rval += "&#9;"; goto start; }
	any { rval += *(YYCURSOR-1); goto start; }
	END { return rval.toString(); }
	*/

	return rval.toString();
}

} // end namespace OpenWBEM

