/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc All rights reserved.
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

#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_StringStream.hpp"
#include "OW_UTF8Utils.hpp"
#include <fstream>
#include <iostream>
#include <assert.h>

using namespace std;
using namespace OpenWBEM;

StringArray sa1;
StringArray sa2;

void printStrings(const String& str1, const String& str2)
{
	cout << "\tunitAssert(UTF8Utils::compareToIgnoreCase(\"";
	for (int i = 0; i < str1.length(); ++i)
	{
		cout << hex << "\\x" << (int)(unsigned char)str1[i];
	}
	cout << "\", \"";
	for (int i = 0; i < str2.length(); ++i)
	{
		cout << hex << "\\x" << (int)(unsigned char)str2[i];
	}
	cout << "\") == 0)\n";
}



struct processLine
{
	void operator()(const String& s) const
	{
		if (s.empty() || !isxdigit(s[0]))
			return;

		StringArray a = s.tokenize(";"); // split up fields
		assert(a.size() >= 3);
		UInt32 c1 = a[0].toUInt32(16);
		StringArray a2 = a[2].tokenize(" "); // split up chars are separated by spaces
		Array<UInt32> c2chars(a2.size());
		for (size_t i = 0; i < a2.size(); ++i)
		{
			c2chars[i] = a2[i].toUInt32(16);
		}
		String str1 = UTF8Utils::UCS4toUTF8(c1);
		String str2;
		for (size_t i = 0; i < c2chars.size(); ++i)
		{
			str2 += UTF8Utils::UCS4toUTF8(c2chars[i]);
		}

		sa1.push_back(str1);
		sa2.push_back(str2);
		sa1.push_back(str2);
		sa2.push_back(str1);
	}
};

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		cerr << "must pass filename (to CaseFolding.txt)" << endl;
		return 1;
	}

	ifstream in(argv[1]);
	if (!in)
	{
		cerr << "could not open " << argv[1] << endl;
		return 1;
	}

	// add transitions for equal matches
	for (int i = 1; i < 256; ++i)
	{
		String s = String(char(i));
		sa1.push_back(s);
		sa2.push_back(s);
	}

	// read in a process the input file
	OStringStream ss;
	ss << in.rdbuf();
	String s = ss.toString();
	StringArray sa = s.tokenize("\n");
	for_each(sa.begin(), sa.end(), processLine());
	// do 1-1 comparisons
	for (int i = 0; i < sa1.size(); ++i)
	{
		printStrings(sa1[i], sa2[i]);
	}
	// do 10-10 comparisons
	for (int i = 0; i < sa1.size();)
	{
		String s1, s2;
		for (int j = 0; j < 10 && i < sa1.size(); ++j, ++i)
		{
			s1 += sa1[i];
			s2 += sa2[i];
		}
		printStrings(s1, s2);
	}
}

