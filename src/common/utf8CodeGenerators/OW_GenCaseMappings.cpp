/*******************************************************************************
* Copyright (C) 2003-2005 Vintela, Inc All rights reserved.
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
*  - Neither the name of Vintela, Inc nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Vintela, Inc OR THE CONTRIBUTORS
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

// The source of the Unicode data is: http://www.unicode.org/Public/UNIDATA/

#include "blocxx/String.hpp"
#include "blocxx/Array.hpp"
#include "blocxx/StringStream.hpp"
#include "blocxx/UTF8Utils.hpp"
#include <fstream>
#include <iostream>
#include <map>

using namespace std;
using namespace OpenWBEM;

map<UInt32, UInt32> upperMap;
map<UInt32, UInt32> lowerMap;

struct processLine
{
	void operator()(const String& s) const
	{
		if (s.empty() || !isxdigit(s[0]))
		{
			cout << "skipping line\n" << s << '\n';
			return;
		}

		StringArray a = s.tokenize(";", String::E_DISCARD_DELIMITERS, String::E_RETURN_EMPTY_TOKENS); // split up fields
		assert(a.size() >= 14);
		UInt32 c1 = a[0].toUInt32(16);
		if (a[13] != "")
		{
			lowerMap[c1] = a[13].toUInt32(16);
		}
		if (a[12] != "")
		{
			upperMap[c1] = a[12].toUInt32(16);
		}
	}
};

int utf8len(UInt32 ucs4char)
{
	if (ucs4char < 0x80u)
	{
		return 1;
	}
	else if (ucs4char < 0x800u)
	{
		return 2;
	}
	else if (ucs4char < 0x10000u)
	{
		return 3;
	}
	else
	{
		return 4;
	}
}


int main(int argc, char** argv)
{
	ifstream in("UnicodeData.txt");
	if (!in)
	{
		cerr << "could not open UnicodeData.txt" << endl;
		return 1;
	}

	// read in a process the input file
	OStringStream ss;
	ss << in.rdbuf();
	String s = ss.toString();
	StringArray sa = s.tokenize("\n");
	for_each(sa.begin(), sa.end(), processLine());
	cout << 
		"struct CaseMapping\n"
		"{\n"
		"\tUInt32 codePoint;\n"
		"\tUInt32 mapping;\n"
		"};\n";
	cout <<
		"const CaseMapping lowerMappings[] =\n"
		"{\n";
	for (map<UInt32, UInt32>::const_iterator i = lowerMap.begin(); i != lowerMap.end(); ++i)
	{
		cout << hex << "\t{0x" << i->first << ", 0x" << i->second << "},";
		if (utf8len(i->first) < utf8len(i->second))
		{
			// do this to see if there are any utf8 sequences that would grow when lower-casing them.
			cout << " // increasing utf8 length";
		}
		else if (utf8len(i->first) > utf8len(i->second))
		{
			// do this to see if there are any utf8 sequences that would grow when lower-casing them.
			cout << " // decreasing utf8 length";
		}
		cout << "\n";
	}
	cout << "};\n\n";
	cout << 
		"const CaseMapping upperMappings[] =\n"
		"{\n";
	for (map<UInt32, UInt32>::const_iterator i = upperMap.begin(); i != upperMap.end(); ++i)
	{
		cout << hex << "\t{0x" << i->first << ", 0x" << i->second << "},";
		if (utf8len(i->first) < utf8len(i->second))
		{
			// do this to see if there are any utf8 sequences that would grow when lower-casing them.
			cout << " // increasing utf8 length";
		}
		else if (utf8len(i->first) > utf8len(i->second))
		{
			// do this to see if there are any utf8 sequences that would grow when lower-casing them.
			cout << " // decreasing utf8 length";
		}
		cout << "\n";
	}
	cout << "};\n";
}

