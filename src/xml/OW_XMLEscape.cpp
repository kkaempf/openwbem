/* Generated by re2c 0.5 on Wed Jul 25 22:09:44 2001 */
#line 1 "OW_XMLEscape.re"
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

/* a simple lexical scanner to escape xml */
#include "OW_config.h"
#include "OW_XMLEscape.hpp"
#include "OW_StringBuffer.hpp"



OW_String OW_XMLEscape(const OW_String& escapedText)
{
	OW_StringBuffer rval(escapedText.length());

	const char* begin = escapedText.c_str();
	const char* end = escapedText.c_str() + escapedText.length();

	#define YYCTYPE char
	#define YYCURSOR        begin
	#define YYLIMIT         end
	#define YYFILL(n)

start:
	{
	YYCTYPE yych;
	goto yy0;
	++YYCURSOR;
yy0:
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	switch(yych){
	case '\000':	goto yy20;
	case '\t':	goto yy16;
	case '\n':	goto yy14;
	case '\r':	goto yy12;
	case '"':	goto yy8;
	case '&':	goto yy6;
	case '\'':	goto yy10;
	case '<':	goto yy4;
	case '>':	goto yy2;
	default:	goto yy18;
	}
yy2:	yych = *++YYCURSOR;
#line 57
	{ rval += "&gt;"; goto start; }
yy4:	yych = *++YYCURSOR;
#line 58
	{ rval += "&lt;"; goto start; }
yy6:	yych = *++YYCURSOR;
#line 59
	{ rval += "&amp;"; goto start; }
yy8:	yych = *++YYCURSOR;
#line 60
	{ rval += "&quot;"; goto start; }
yy10:	yych = *++YYCURSOR;
#line 61
	{ rval += "&apos;"; goto start; }
yy12:	yych = *++YYCURSOR;
#line 62
	{ rval += "&#13;"; goto start; }
yy14:	yych = *++YYCURSOR;
#line 63
	{ rval += "&#10;"; goto start; }
yy16:	yych = *++YYCURSOR;
#line 64
	{ rval += "&#9;"; goto start; }
yy18:	yych = *++YYCURSOR;
#line 65
	{ rval += *(YYCURSOR-1); goto start; }
yy20:	
#line 66
	{ return rval.toString(); }
}
#line 67


	return rval.toString();
}
