#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_StringStream.hpp"
#include "OW_UTF8Utils.hpp"
#include <fstream>
#include <cctype> // for isxdigit
#include <algorithm>
#include <vector>
#include <iostream>

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
		m_states[0].first.resize(256, -1);
		m_states[0].second = -1;
	}
	int getTransition(int state, UInt8 input) const
	{
		assert(m_states.size() > state);
		return m_states[state].first[input];
	}

	int getStateStr(int state) const
	{
		assert(m_states.size() > state);
		return m_states[state].second;
	}

	int addState()
	{
		m_states.resize(m_states.size() + 1);
		int state = m_states.size() - 1;
		m_states[state].first.resize(256, -1);
		m_states[state].second = -1;
		return state;
	}

	// val.second == 1, increment the pointer to str1
	// val.second == 2, increment the pointer to str2
	void addTransition(int firstState, UInt8 input, int nextState, int stateSelection)
	{
		assert(m_states.size() > firstState);
		assert(m_states.size() > nextState);
		assert(stateSelection == 1 || stateSelection == 2);
		assert(m_states[firstState].second == -1 || m_states[firstState].second == stateSelection);
		m_states[firstState].first[input] = nextState;
		m_states[firstState].second = stateSelection;
	}

	void debug() const
	{
		cout << "# states = " << m_states.size() << '\n';
	}

	// transitions, and whether state applies to str1 or str2
	typedef pair<vector<int>, int> state_t;
	vector<state_t> m_states;
};

StateMachine stateMachine;

int followOrAddTransition(int curTransition, UInt8 input, int aux)
{
	int nextTransition = stateMachine.getTransition(curTransition, input);
	if (nextTransition == StateMachine::invalid)
	{
		int newState = stateMachine.addState();
		stateMachine.addTransition(curTransition, input, newState, aux);
		nextTransition = newState;
	}
	else
	{
		assert(stateMachine.getStateStr(curTransition) == aux);
	}
	return nextTransition;
}

void buildTransitions(const String& str1, const String& str2)
{
	// do it first for str1/str2
	int trans = StateMachine::start;
	int pos1 = 0;
	int pos2 = 0;
	while (pos1 < str1.length() && pos1 < str2.length())
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
	while (pos1 < str1.length() && pos1 < str2.length())
	{
		if (str2[pos2])
			trans = followOrAddTransition(trans, str2[pos2++], 1);

		if (str1[pos1])
			trans = followOrAddTransition(trans, str1[pos1++], 2);
	}
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
		// do it first for str1/str2
		int trans = StateMachine::start;
		int pos1 = 0;
		int pos2 = 0;
		while (pos1 < str1.length() && pos1 < str2.length())
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
		while (pos1 < str1.length() && pos1 < str2.length())
		{
			if (str2[pos2])
				trans = followOrAddTransition(trans, str2[pos2++], 1);

			if (str1[pos1])
				trans = followOrAddTransition(trans, str1[pos1++], 2);
		}
//		cout << "finished building SM for " << s << "\n";

	}
};

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		cerr << "must pass filename (to CaseFolding.txt)" << endl;
		return 1;
	}

	fstream in(argv[1]);
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
	for (int i = 0; i < 256; ++i)
	{
		String s = String(char(i));
		buildTransitions(s, s);
	}

	// now generate the code:
	cout << "int utf8strcasecmp(const char* str1, const char* str2)\n";
	cout << "{\n";
	cout << "begin:\n";

	for (int i = 0; i < stateMachine.m_states.size(); ++i)
	{
		if (stateMachine.m_states[i].second == -1)
			continue; // it's an acceptance state, we don't need to print it

		cout << "state" << i << ":\n";
		if (stateMachine.m_states[i].second == 1)
		{
			cout << "\tswitch(*(str1++)){\n";
		}
		else
		{
			cout << "\tswitch(*(str2++)){\n";
		}

		if (i == 0)
			cout << "\t\tcase 0x0: goto zero;\n";

		for (int j = 0; j < stateMachine.m_states[i].first.size(); ++j)
		{
			if (stateMachine.m_states[i].first[j] != -1)
			{
				if (stateMachine.m_states[stateMachine.m_states[i].first[j]].second == -1)
					cout << "\t\tcase 0x" << hex << j << ": goto begin;\n";
				else
					cout << "\t\tcase 0x" << hex << j << ": goto state" << stateMachine.m_states[i].first[j] << ";\n";
			}
		}
		cout << "\t\tdefault: goto no_match;\n\t}\n";
	}
	cout << "match:\n";
	cout << "\treturn 0;\n";
	cout << "no_match:\n";
	cout << "\treturn *(str1-1) - *(str2-1);\n";
	cout << "zero:\n";
	cout << "\treturn 0 - *str2;\n";
	cout << "}\n";

}

