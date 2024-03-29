/* Generated by re2c 0.13.5 on Mon Oct 26 19:19:51 2009 */
#line 1 "OW_XMLEscape.re"
/*******************************************************************************
* Copyright (C) 2001-2008 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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
 * @author Kevin Harris
 */


/* a simple lexical scanner to escape xml */
#include "OW_config.h"
#include "OW_XMLEscape.hpp"
#include "blocxx/StringBuffer.hpp"

namespace OW_NAMESPACE
{
using namespace blocxx;

String XMLEscape(const char* unescapedText, unsigned len)
{
	StringBuffer rval(len * 2);

	const char* begin = unescapedText;
	const char* end = unescapedText + len;
	const char* current = begin;
	// This is reset to be one-after the start for each "invalid" character
	// sequence (multi-byte or not).
	const char* marker = begin + 1;
 start:
	if( current >= end )
	{
		goto finish;
	}

	// 0000-0008 is invalid
	// 000b is invalid
	// 000e-001f is invalid
	// d800-dfff is invalid
	// fffe-ffff is invalid

	
#line 72 "OW_XMLEscape.cpp"
	{
		unsigned char yych;

		yych = (unsigned char)*current;
		switch (yych) {
		case 0x00:
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
		case 0x08:
		case '\v':
		case 0x0E:
		case 0x0F:
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13:
		case 0x14:
		case 0x15:
		case 0x16:
		case 0x17:
		case 0x18:
		case 0x19:
		case 0x1A:
		case 0x1B:
		case 0x1C:
		case 0x1D:
		case 0x1E:
		case 0x1F:	goto yy18;
		case '\t':	goto yy16;
		case '\n':	goto yy14;
		case '\r':	goto yy12;
		case '"':	goto yy8;
		case '&':	goto yy6;
		case '\'':	goto yy10;
		case '<':	goto yy4;
		case '>':	goto yy2;
		case 0xED:	goto yy20;
		case 0xEF:	goto yy22;
		default:	goto yy23;
		}
yy2:
		++current;
#line 82 "OW_XMLEscape.re"
		{ rval += "&gt;"; goto start; }
#line 122 "OW_XMLEscape.cpp"
yy4:
		++current;
#line 83 "OW_XMLEscape.re"
		{ rval += "&lt;"; goto start; }
#line 127 "OW_XMLEscape.cpp"
yy6:
		++current;
#line 84 "OW_XMLEscape.re"
		{ rval += "&amp;"; goto start; }
#line 132 "OW_XMLEscape.cpp"
yy8:
		++current;
#line 85 "OW_XMLEscape.re"
		{ rval += "&quot;"; goto start; }
#line 137 "OW_XMLEscape.cpp"
yy10:
		++current;
#line 86 "OW_XMLEscape.re"
		{ rval += "&apos;"; goto start; }
#line 142 "OW_XMLEscape.cpp"
yy12:
		++current;
#line 87 "OW_XMLEscape.re"
		{ rval += "&#13;"; goto start; }
#line 147 "OW_XMLEscape.cpp"
yy14:
		++current;
#line 88 "OW_XMLEscape.re"
		{ rval += "&#10;"; goto start; }
#line 152 "OW_XMLEscape.cpp"
yy16:
		++current;
#line 89 "OW_XMLEscape.re"
		{ rval += "&#9;"; goto start; }
#line 157 "OW_XMLEscape.cpp"
yy18:
		++current;
yy19:
#line 91 "OW_XMLEscape.re"
		{
			// This replaces the character sequence from (marker - 1) to (current)
			// with the UTF-8 replacement sequence.
			rval += "\xef\xbf\xbd"; // UTF-8 replacement sequence.
			goto start;
		}
#line 168 "OW_XMLEscape.cpp"
yy20:
		yych = (unsigned char)*(marker = ++current);
		switch (yych) {
		case 0xA0:	goto yy27;
		case 0xA1:
		case 0xA2:
		case 0xA3:
		case 0xA4:
		case 0xA5:
		case 0xA6:
		case 0xA7:
		case 0xA8:
		case 0xA9:
		case 0xAA:
		case 0xAB:
		case 0xAC:
		case 0xAD:
		case 0xAE:
		case 0xAF:
		case 0xB0:
		case 0xB1:
		case 0xB2:
		case 0xB3:
		case 0xB4:
		case 0xB5:
		case 0xB6:
		case 0xB7:
		case 0xB8:
		case 0xB9:
		case 0xBA:
		case 0xBB:
		case 0xBC:
		case 0xBD:
		case 0xBE:	goto yy28;
		case 0xBF:	goto yy29;
		default:	goto yy21;
		}
yy21:
#line 98 "OW_XMLEscape.re"
		{
			// Copy straight across.
			rval += *(current-1);
			marker = current + 1;
			goto start;
		}
#line 214 "OW_XMLEscape.cpp"
yy22:
		yych = (unsigned char)*(marker = ++current);
		switch (yych) {
		case 0xBF:	goto yy24;
		default:	goto yy21;
		}
yy23:
		yych = (unsigned char)*++current;
		goto yy21;
yy24:
		yych = (unsigned char)*++current;
		switch (yych) {
		case 0xBE:
		case 0xBF:	goto yy26;
		default:	goto yy25;
		}
yy25:
		current = marker;
		goto yy21;
yy26:
		yych = (unsigned char)*++current;
		goto yy19;
yy27:
		yych = (unsigned char)*++current;
		if (yych <= 0x7F) goto yy25;
		goto yy26;
yy28:
		yych = (unsigned char)*++current;
		switch (yych) {
		case '\n':	goto yy25;
		default:	goto yy26;
		}
yy29:
		++current;
		if ((yych = (unsigned char)*current) <= 0xBF) goto yy26;
		goto yy25;
	}
#line 104 "OW_XMLEscape.re"

 finish:

	return rval.releaseString();
}

} // end namespace OW_NAMESPACE

