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
#include <map>
#include <set>

using namespace std;
using namespace OpenWBEM;

#if 1
#define DEBUG(x) cout << x
#else
#define DEBUG(x)
#endif

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
		return m_states[state].transitions[input].toState;
	}

	int getStateStr(int state, UInt8 input) const
	{
		assert(m_states.size() > state);
		return m_states[state].transitions[input].inputSelection;
	}

	int addState()
	{
		m_states.resize(m_states.size() + 1);
		int state = m_states.size() - 1;
		m_states[state].acceptState = false;
		return state;
	}

	void setStateAccept(int state)
	{
		assert(m_states.size() > state);
		m_states[state].acceptState = true;
	}

	// val.inputSelection == 1, increment the pointer to str1
	// val.inputSelection == 2, increment the pointer to str2
	void addTransition(int transitionsState, UInt8 input, int nextState, int inputSelection)
	{
		assert(m_states.size() > transitionsState);
		assert(m_states.size() > nextState);
		assert(inputSelection == 1 || inputSelection == 2);
		assert(m_states[transitionsState].transitions[input].toState == invalid);
		assert(m_states[transitionsState].transitions[input].inputSelection == -1);
		m_states[transitionsState].transitions[input].toState = nextState;
		m_states[transitionsState].transitions[input].inputSelection = inputSelection;
	}

	void debug() const
	{
		DEBUG("# states = " << m_states.size() << '\n');
	}

	void removeDuplicateState(int state1, int state2)
	{
		assert(state1 < state2);
		m_states.erase(m_states.begin() + state2);
		for (int i = 0; i < m_states.size(); ++i)
		{
			for (int j = 0; j < m_states[i].transitions.size(); ++j)
			{
				if (m_states[i].transitions[j].toState == state2)
				{
					m_states[i].transitions[j].toState = state1;
				}
				else if (m_states[i].transitions[j].toState > state2)
				{
					// we removed a state, so we just decrement it by one.
					--m_states[i].transitions[j].toState;
				}
			}
		}
	}
	
	// transitions, and whether state applies to str1 or str2
	struct transition_t
	{
		transition_t()
		: inputSelection(-1)
		, toState(invalid)
		{
		}
		int inputSelection;
		int toState;

		friend bool operator==(const transition_t& x, const transition_t& y)
		{
			return (x.inputSelection == y.inputSelection) && (x.toState == y.toState);
		}
	};

	struct state_t
	{
		state_t() 
		: transitions()
		, acceptState(false)
		{
			transitions.resize(256);
		}
		vector<transition_t> transitions;
		bool acceptState;

		friend bool operator==(const state_t& x, const state_t& y)
		{
			return (x.transitions == y.transitions) && (x.acceptState == y.acceptState);
		}
	};
	vector<state_t> m_states;
};

StateMachine stateMachine;
typedef std::multimap<String, String> mmap_t;
typedef mmap_t::const_iterator ci_t;
std::multimap<String, String> caseFoldingEntries;

int followOrAddTransition(int curTransition, UInt8 input, int aux)
{
	int nextTransition = stateMachine.getTransition(curTransition, input);
	if (nextTransition == StateMachine::invalid)
	{
		// no transition, add one
		int newState = stateMachine.addState();
		DEBUG("added new state " << newState);
		//stateMachine.setInputSelection(curTransition, input, aux);
		stateMachine.addTransition(curTransition, input, newState, aux);
		DEBUG(" and transition from " << curTransition << " to " << newState << " on input " << aux << " = \\x" << int(input) << endl);
		nextTransition = newState;
	}
	else
	{
		if (stateMachine.getStateStr(curTransition, input) == -1)
		{
			DEBUG("?? stateMachine.getStateStr(curTransition, input) == -1 ??" << endl);
			//stateMachine.setInputSelection(curTransition, input, aux);
		}
		DEBUG("transition already exists.  curTransition = " << curTransition << " aux = " << aux << " getStateStr = " << stateMachine.getStateStr(curTransition, input) << endl);
		// found a transition
		assert(stateMachine.getStateStr(curTransition, input) == aux);
	}
	return nextTransition;
}

void printStrings(const String& str1, const String& str2)
{
	DEBUG("buildTransitions: str1 = \"");
	for (int i = 0; i < str1.length(); ++i)
	{
		DEBUG( hex << "\\x" << (int)(unsigned char)str1[i]);
	}
	DEBUG("\" str2 = \"");
	for (int i = 0; i < str2.length(); ++i)
	{
		DEBUG( hex << "\\x" << (int)(unsigned char)str2[i]);
	}
	DEBUG( "\"\n");
}


void buildTransitions(const String& str1, const String& str2)
{
	printStrings(str1, str2);

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
	DEBUG( "setStateAccept(" << trans << ")\n");
	stateMachine.setStateAccept(trans);
}

struct processLine
{
	void operator()(const String& s) const
	{
		if (s.empty() || !isxdigit(s[0]))
			return;

		DEBUG("processLine: " << s << endl);
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

		caseFoldingEntries.insert(std::make_pair(str1, str2));
		caseFoldingEntries.insert(std::make_pair(str2, str1));
		//buildTransitions(str1, str2);
		//buildTransitions(str2, str1);
	}
};

struct isForInput : public unary_function<StateMachine::transition_t, bool>
{
	isForInput(int input) : m_input(input) {}
	int m_input;
	bool operator()(const StateMachine::transition_t& t)
	{
		return t.inputSelection == m_input;
	}
};

void outputHeader()
{
	cout << "/*******************************************************************************\n";
	cout << "* Copyright (C) 2003-2004 Vintela, Inc All rights reserved.\n";
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
	cout << "*  - Neither the name of Vintela, Inc nor the names of its\n";
	cout << "*    contributors may be used to endorse or promote products derived from this\n";
	cout << "*    software without specific prior written permission.\n";
	cout << "*\n";
	cout << "* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''\n";
	cout << "* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n";
	cout << "* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE\n";
	cout << "* ARE DISCLAIMED. IN NO EVENT SHALL Vintela, Inc OR THE CONTRIBUTORS\n";
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
}

void outputTransitions(const StateMachine::state_t& state, int inputSelection, bool outputDefault)
{
	for (int j = 0; j < state.transitions.size(); ++j)
	{
		if (state.transitions[j].toState != -1 && state.transitions[j].inputSelection == inputSelection)
		{
			cout <<	"\t\tcase 0x" << hex <<	j << ": goto state"	<< state.transitions[j].toState << ";\n";
		}
	}
	if (outputDefault)
	{
		cout << "\t\tdefault: goto no_match;\n";
	}
	cout << "\t}\n";
}

void outputFirstState(const StateMachine::state_t& state)
{
	cout << "\tswitch(*(str1++)){\n";
	// first state has to handle 0
	cout << "\t\tcase 0x0: goto zero;\n";
	outputTransitions(state, 1, true);
}

void outputSwitch(const StateMachine::state_t& state, int inputSelection, bool outputDefault)
{
	cout << "\tswitch(*(str" << inputSelection << "++)){\n";
	outputTransitions(state, inputSelection, outputDefault);
}

void outputCode()
{
	outputHeader();

	cout << "#include \"OW_config.h\"\n";
	cout << "#include \"OW_UTF8Utils.hpp\"\n";
	cout << "\nnamespace OpenWBEM\n{\n";
	cout << "namespace UTF8Utils\n{\n";
	cout << "\n/////////////////////////////////////////////////////////////////////////////\n";

	cout << "int compareToIgnoreCase(const char* cstr1, const char* cstr2)\n";
	cout << "{\n";
	cout << "\tconst unsigned char* str1 = reinterpret_cast<const unsigned char*>(cstr1);\n";
	cout << "\tconst unsigned char* str2 = reinterpret_cast<const unsigned char*>(cstr2);\n";
	cout << "\tconst unsigned char* str1marker = 0;\n";
	cout << "\tconst unsigned char* str2marker = 0;\n";
	cout << "\tgoto state0;\n";
	cout << "no_match:\n";
	cout << "\tif (str1marker) {\n";
	cout << "\t\tstr1 = str1marker; str1marker = 0;\n";
	cout << "\t\tstr2 = str2marker; str2marker = 0;\n";
	cout << "\t\tgoto state0;\n";
	cout << "\t}\n";
	cout << "\treturn *(str1-1) - *(str2-1);\n";
	cout << "zero:\n";
	cout << "\treturn 0 - *str2;\n";

	for (int i = 0; i < stateMachine.m_states.size(); ++i)
	{

		cout << "state" << i << ":\n";
		int c1 = count_if(stateMachine.m_states[i].transitions.begin(), stateMachine.m_states[i].transitions.end(), isForInput(1));
		int c2 = count_if(stateMachine.m_states[i].transitions.begin(), stateMachine.m_states[i].transitions.end(), isForInput(2));

		if (i == 0)
		{
			outputFirstState(stateMachine.m_states[0]);
		}
		else
		{
			// we're in an accept state with outgoing transitions, we need to save our position
			if ((c1 || c2) && stateMachine.m_states[i].acceptState) 
			{
				cout << "\tstr1marker = str1;\n";
				cout << "\tstr2marker = str2;\n";
			}
			if (c1)
			{
				outputSwitch(stateMachine.m_states[i], 1, c2 == 0);
			}
			// need to rewind str1 in the case of 2 switches and the first one doesn't match
			if (c1 && c2)
			{
				cout << "\t--str1;\n";
			}
			if (c2)
			{
				outputSwitch(stateMachine.m_states[i], 2, true); // true means ouput the default
			}
			if (c1 == 0 && c2 == 0)
			{
				if (stateMachine.m_states[i].acceptState)
				{
					cout << "\tgoto state0;\n";
				}
				else
				{
					cout << "\tgoto no_match;\n";
				}
			}
		}


	}
	cout << "}\n\n";
	cout << "} // end namespace UTF8Utils\n";
	cout << "} // end namespace OpenWBEM\n\n";
}

bool findDuplicateStates(int& state1, int& state2)
{
	for (int i = stateMachine.m_states.size() - 2; i > -1; --i)
	{
		for (int j = stateMachine.m_states.size() -1; j > i; --j)
		{
			if (stateMachine.m_states[i] == stateMachine.m_states[j])
			{
				state1 = i;
				state2 = j;
				DEBUG("found duplicate states: " << i << " and " << j << endl);
				return true;
			}
		}
	}
	return false;
}

void minimizeStateMachine()
{
	// this is a horribly inefficient way of doing this, but it works, was 
	// simple to code, and it only needs to be run once.
	DEBUG("minimizing state machine\n");
	int state1 = StateMachine::invalid;
	int state2 = StateMachine::invalid;
	while (findDuplicateStates(state1, state2))
	{
		stateMachine.removeDuplicateState(state1, state2);
	}
}

void getEntriesFor(const String& key, set<String>& rval)
{
	for (ci_t ci = caseFoldingEntries.lower_bound(key);
		ci->first == key;
		++ci)
	{
		if (rval.find(ci->second) == rval.end())
		{
			rval.insert(ci->second);
			getEntriesFor(ci->second, rval);
		}
	}
}

bool haveEntry(const String& key, const String& val)
{
	for (ci_t ci = caseFoldingEntries.lower_bound(key);
		ci->first == key;
		++ci)
	{
		if (ci->second == val)
			return true;
	}
	return false;
}

void calculateTransitiveClosure()
{
	DEBUG("calculateTransitiveClosure\n");
start_over:
	for (ci_t ci = caseFoldingEntries.begin();
		ci != caseFoldingEntries.end();
		++ci)
	{
		set<String> newEntries;
		getEntriesFor(ci->second, newEntries);
		bool addedAnEntry = false;
		String key = ci->first;  // make a copy since the iterator may be invalidated after an insert
		for (set<String>::const_iterator curEntry = newEntries.begin(); curEntry != newEntries.end(); ++curEntry)
		{
			if (!haveEntry(key, *curEntry))
			{
				caseFoldingEntries.insert(std::make_pair(key, *curEntry));
				addedAnEntry = true;
			}
		}

		if (addedAnEntry)
			goto start_over; // since the iterators may be invalidated.
	}
}

void buildStateMachine()
{
	for (ci_t ci = caseFoldingEntries.begin();
		ci != caseFoldingEntries.end();
		++ci)
	{
		buildTransitions(ci->first, ci->second);
	}
}

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
		caseFoldingEntries.insert(std::make_pair(s, s));
		//buildTransitions(s, s);
	}

	// read in a process the input file
	OStringStream ss;
	ss << in.rdbuf();
	String s = ss.toString();
	StringArray sa = s.tokenize("\n");
	for_each(sa.begin(), sa.end(), processLine());

	calculateTransitiveClosure();
	buildStateMachine();

	// disable duplicate states
	minimizeStateMachine();

	// now generate the code:
	outputCode();
}

