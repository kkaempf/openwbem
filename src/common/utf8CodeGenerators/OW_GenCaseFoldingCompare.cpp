/*******************************************************************************
* Copyright (C) 2003 Vintela, Inc All rights reserved.
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
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_StringStream.hpp"
#include "OW_UTF8Utils.hpp"
#include <fstream>
#include <cctype> // for isxdigit
#include <algorithm>
#include <vector>
#include <iostream>
#include <cassert>

using namespace std;
using namespace OpenWBEM;

class StateMachine
{
public:
	static const int start = 0;
	static const int invalid = -1;

	StateMachine()
	{
		// create the start state
		m_states.resize(1);
	}
	int getTransition(int state, UInt8 input) const
	{
		assert(m_states.size() > state);
		return m_states[state].transitions[input];
	}

	int getStateStr(int state) const
	{
		assert(m_states.size() > state);
		return m_states[state].inputSelection;
	}

	int addState()
	{
		m_states.resize(m_states.size() + 1);
		int state = m_states.size() - 1;
		m_states[state].inputSelection = -1;
		m_states[state].acceptState = false;
		return state;
	}

	void setStateAccept(int state)
	{
		assert(m_states.size() > state);
		m_states[state].acceptState = true;
	}
	
	void setStateInputSelection(int state, int inputSelection)
	{
		assert(m_states.size() > state);
		assert(inputSelection == 1 || inputSelection == 2);
		assert(m_states[state].inputSelection == -1 || 
			   m_states[state].inputSelection == inputSelection);
		m_states[state].inputSelection = -1;
	}

	// val.inputSelection == 1, increment the pointer to str1
	// val.inputSelection == 2, increment the pointer to str2
	void addTransition(int transitionsState, UInt8 input, int nextState, int inputSelection)
	{
		assert(m_states.size() > transitionsState);
		assert(m_states.size() > nextState);
		assert(inputSelection == 1 || inputSelection == 2);
		assert(m_states[transitionsState].inputSelection == -1 || m_states[transitionsState].inputSelection == inputSelection);
		m_states[transitionsState].transitions[input] = nextState;
	}

	void debug() const
	{
		cout << "# states = " << m_states.size() << '\n';
	}

	// transitions, and whether state applies to str1 or str2
	struct state_t
	{
		state_t() 
			: transitions()
			, inputSelection(-1)
			, acceptState(false)
		{
			transitions.resize(256, -1);
		}
		vector<int> transitions;
		int inputSelection;
		bool acceptState;
	};
	//typedef pair<vector<int>, int> state_t;
	vector<state_t> m_states;
};

StateMachine stateMachine;

int followOrAddTransition(int curTransition, UInt8 input, int aux)
{
	int nextTransition = stateMachine.getTransition(curTransition, input);
	if (nextTransition == StateMachine::invalid)
	{
		// no transition, add one
		int newState = stateMachine.addState();
		stateMachine.setStateInputSelection(curTransition, aux);
		stateMachine.addTransition(curTransition, input, newState, aux);
		nextTransition = newState;
	}
	else
	{
		// found a transition
		assert(stateMachine.getStateStr(curTransition) == aux);
		// check that it's for the right string
/*
		int stateStr = stateMachine.getStateStr(nextTransition);
		if (stateStr != aux)
		{
			// not the right string, add a new one.
			int newState = stateMachine.addState();
			stateMachine.addTransition(curTransition, input, newState, aux);
			nextTransition = newState;
		}
*/
	}
	return nextTransition;
}

void buildTransitions(const String& str1, const String& str2)
{
	cout << "buildTransitions: str1 = \"";
	for (int i = 0; i < str1.length(); ++i)
	{
		cout << hex << "\\x" << (int)(unsigned char)str1[i];
	}
	cout << "\" str2 = \"";
	for (int i = 0; i < str2.length(); ++i)
	{
		cout << hex << "\\x" << (int)(unsigned char)str2[i];
	}
	cout << "\"\n";

	// do it transitions for str1/str2
	int trans = StateMachine::start;
	int pos1 = 0;
	int pos2 = 0;
	while (pos1 < str1.length() || pos2 < str2.length())
	{
		if (str1[pos1])
			trans = followOrAddTransition(trans, str1[pos1++], 1);

		if (str2[pos2])
			trans = followOrAddTransition(trans, str2[pos2++], 2);
	}
	// do it next for str2/str1
	trans = StateMachine::start;
	pos1 = 0;
	pos2 = 0;
	while (pos1 < str1.length() || pos2 < str2.length())
	{
		if (str2[pos2])
			trans = followOrAddTransition(trans, str2[pos2++], 1);

		if (str1[pos1])
			trans = followOrAddTransition(trans, str1[pos1++], 2);
	}
    stateMachine.setStateAccept(trans);
//	cout << "finished building SM for " << str1 << " " << str2 << "\n";
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

		buildTransitions(str1, str2);
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

	StringStream ss;
	ss << in.rdbuf();
	String s = ss.toString();
	StringArray sa = s.tokenize("\n");
	for_each(sa.begin(), sa.end(), processLine());

	// now add transitions for equal matches
/*	for (int i = 1; i < 256; ++i)
	{
		String s = String(char(i));
		buildTransitions(s, s);
	}
*/

	// TODO: disable duplicate states

	// now generate the code:
	cout << "/*******************************************************************************\n";
	cout << "* Copyright (C) 2003 Vintela, Inc All rights reserved.\n";
	cout << "*\n";
	cout << "* Redistribution and use in source and binary forms, with or without\n";
	cout << "* modification, are permitted provided that the following conditions are met:\n";
	cout << "*\n";
	cout << "*  - Redistributions of source code must retain the above copyright notice,\n";
	cout << "*    this list of conditions and the following disclaimer.\n";
	cout << "*\n";
	cout << "*  - Redistributions in binary form must reproduce the above copyright notice,\n";
	cout << "*    this list of conditions and the following disclaimer in the documentation\n";
	cout << "*    and/or other materials provided with the distribution.\n";
	cout << "*\n";
	cout << "*  - Neither the name of Center 7 nor the names of its\n";
	cout << "*    contributors may be used to endorse or promote products derived from this\n";
	cout << "*    software without specific prior written permission.\n";
	cout << "*\n";
	cout << "* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''\n";
	cout << "* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n";
	cout << "* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE\n";
	cout << "* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS\n";
	cout << "* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR\n";
	cout << "* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF\n";
	cout << "* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS\n";
	cout << "* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN\n";
	cout << "* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)\n";
	cout << "* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE\n";
	cout << "* POSSIBILITY OF SUCH DAMAGE.\n";
	cout << "*******************************************************************************/\n";
	cout << "\n// Do NOT modify this file.  It was generated by OW_GenCaseFoldingCompare.cpp\n";
	cout << "// If this file needs to be modified, change the generator and regenerate it.\n";
	cout << "#include \"OW_config.h\"\n";
	cout << "#include \"OW_UTF8Utils.hpp\"\n";
	cout << "\nnamespace OpenWBEM\n{\n";
	cout << "namespace UTF8Utils\n{\n";
	cout << "\n/////////////////////////////////////////////////////////////////////////////\n";

	cout << "int compareToIgnoreCase(const char* cstr1, const char* cstr2)\n";
	cout << "{\n";
	cout << "\tconst unsigned char* str1 = reinterpret_cast<const unsigned char*>(cstr1);\n";
	cout << "\tconst unsigned char* str2 = reinterpret_cast<const unsigned char*>(cstr2);\n";

	for (int i = 0; i < stateMachine.m_states.size(); ++i)
	{
		if (stateMachine.m_states[i].inputSelection == -1)
			continue; // it's an acceptance or duplicate state, we don't need to print it

		cout << "state" << i << ":\n";
		if (stateMachine.m_states[i].inputSelection == 1)
		{
			cout << "\tswitch(*(str1++)){\n";
		}
		else
		{
			cout << "\tswitch(*(str2++)){\n";
		}

		if (i == 0)
			cout << "\t\tcase 0x0: goto zero;\n";

		for (int j = 0; j < stateMachine.m_states[i].transitions.size(); ++j)
		{
			if (stateMachine.m_states[i].transitions[j] != -1)
			{
				if (stateMachine.m_states[stateMachine.m_states[i].transitions[j]].inputSelection == -1)
					cout << "\t\tcase 0x" << hex << j << ": goto state0;\n";
				else
					cout << "\t\tcase 0x" << hex << j << ": goto state" << stateMachine.m_states[i].transitions[j] << ";\n";
			}
		}
		cout << "\t\tdefault: goto no_match;\n\t}\n";
	}
	cout << "no_match:\n";
	cout << "\treturn *(str1-1) - *(str2-1);\n";
	cout << "zero:\n";
	cout << "\treturn 0 - *str2;\n";
	cout << "}\n\n";
	cout << "} // end namespace UTF8Utils\n";
	cout << "} // end namespace OpenWBEM\n\n";
}

