/* Generated by re2c 0.5 on Wed Jul 25 22:09:49 2001 */
#line 1 "OW_XMLUnescape.re"
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

/* a simple lexical scanner to escape xml */
#include "OW_config.h"
#include "OW_XMLUnescape.hpp"
#include "OW_StringBuffer.hpp"
#include "OW_XMLParser.hpp" // for OW_XMLParseException
#include "OW_Format.hpp"

#include <limits.h> // for CHAR_MAX
#include <stdlib.h> // for strtol


OW_String OW_XMLUnescape(const char* escapedText, unsigned len)
{
	OW_StringBuffer rval(len * 2);

	const char* begin = escapedText;
	const char* end = escapedText + len;
	const char* q;
	const char* thisTokStart = 0;

	#define YYCTYPE char
	#define YYCURSOR        begin
	#define YYLIMIT         end
	#define YYMARKER        q
	#define YYFILL(n)

start:
	{
	YYCTYPE yych;
	unsigned int yyaccept;
	goto yy0;
	++YYCURSOR;
yy0:
	if((YYLIMIT - YYCURSOR) < 6) YYFILL(6);
	yych = *YYCURSOR;
	switch(yych){
	case '\000':	goto yy5;
	case '&':	goto yy2;
	default:	goto yy4;
	}
yy2:	yyaccept = 0;
	yych = *(YYMARKER = ++YYCURSOR);
	switch(yych){
	case '#':	goto yy7;
	case 'a':	goto yy9;
	case 'g':	goto yy12;
	case 'l':	goto yy11;
	case 'q':	goto yy10;
	default:	goto yy3;
	}
yy3:
#line 90
	{ rval += *(YYCURSOR-1); thisTokStart = YYCURSOR; goto start; }
yy4:	yych = *++YYCURSOR;
	goto yy3;
yy5:	
#line 91
	{ return rval.releaseString(); }
yy7:	yych = *++YYCURSOR;
	switch(yych){
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy34;
	case 'x':	goto yy33;
	default:	goto yy8;
	}
yy8:	YYCURSOR = YYMARKER;
	switch(yyaccept){
	case 0:	goto yy3;
	}
yy9:	yych = *++YYCURSOR;
	switch(yych){
	case 'm':	goto yy24;
	case 'p':	goto yy25;
	default:	goto yy8;
	}
yy10:	yych = *++YYCURSOR;
	switch(yych){
	case 'u':	goto yy19;
	default:	goto yy8;
	}
yy11:	yych = *++YYCURSOR;
	switch(yych){
	case 't':	goto yy16;
	default:	goto yy8;
	}
yy12:	yych = *++YYCURSOR;
	switch(yych){
	case 't':	goto yy13;
	default:	goto yy8;
	}
yy13:	yych = *++YYCURSOR;
	switch(yych){
	case ';':	goto yy14;
	default:	goto yy8;
	}
yy14:	yych = *++YYCURSOR;
#line 63
	{ rval += ">"; thisTokStart = YYCURSOR; goto start; }
yy16:	yych = *++YYCURSOR;
	switch(yych){
	case ';':	goto yy17;
	default:	goto yy8;
	}
yy17:	yych = *++YYCURSOR;
#line 64
	{ rval += "<"; thisTokStart = YYCURSOR; goto start; }
yy19:	yych = *++YYCURSOR;
	switch(yych){
	case 'o':	goto yy20;
	default:	goto yy8;
	}
yy20:	yych = *++YYCURSOR;
	switch(yych){
	case 't':	goto yy21;
	default:	goto yy8;
	}
yy21:	yych = *++YYCURSOR;
	switch(yych){
	case ';':	goto yy22;
	default:	goto yy8;
	}
yy22:	yych = *++YYCURSOR;
#line 66
	{ rval += "\""; thisTokStart = YYCURSOR; goto start; }
yy24:	yych = *++YYCURSOR;
	switch(yych){
	case 'p':	goto yy30;
	default:	goto yy8;
	}
yy25:	yych = *++YYCURSOR;
	switch(yych){
	case 'o':	goto yy26;
	default:	goto yy8;
	}
yy26:	yych = *++YYCURSOR;
	switch(yych){
	case 's':	goto yy27;
	default:	goto yy8;
	}
yy27:	yych = *++YYCURSOR;
	switch(yych){
	case ';':	goto yy28;
	default:	goto yy8;
	}
yy28:	yych = *++YYCURSOR;
#line 67
	{ rval += "'"; thisTokStart = YYCURSOR; goto start; }
yy30:	yych = *++YYCURSOR;
	switch(yych){
	case ';':	goto yy31;
	default:	goto yy8;
	}
yy31:	yych = *++YYCURSOR;
#line 65
	{ rval += "&"; thisTokStart = YYCURSOR; goto start; }
yy33:	yych = *++YYCURSOR;
	switch(yych){
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy38;
	default:	goto yy8;
	}
yy34:	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	switch(yych){
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy34;
	case ';':	goto yy36;
	default:	goto yy8;
	}
yy36:	yych = *++YYCURSOR;
#line 80
	{
		long lval = strtol( thisTokStart + 2, NULL, 10 );
		if (lval > CHAR_MAX)
		{
			OW_THROW(OW_XMLParseException, format("XML escape code in unsupported range: %1", YYCURSOR - 1).c_str());
		}
		char val = lval;
		rval += val;
		thisTokStart = YYCURSOR; goto start;
	}
yy38:	++YYCURSOR;
	if(YYLIMIT == YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	switch(yych){
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy38;
	case ';':	goto yy40;
	default:	goto yy8;
	}
yy40:	yych = *++YYCURSOR;
#line 69
	{
		long lval = strtol( thisTokStart + 3, NULL, 16 );
		if (lval > CHAR_MAX)
		{
			OW_THROW(OW_XMLParseException, format("XML escape code in unsupported range: %1", YYCURSOR - 1).c_str());
		}
		char val = lval;
		rval += val;
		thisTokStart = YYCURSOR; goto start;
	}
}
#line 92


	return rval.releaseString();
}
