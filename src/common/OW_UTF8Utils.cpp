/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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
// Portions of this file based on utf/detail/utf8_algo.hpp
//
//  Copyright (c) 2003 Alberto Barbati
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
#include "OW_config.h"
#include "OW_UTF8Utils.hpp"
#include "OW_String.hpp"
#include "OW_Assertion.hpp"

namespace OpenWBEM
{

namespace UTF8Utils
{
namespace {
/*
   UTF-8 sequence length table.
   This table gives the sequence length according to the value of the
   sequence leading character.
   Notice: this table is different from the one found in the official
   UTF-8 conversion program, found here
   http://www.unicode.org/Public/PROGRAMS/CVTUTF/
   This table encodes only the sequence lenghts of UTF-8 sequences
   that can encode (or possibly encode) a Unicode character according
   to Unicode 3.2 requirements.
   In particular:
   - lines from 0x80 to 0xb0 (inclusive) contains "0" because those
   values do not represent a valid leading character
   - line 0xc0 begins with two "0" because values 0xc0 and 0xc1 lead
   non-shortest sequences, that are illegal since Unicode 3.1
   - line 0xf0 has only five "4" instead of eight and lacks values
   "5" and "6" because values above 0xf4 lead sequences that cannot
   encode a Unicode character.
*/
UInt8 SequenceLengthTable[256] =
{
	/* 0x00 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x0f */
	/* 0x10 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x1f */
	/* 0x20 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x2f */
	/* 0x30 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x3f */
	/* 0x40 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x4f */
	/* 0x50 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x5f */
	/* 0x60 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x6f */
	/* 0x70 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x7f */
	/* 0x80 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x8f */
	/* 0x90 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x9f */
	/* 0xa0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xaf */
	/* 0xb0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0xbf */
	/* 0xc0 */ 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, /* 0xcf */
	/* 0xd0 */ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, /* 0xdf */
	/* 0xe0 */ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, /* 0xef */
	/* 0xf0 */ 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  /* 0xff */
};
} // end unnamed namespace
size_t charCount(const char* utf8str)
{
	OW_ASSERT(utf8str != 0);
	const char* p = utf8str;
	size_t count = 0;
	while (*p)
	{
		// any chars 0x80-0xBF are extension bytes.  Anything else signals a new char
		UInt8 c = static_cast<UInt8>(*p);
		if (c < 0x80 || c > 0xBF)
			++count;
		++p;
	}
	return count;
}
UInt16 UTF8toUCS2(const char* utf8char)
{
	OW_ASSERT(utf8char != 0);
	OW_ASSERT(utf8char[0] != '\0');
	const char* p = utf8char;
	const UInt32 c0 = static_cast<UInt8>(p[0]);
	switch (SequenceLengthTable[c0])
	{
		case 1:
		{
			return c0;
		}
		case 2:
		{
			// check for short (invalid) utf8 sequence
			if (p[1] == '\0')
				return c0;
			const UInt32 c1 = static_cast<UInt8>(p[1]);
			return ((c0 & 0x1fu) << 6) | (c1 & 0x3fu);
		}
		case 3:
		{
			// check for short (invalid) utf8 sequence
			if (p[1] == '\0' || p[2] == '\0')
				return c0;
			const UInt32 c1 = static_cast<UInt8>(p[1]);
			const UInt32 c2 = static_cast<UInt8>(p[2]);
			return ((c0 & 0x0fu) << 12) | ((c1 & 0x3fu) << 6) | (c2 & 0x3fu);
		}
		case 4:
		{
			// check for short (invalid) utf8 sequence
			if (p[1] == '\0' || p[2] == '\0' || p[3] == '\0')
				return c0;
			
			const UInt32 c1 = static_cast<UInt8>(p[1]);
			const UInt32 c2 = static_cast<UInt8>(p[2]);
			const UInt32 c3 = static_cast<UInt8>(p[3]);
			
			return ((c0 & 0x03u) << 18) | ((c1 & 0x3fu) << 12) | ((c2 & 0x3fu) << 6) | (c3 & 0x3fu);
		}
		default:
		{
			// invalid, just skip it
			break;
		}
	}
	return c0;
}
String UCS2toUTF8(UInt16 ucs2char)
{
	char rval[4] = {0,0,0,0};
	if(ucs2char < 0x80u)
	{
		// one byte
		rval[0] = static_cast<char>(static_cast<UInt8>(ucs2char));
	}
	else if(ucs2char < 0x800u)
	{
		rval[0] = static_cast<char>(static_cast<UInt8>(0xc0u | (ucs2char >> 6)));
		rval[1] = static_cast<char>(static_cast<UInt8>(0x80u | (ucs2char & 0x3fu)));
	}
	else if(ucs2char < 0x10000u)
	{
		rval[0] = static_cast<char>(static_cast<UInt8>(0xe0u | (ucs2char >> 12)));
		rval[1] = static_cast<char>(static_cast<UInt8>(0x80u | ((ucs2char >> 6) & 0x3fu)));
		rval[2] = static_cast<char>(static_cast<UInt8>(0x80u | (ucs2char & 0x3fu)));
	}
	return rval;
}
} // end namespace OW_UTF8Utils

} // end namespace OpenWBEM

