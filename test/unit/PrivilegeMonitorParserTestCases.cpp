/*******************************************************************************
 * Copyright (C) 2006 Quest Software, Inc. All rights reserved.
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

/*
  ###########################################################################
  Tests that still need to be done:
  ###########################################################################
  open_rw { }
  open_r { }
  open_w { }
  open_a { }
  read_dir { }
  check_path { }
  rename_from { }
  rename_to { }
  rename_from_to { }
  remove_file { }

  ###########################################################################
  Tests covered here:
  ###########################################################################
  unpriv_user
  user_exec
  user_exec_check_args
  monitored_exec
  monitored_exec_check_args
  monitored_user_exec
  monitored_user_exec_check_args
*/


#include "OW_config.h"
#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "PrivilegeMonitorParserTestCases.hpp"
AUTO_UNIT_TEST_SUITE_NAMED(PrivilegeMonitorParserTestCases,"PrivilegeMonitorParser");

#include "OW_PrivilegeConfig.hpp"
#include "blocxx/StringStream.hpp"
#include "blocxx/Format.hpp"
#include "blocxx/Reference.hpp"
#include <cassert>

using namespace OpenWBEM;
using namespace blocxx;


// set this to 1 to enable parser debugging
//extern int openwbem_privconfig_debug;

void PrivilegeMonitorParserTestCases::setUp()
{
}

void PrivilegeMonitorParserTestCases::tearDown()
{
}

namespace // anonymous
{
	class TestIncludeHandler : public PrivilegeConfig::IncludeHandler
	{
	public:
		TestIncludeHandler()
		: m_liveIncludeCount(0)
		{
		}

		TestIncludeHandler(const StringArray& names, const StringArray& includes)
		: m_liveIncludeCount(0)
		, m_names(names)
		{
			for (size_t i = 0; i < includes.size(); ++i)
			{
				m_s.push_back(Reference<IStringStream>(new IStringStream(includes[i])));
			}
		}

		virtual std::istream* getInclude(const String& includeParam)
		{
			for (size_t i = 0; i < m_names.size(); ++i)
			{
				if (m_names[i] == includeParam)
				{
					++m_liveIncludeCount;
					return m_s[i].getPtr();
				}
			}
			assert(0);
			return 0;
		}

		virtual void endInclude()
		{
			--m_liveIncludeCount;
		}
	private:
		int m_liveIncludeCount;
		StringArray m_names;
		Array<Reference<IStringStream> > m_s;
	};

	#define FIRST_PRIV_BUFFER_NAME "original buffer"
	//
	// Helper functions for parsing privilege strings
	//
	bool parsePrivilegeStream(std::istream& s, OpenWBEM::PrivilegeConfig::Privileges& privileges, OpenWBEM::PrivilegeConfig::ParseError error)
	{
		TestIncludeHandler tih;
		PrivilegeConfig::openwbem_privconfig_Lexer lex(s, tih, FIRST_PRIV_BUFFER_NAME);
		return openwbem_privconfig_parse(&privileges, &error, &lex) == 0;
	}

	bool parsePrivilegeStreamInclude(std::istream& s, PrivilegeConfig::IncludeHandler& includeHandler, OpenWBEM::PrivilegeConfig::Privileges& privileges, OpenWBEM::PrivilegeConfig::ParseError error)
	{
		PrivilegeConfig::openwbem_privconfig_Lexer lex(s, includeHandler, FIRST_PRIV_BUFFER_NAME);
		return openwbem_privconfig_parse(&privileges, &error, &lex) == 0;
	}

	bool parsePrivilegeString(const String& s, OpenWBEM::PrivilegeConfig::Privileges& privileges, OpenWBEM::PrivilegeConfig::ParseError error)
	{
		IStringStream str(s);
		return parsePrivilegeStream(str, privileges, error);
	}


	bool parsePrivilegeStringInclude(const String& s, const String& include, OpenWBEM::PrivilegeConfig::Privileges& privileges, OpenWBEM::PrivilegeConfig::ParseError error)
	{
		IStringStream str(s);
		StringArray names(1, "input2");
		StringArray includes(1, include);
		names.push_back(FIRST_PRIV_BUFFER_NAME);
		includes.push_back(s);
		TestIncludeHandler tih(names, includes);
		return parsePrivilegeStreamInclude(str, tih, privileges, error);
	}

	bool parsePrivilegeStringInclude(const String& s, TestIncludeHandler& tih, OpenWBEM::PrivilegeConfig::Privileges& privileges, OpenWBEM::PrivilegeConfig::ParseError error)
	{
		IStringStream str(s);
		return parsePrivilegeStreamInclude(str, tih, privileges, error);
	}

	//
	// Helper functions for converting a string into the executable/arguments it contains.
	//
	String getExecutable(const String& var)
	{
		StringArray foo = var.tokenize();
		if( !foo.empty() )
		{
			return *foo.begin();
		}
		return String();
	}

	StringArray getArguments(const String& var)
	{
		return var.tokenize();
	}
} // end anonymous namespace


void PrivilegeMonitorParserTestCases::parseEmpty()
{
	// Perfectly legal to have an empty privilege file.
	{
		String empty;
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeString(empty, privileges, error));
	}
	// Empty specs
	{
		String emptyspecs("\
open_rw { }\n\
open_r { }\n\
open_w { }\n\
open_a { }\n\
read_dir { }\n\
stat { }\n\
check_path { }\n\
rename_from { }\n\
rename_to { }\n\
rename_from_to { }\n\
remove_file { }\n\
remove_dir { }\n\
unpriv_user { me }\n\
monitored_exec { }\n\
monitored_user_exec { }\n\
user_exec { }\n\
monitored_exec_check_args { }\n\
monitored_user_exec_check_args { }\n\
user_exec_check_args { }");

		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeString(emptyspecs, privileges, error));
	}
}


void PrivilegeMonitorParserTestCases::parseInvalid()
{
	// Illegal to have non-matching braces.
	String input1("{");
	String input2("}");
	OpenWBEM::PrivilegeConfig::Privileges privileges;
	OpenWBEM::PrivilegeConfig::ParseError error;
	unitAssert(!parsePrivilegeString(input1, privileges, error));
	unitAssert(!parsePrivilegeString(input2, privileges, error));
}


void PrivilegeMonitorParserTestCases::parseValidSimple()
{
	// Very simple
	{
		String user("me");
		String input(Format("unpriv_user { %1 }", user));
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeString(input, privileges, error));
		unitAssertEquals(user, privileges.unpriv_user);
	}

	// Simple, but includes multiple unpriv_user specs.
	{
		String user("me");
		String input(Format("\
unpriv_user { %1%1 }\n\
unpriv_user { %1 }\n\
unpriv_user { %1%1%1 }",
				user));
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeString(input, privileges, error));
		unitAssertEquals(user + user + user, privileges.unpriv_user);
	}
}


void PrivilegeMonitorParserTestCases::parseValidUserExec()
{
	StringArray emptyEnv;
	// Test the user_exec section...
	{
		String cmd1("/bin/foo");
		String cmd2("/bin/bar");
		String cmd3("/bin/baz");
		String bad_cmd("/bin/quux");
		String user("id");
		String bad_user("occidare");
		String input(Format("\
user_exec                                       \n\
{                                               \n\
  %2 @ %1                                       \n\
  %3 @ %1                                       \n\
  %4 @ %1                                       \n\
  # Note the lack of /bin/quux                  \n\
}                                                 \
",
				user, cmd1, cmd2, cmd3));
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeString(input, privileges, error));
		// All of the commands should match when executed as the correct user.
		unitAssert(privileges.user_exec.match(cmd1, emptyEnv, user));
		unitAssert(privileges.user_exec.match(cmd2, emptyEnv, user));
		unitAssert(privileges.user_exec.match(cmd3, emptyEnv, user));
		unitAssert(!privileges.user_exec.match(bad_cmd, emptyEnv, user));

		// All of the commands should fail to match when executed as an invalid (unspecified) user.
		unitAssert(!privileges.user_exec.match(cmd1, emptyEnv, bad_user));
		unitAssert(!privileges.user_exec.match(cmd2, emptyEnv, bad_user));
		unitAssert(!privileges.user_exec.match(cmd3, emptyEnv, bad_user));
		unitAssert(!privileges.user_exec.match(bad_cmd, emptyEnv, bad_user));
	}
	// Test the user_exec_check_args section
	{
		String cmd1("/bin/foo");
		String cmd2("/bin/foo a b c");
		String cmd3("/bin/foo #@! f g");
		String cmd4("/bin/foo *");
		String cmd5("/bin/foo * @ *");
		String cmd6("/bin/foo str");

		String bad_cmd1("/bin/quux");
		String bad_cmd2("/bin/foo x y z");
		String bad_cmd3("/bin/foo a b d");
		String bad_cmd4("/bin/foo @");
		String bad_cmd5("/bin/foo * *");
		String bad_cmd6("/bin/foo * *");
		String bad_cmd7("/bin/foo * @ @ *");
		String user("id");
		String bad_user("occidare");
		String input(Format("\
user_exec_check_args	                        \n\
{                                               \n\
  /bin/foo @ %1                                 \n\
  /bin/foo \"a\" \"b\" \"c\" @ %1                           \n\
  # Allow a single argument to be anything      \n\
  /bin/foo * \"f\" \"g\" @ %1                           \n\
  /bin/foo \"*\" @ %1                             \n\
  /bin/foo \"*\" \"@\" \"*\" @ %1                     \n\
  # string arguments                            \n\
  /bin/foo \"str\" @ %1                         \n\
  /opt/quest/bin/vastool       \"-u\" \"host/\" \"search\" * \"serviceBindingInformation\" @ root\n\
}                                                 \
", user, cmd1, cmd2, cmd3));
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeString(input, privileges, error));

#define CHECKCOMMAND(cmd, user)														\
		privileges.user_exec_check_args.match(getExecutable(cmd), getArguments(cmd), emptyEnv, user)
#define CHECKCOMMAND2(cmd, args, user)														\
		privileges.user_exec_check_args.match(cmd, getArguments(args), emptyEnv, user)

		// All of the commands should match when executed as the correct user.
		unitAssert(CHECKCOMMAND(cmd1, user));
		unitAssert(CHECKCOMMAND(cmd2, user));
		unitAssert(CHECKCOMMAND(cmd3, user));
		unitAssert(CHECKCOMMAND(cmd4, user));
		unitAssert(CHECKCOMMAND(cmd5, user));
		unitAssert(CHECKCOMMAND(cmd6, user));
		unitAssert(!CHECKCOMMAND(bad_cmd1, user));
		unitAssert(!CHECKCOMMAND(bad_cmd2, user));
		unitAssert(!CHECKCOMMAND(bad_cmd3, user));
		unitAssert(!CHECKCOMMAND(bad_cmd4, user));
		unitAssert(!CHECKCOMMAND(bad_cmd5, user));
		unitAssert(!CHECKCOMMAND(bad_cmd6, user));
		unitAssert(!CHECKCOMMAND(bad_cmd7, user));

		unitAssert(CHECKCOMMAND("/opt/quest/bin/vastool -u host/ search (cn=SMS-Site-DAN) serviceBindingInformation", "root"));
		unitAssert(!CHECKCOMMAND2("/opt/quest/bin/vastool", "/opt/quest/bin//vastool -u host/ search (cn=SMS-Site-DAN) serviceBindingInformation", "root"));

		// All of the commands should fail to match when executed as an invalid (unspecified) user.
		unitAssert(!CHECKCOMMAND(cmd1, bad_user));
		unitAssert(!CHECKCOMMAND(cmd2, bad_user));
		unitAssert(!CHECKCOMMAND(cmd3, bad_user));
		unitAssert(!CHECKCOMMAND(cmd4, bad_user));
		unitAssert(!CHECKCOMMAND(cmd5, bad_user));
		unitAssert(!CHECKCOMMAND(cmd6, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd1, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd2, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd3, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd4, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd5, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd6, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd7, bad_user));
#undef CHECKCOMMAND

	}
}


void PrivilegeMonitorParserTestCases::parseValidMonitoredUserExec()
{
	StringArray emptyEnv;
	// Test the monitored_user_exec section...
	{
		String cmd1("/bin/foo");
		String cmd2("/bin/bar");
		String cmd3("/bin/baz");
		String cmd4("/bin/cmd4");
		String bad_cmd("/bin/quux");
		String user("id");
		String ident("ego");
		String bad_user("occidare");
		String input(Format("\
monitored_user_exec                             \n\
{                                               \n\
  %3 @ %1 @ %2                                  \n\
  %4 @ %1 @ %2                                  \n\
  %5 @ %1 @ %2                                  \n\
  # Note the lack of /bin/quux                  \n\
  %6 @ %1 @ %2 allowed_environment_variables { }\n\
  %6 @ %1 @ %2 allowed_environment_variables { FOO }\n\
  %6 @ %1 @ %2 allowed_environment_variables { FOO = \"BAR\"}\n\
  %6 @ %1 @ %2 allowed_environment_variables { FOO = /a/dir }\n\
  %6 @ %1 @ %2 allowed_environment_variables { FOO FOO = \"BAR\" FOO = /a/dir }\n\
}                                                 \
",
				user, ident, cmd1, cmd2, cmd3, cmd4));
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeString(input, privileges, error));
		// All of the commands should match when executed as the correct user.
		unitAssert(privileges.monitored_user_exec.match(cmd1, emptyEnv, user, ident));
		unitAssert(privileges.monitored_user_exec.match(cmd2, emptyEnv, user, ident));
		unitAssert(privileges.monitored_user_exec.match(cmd3, emptyEnv, user, ident));
		unitAssert(!privileges.monitored_user_exec.match(bad_cmd, emptyEnv, user, ident));

		// All of the commands should fail to match when executed as an invalid (unspecified) user.
		unitAssert(!privileges.monitored_user_exec.match(cmd1, emptyEnv, bad_user, ident));
		unitAssert(!privileges.monitored_user_exec.match(cmd2, emptyEnv, bad_user, ident));
		unitAssert(!privileges.monitored_user_exec.match(cmd3, emptyEnv, bad_user, ident));
		unitAssert(!privileges.monitored_user_exec.match(bad_cmd, emptyEnv, bad_user, ident));
	}
	// Test the monitored_user_exec_check_args section
	{
		String cmd1("/bin/foo");
		String cmd2("/bin/foo a b c");
		String cmd3("/bin/foo #@! f g");
		String cmd4("/bin/foo *");
		String cmd5("/bin/foo * @ *");

		String bad_cmd1("/bin/quux");
		String bad_cmd2("/bin/foo x y z");
		String bad_cmd3("/bin/foo a b d");
		String bad_cmd4("/bin/foo @");
		String bad_cmd5("/bin/foo * *");
		String bad_cmd6("/bin/foo * *");
		String bad_cmd7("/bin/foo * @ @ *");
		String user("id");
		String ident("ego");
		String bad_user("occidare");
		String input(Format("\
monitored_user_exec_check_args	               \n\
{                                               \n\
  /bin/foo @ %1 @ %2                            \n\
  /bin/foo \"a\" \"b\" \"c\" @ %1 @ %2                      \n\
  # Allow a single argument to be anything      \n\
  /bin/foo * \"f\" \"g\" @ %1 @ %2                      \n\
  /bin/foo \"*\" @ %1 @ %2                        \n\
  /bin/foo \"*\" \"@\" \"*\" @ %1 @ %2                \n\
}                                                 \
", user, ident, cmd1, cmd2, cmd3));
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeString(input, privileges, error));

#define CHECKCOMMAND(cmd, user)														\
		privileges.monitored_user_exec_check_args.match(getExecutable(cmd), getArguments(cmd), emptyEnv, user, ident)

		// All of the commands should match when executed as the correct user.
		unitAssert(CHECKCOMMAND(cmd1, user));
		unitAssert(CHECKCOMMAND(cmd2, user));
		unitAssert(CHECKCOMMAND(cmd3, user));
		unitAssert(CHECKCOMMAND(cmd4, user));
		unitAssert(CHECKCOMMAND(cmd5, user));
		unitAssert(!CHECKCOMMAND(bad_cmd1, user));
		unitAssert(!CHECKCOMMAND(bad_cmd2, user));
		unitAssert(!CHECKCOMMAND(bad_cmd3, user));
		unitAssert(!CHECKCOMMAND(bad_cmd4, user));
		unitAssert(!CHECKCOMMAND(bad_cmd5, user));
		unitAssert(!CHECKCOMMAND(bad_cmd6, user));
		unitAssert(!CHECKCOMMAND(bad_cmd7, user));

		// All of the commands should fail to match when executed as an invalid (unspecified) user.
		unitAssert(!CHECKCOMMAND(cmd1, bad_user));
		unitAssert(!CHECKCOMMAND(cmd2, bad_user));
		unitAssert(!CHECKCOMMAND(cmd3, bad_user));
		unitAssert(!CHECKCOMMAND(cmd4, bad_user));
		unitAssert(!CHECKCOMMAND(cmd5, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd1, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd2, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd3, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd4, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd5, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd6, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd7, bad_user));
#undef CHECKCOMMAND

	}
}


void PrivilegeMonitorParserTestCases::parseValidMonitoredExec()
{
	StringArray emptyEnv;
	// Test the user_exec section...
	{
		String cmd1("/bin/foo");
		String cmd2("/bin/bar");
		String cmd3("/bin/baz");
		String bad_cmd("/bin/quux");
		String user("id");
		String ident("ego");
		String bad_user("occidare");
		String input(Format("\
monitored_exec                                  \n\
{                                               \n\
  %2 @ %1                                       \n\
  %3 @ %1                                       \n\
  %4 @ %1                                       \n\
  # Note the lack of /bin/quux                  \n\
}                                                 \
",
				user, cmd1, cmd2, cmd3));
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeString(input, privileges, error));
		// All of the commands should match when executed as the correct user.
		unitAssert(privileges.monitored_exec.match(cmd1, emptyEnv, user));
		unitAssert(privileges.monitored_exec.match(cmd2, emptyEnv, user));
		unitAssert(privileges.monitored_exec.match(cmd3, emptyEnv, user));
		unitAssert(!privileges.monitored_exec.match(bad_cmd, emptyEnv, user));

		// All of the commands should fail to match when executed as an invalid (unspecified) user.
		unitAssert(!privileges.monitored_exec.match(cmd1, emptyEnv, bad_user));
		unitAssert(!privileges.monitored_exec.match(cmd2, emptyEnv, bad_user));
		unitAssert(!privileges.monitored_exec.match(cmd3, emptyEnv, bad_user));
		unitAssert(!privileges.monitored_exec.match(bad_cmd, emptyEnv, bad_user));
	}
	// Test the monitored_exec_check_args section
	{
		String cmd1("/bin/foo");
		String cmd2("/bin/foo a b c");
		String cmd3("/bin/foo #@! f g");
		String cmd4("/bin/foo *");
		String cmd5("/bin/foo * @ *");

		String bad_cmd1("/bin/quux");
		String bad_cmd2("/bin/foo x y z");
		String bad_cmd3("/bin/foo a b d");
		String bad_cmd4("/bin/foo @");
		String bad_cmd5("/bin/foo * *");
		String bad_cmd6("/bin/foo * *");
		String bad_cmd7("/bin/foo * @ @ *");
		String user("id");
		String ident("ego");
		String bad_user("occidare");
		String input(Format("\
monitored_exec_check_args	                     \n\
{                                               \n\
  /bin/foo @ %1                                 \n\
  /bin/foo \"a\" \"b\" \"c\" @ %1                           \n\
  # Allow a single argument to be anything      \n\
  /bin/foo * \"f\" \"g\" @ %1                           \n\
  /bin/foo \"*\" @ %1                             \n\
  /bin/foo \"*\" \"@\" \"*\" @ %1                     \n\
}                                                 \
",
				user, cmd1, cmd2, cmd3));
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeString(input, privileges, error));

#define CHECKCOMMAND(cmd, user)														\
		privileges.monitored_exec_check_args.match(getExecutable(cmd), getArguments(cmd), emptyEnv, user)

		// All of the commands should match when executed as the correct user.
		unitAssert(CHECKCOMMAND(cmd1, user));
		unitAssert(CHECKCOMMAND(cmd2, user));
		unitAssert(CHECKCOMMAND(cmd3, user));
		unitAssert(CHECKCOMMAND(cmd4, user));
		unitAssert(CHECKCOMMAND(cmd5, user));
		unitAssert(!CHECKCOMMAND(bad_cmd1, user));
		unitAssert(!CHECKCOMMAND(bad_cmd2, user));
		unitAssert(!CHECKCOMMAND(bad_cmd3, user));
		unitAssert(!CHECKCOMMAND(bad_cmd4, user));
		unitAssert(!CHECKCOMMAND(bad_cmd5, user));
		unitAssert(!CHECKCOMMAND(bad_cmd6, user));
		unitAssert(!CHECKCOMMAND(bad_cmd7, user));

		// All of the commands should fail to match when executed as an invalid (unspecified) user.
		unitAssert(!CHECKCOMMAND(cmd1, bad_user));
		unitAssert(!CHECKCOMMAND(cmd2, bad_user));
		unitAssert(!CHECKCOMMAND(cmd3, bad_user));
		unitAssert(!CHECKCOMMAND(cmd4, bad_user));
		unitAssert(!CHECKCOMMAND(cmd5, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd1, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd2, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd3, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd4, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd5, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd6, bad_user));
		unitAssert(!CHECKCOMMAND(bad_cmd7, bad_user));
#undef CHECKCOMMAND
	}

}

void
PrivilegeMonitorParserTestCases::parseInclude()
{
	StringArray emptyEnv;
	{	// include at the end
		String cmd1("/bin/foo");
		String cmd2("/bin/bar");
		String user("id");
		String input1(Format("\
user_exec                                       \n\
{                                               \n\
  %2 @ %1                                       \n\
}                                               \n\
include { \"input2\" }                          \n\
",
				user, cmd1));
		String input2(Format("\
user_exec                                       \n\
{                                               \n\
  %2 @ %1                                       \n\
}                                                 \
",
				user, cmd2));
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeStringInclude(input1, input2, privileges, error));
		unitAssert(privileges.user_exec.match(cmd1, emptyEnv, user));
		unitAssert(privileges.user_exec.match(cmd2, emptyEnv, user));
	}

	{	// include at the beginning
		String cmd1("/bin/foo");
		String cmd2("/bin/bar");
		String user("id");
		String input1(Format("\
include { \"input2\" }                          \n\
user_exec                                       \n\
{                                               \n\
  %2 @ %1                                       \n\
}                                               \n\
",
				user, cmd1));
		String input2(Format("\
user_exec                                       \n\
{                                               \n\
  %2 @ %1                                       \n\
}                                                 \
",
				user, cmd2));
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeStringInclude(input1, input2, privileges, error));
		unitAssert(privileges.user_exec.match(cmd1, emptyEnv, user));
		unitAssert(privileges.user_exec.match(cmd2, emptyEnv, user));
	}

	{	// include in the middle
		String cmd1("/bin/foo");
		String cmd2("/bin/bar");
		String cmd3("/bin/baz");
		String user("id");
		String input1(Format("\
user_exec                                       \n\
{                                               \n\
  %2 @ %1                                       \n\
}                                               \n\
include { \"input2\" }                          \n\
user_exec                                       \n\
{                                               \n\
  %3 @ %1                                       \n\
}                                               \n\
",
				user, cmd1, cmd3));
		String input2(Format("\
user_exec                                       \n\
{                                               \n\
  %2 @ %1                                       \n\
}                                                 \
",
				user, cmd2));
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeStringInclude(input1, input2, privileges, error));
		unitAssert(privileges.user_exec.match(cmd1, emptyEnv, user));
		unitAssert(privileges.user_exec.match(cmd2, emptyEnv, user));
		unitAssert(privileges.user_exec.match(cmd3, emptyEnv, user));
	}

	{	// recursive include
		String cmd1("/bin/foo");
		String cmd2("/bin/bar");
		String user("id");
		String input1(Format("\
include { \"input2\" }                          \n\
user_exec                                       \n\
{                                               \n\
  %2 @ %1                                       \n\
}                                               \n\
",
				user, cmd1));
		String input2(Format("\
include { \"" FIRST_PRIV_BUFFER_NAME "\" }      \n\
user_exec                                       \n\
{                                               \n\
  %2 @ %1                                       \n\
}                                                 \
",
				user, cmd2));
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(!parsePrivilegeStringInclude(input1, input2, privileges, error));
	}
	{	// multiple include
		String cmd1("/bin/foo");
		String cmdbar1("/bin/bar1");
		String cmdbar2("/bin/bar2");
		String cmdbar3("/bin/bar3");
		String user("id");
		String input1(Format("\
include { \"input2\" \"input3\" \"input4\" }    \n\
user_exec                                       \n\
{                                               \n\
  %2 @ %1                                       \n\
}                                               \n\
",
				user, cmd1));
		String input2(Format("\
user_exec                                       \n\
{                                               \n\
  %2 @ %1                                       \n\
}                                                 \
",
				user, cmdbar1));
		String input3(Format("\
user_exec                                       \n\
{                                               \n\
  %2 @ %1                                       \n\
}                                                 \
",
				user, cmdbar2));
		String input4(Format("\
user_exec                                       \n\
{                                               \n\
  %2 @ %1                                       \n\
}                                                 \
",
				user, cmdbar3));
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		StringArray names(1, "input2");
		StringArray includes(1, input2);
		names.push_back("input3");
		includes.push_back(input3);
		names.push_back("input4");
		includes.push_back(input4);
		TestIncludeHandler tih(names, includes);
		unitAssert(parsePrivilegeStringInclude(input1, tih, privileges, error));
		unitAssert(privileges.user_exec.match(cmd1, emptyEnv, user));
		unitAssert(privileges.user_exec.match(cmdbar1, emptyEnv, user));
		unitAssert(privileges.user_exec.match(cmdbar2, emptyEnv, user));
		unitAssert(privileges.user_exec.match(cmdbar3, emptyEnv, user));
	}
}

void PrivilegeMonitorParserTestCases::testAllowedEnvironmentVariables()
{
	StringArray emptyEnv;
	StringArray badEnv(String("BAD=bad").tokenize());
	StringArray fooEnv(String("FOO=BAR").tokenize());
	StringArray fooPathEnv(String("FOO=/a/dir").tokenize());
	StringArray foo123Env(String("FOO1=FLUMP FOO2=BAR FOO3=/a/dir").tokenize());
	String cmd1("/bin/foo");
	StringArray noArgs(cmd1.tokenize());
	String user("id");
	String ident("ego");
	String input(Format("\
monitored_user_exec\n\
{\n\
  %3 @ %1 @ %2 allowed_environment_variables { }\n\
}\n\
monitored_exec\n\
{\n\
  %3 @ %2 allowed_environment_variables { FOO }\n\
}\n\
user_exec\n\
{\n\
  %3 @ %1 allowed_environment_variables { FOO = \"BAR\"}\n\
}\n\
monitored_exec_check_args\n\
{\n\
  %3 @ %1 allowed_environment_variables { FOO = /a/dir }\n\
}\n\
monitored_user_exec_check_args\n\
{\n\
  %3 @ %2 @ %1 allowed_environment_variables { FOO FOO = \"BAR\" FOO = /a/dir }\n\
}\n\
user_exec_check_args\n\
{\n\
  %3 @ %1 allowed_environment_variables { FOO1 FOO2 = \"BAR\" FOO3 = /a/dir }\n\
}\n\
",
			user, ident, cmd1));
	OpenWBEM::PrivilegeConfig::Privileges privileges;
	OpenWBEM::PrivilegeConfig::ParseError error;
	unitAssert(parsePrivilegeString(input, privileges, error));
	unitAssert(privileges.monitored_user_exec.match(cmd1, emptyEnv, user, ident));
	unitAssert(!privileges.monitored_user_exec.match(cmd1, badEnv, user, ident));
	unitAssert(privileges.monitored_exec.match(cmd1, emptyEnv, ident));
	unitAssert(privileges.monitored_exec.match(cmd1, fooEnv, ident));
	unitAssert(!privileges.monitored_exec.match(cmd1, badEnv, ident));
	unitAssert(privileges.user_exec.match(cmd1, emptyEnv, user));
	unitAssert(privileges.user_exec.match(cmd1, fooEnv, user));
	unitAssert(!privileges.user_exec.match(cmd1, badEnv, user));
	unitAssert(privileges.monitored_exec_check_args.match(cmd1, noArgs, emptyEnv, user));
	unitAssert(privileges.monitored_exec_check_args.match(cmd1, noArgs, fooPathEnv, user));
	unitAssert(!privileges.monitored_exec_check_args.match(cmd1, noArgs, badEnv, user));
	unitAssert(privileges.monitored_user_exec_check_args.match(cmd1, noArgs, emptyEnv, ident, user));
	unitAssert(privileges.monitored_user_exec_check_args.match(cmd1, noArgs, fooEnv, ident, user));
	unitAssert(privileges.monitored_user_exec_check_args.match(cmd1, noArgs, fooPathEnv, ident, user));
	unitAssert(!privileges.monitored_user_exec_check_args.match(cmd1, noArgs, badEnv, ident, user));
	unitAssert(privileges.user_exec_check_args.match(cmd1, noArgs, emptyEnv, user));
	unitAssert(privileges.user_exec_check_args.match(cmd1, noArgs, foo123Env, user));
	unitAssert(!privileges.user_exec_check_args.match(cmd1, noArgs, badEnv, user));
}

void PrivilegeMonitorParserTestCases::testPathPatterns()
{
	using namespace PrivilegeConfig;
	{
		PathPatterns pp;
		pp.addPattern("/foo");
		unitAssert(pp.match("/foo"));
		unitAssert(!pp.match("foo"));
		unitAssert(!pp.match("/foo/"));
		unitAssert(!pp.match("/bar"));
		unitAssert(!pp.match("/foo/."));
		unitAssert(!pp.match("/foo/.."));
	}
	{
		PathPatterns pp;
		pp.addPattern("/");
		unitAssert(pp.match("/"));
		unitAssert(!pp.match("/bar"));
	}
}

void PrivilegeMonitorParserTestCases::parseValidOpenR()
{
	StringArray emptyEnv;
	// Test the open_r section...
	{
		String input("\
open_r                                          \n\
{                                               \n\
  /**                                           \n\
}                                                 \
");
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeString(input, privileges, error));
		unitAssert(privileges.open_read.match("/"));
		unitAssert(privileges.open_read.match("/bin"));
		unitAssert(privileges.open_read.match("/bin/sh"));
	}
	// Test the open_r section...
	{
		String input("\
open_r                                          \n\
{                                               \n\
  /var/opt/foo/**                               \n\
}                                                 \
");
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeString(input, privileges, error));
		unitAssert(!privileges.open_read.match("/"));
		unitAssert(!privileges.open_read.match("/var"));
		unitAssert(!privileges.open_read.match("/var/opt"));
		unitAssert(!privileges.open_read.match("/var/opt/foo"));
		unitAssert(privileges.open_read.match("/var/opt/foo/bar"));
		unitAssert(privileges.open_read.match("/var/opt/foo/bar/baz"));
	}
}


void PrivilegeMonitorParserTestCases::parseValidStat()
{
	// Test the stat section...
	{
		String input("\
stat                                          \n\
{                                               \n\
  /**                                           \n\
}                                                 \
");
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeString(input, privileges, error));
		unitAssert(privileges.stat.match("/"));
		unitAssert(privileges.stat.match("/bin"));
		unitAssert(privileges.stat.match("/bin/sh"));
	}
	// Test the stat section...
	{
		String input("stat { /var/opt/foo/** }");
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeString(input, privileges, error));
		unitAssert(!privileges.stat.match("/"));
		unitAssert(!privileges.stat.match("/var"));
		unitAssert(!privileges.stat.match("/var/opt"));
		unitAssert(!privileges.stat.match("/var/opt/foo"));
		unitAssert(privileges.stat.match("/var/opt/foo/bar"));
		unitAssert(privileges.stat.match("/var/opt/foo/bar/baz"));

		// Non-matching paths because they are unacceptable as input.
		unitAssert(!privileges.stat.match("/var/opt/foo/bar/"));
		unitAssert(!privileges.stat.match("/var/opt/foo/bar/."));

		// Non-matching because they contain ".."
		unitAssert(!privileges.stat.match("/var/opt/foo/bar/.."));
		unitAssert(!privileges.stat.match("/var/opt/bar/../foo/bar/baz"));
		unitAssert(!privileges.stat.match("/var/opt/foo/././../././bar/baz"));
	}
}

void PrivilegeMonitorParserTestCases::parseValidRemoveDir()
{
	{
		String input("\
remove_dir                                          \n\
{                                               \n\
  /**                                           \n\
}                                                 \
");
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeString(input, privileges, error));
		unitAssert(privileges.remove_dir.match("/"));
		unitAssert(privileges.remove_dir.match("/foo"));
		unitAssert(privileges.remove_dir.match("/foo/bar"));
	}
	{
		String input("remove_dir { /var/opt/foo/** }");
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeString(input, privileges, error));
		unitAssert(!privileges.remove_dir.match("/"));
		unitAssert(!privileges.remove_dir.match("/var"));
		unitAssert(!privileges.remove_dir.match("/var/opt"));
		unitAssert(privileges.remove_dir.match("/var/opt/foo/bar"));
		unitAssert(privileges.remove_dir.match("/var/opt/foo/bar/baz"));

		// These have the trailing dots removed and remain valid.
		unitAssert(privileges.remove_dir.match("/var/opt/foo/bar/."));

		// ".." will always be rejected.
		unitAssert(!privileges.remove_dir.match("/var/opt/foo/bar/.."));
		unitAssert(!privileges.remove_dir.match("/var/opt/bar/../foo/bar/baz"));
		unitAssert(!privileges.remove_dir.match("/var/opt/foo/././../././bar/baz"));
	}
}


Test* PrivilegeMonitorParserTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("PrivilegeMonitorParser");

	ADD_TEST_TO_SUITE(PrivilegeMonitorParserTestCases, parseEmpty);
	ADD_TEST_TO_SUITE(PrivilegeMonitorParserTestCases, parseInvalid);
	ADD_TEST_TO_SUITE(PrivilegeMonitorParserTestCases, parseValidSimple);
	ADD_TEST_TO_SUITE(PrivilegeMonitorParserTestCases, parseValidUserExec);
	ADD_TEST_TO_SUITE(PrivilegeMonitorParserTestCases, parseValidMonitoredExec);
	ADD_TEST_TO_SUITE(PrivilegeMonitorParserTestCases, parseValidMonitoredUserExec);
	ADD_TEST_TO_SUITE(PrivilegeMonitorParserTestCases, parseInclude);
	ADD_TEST_TO_SUITE(PrivilegeMonitorParserTestCases, testAllowedEnvironmentVariables);
	ADD_TEST_TO_SUITE(PrivilegeMonitorParserTestCases, testPathPatterns);
	ADD_TEST_TO_SUITE(PrivilegeMonitorParserTestCases, parseValidOpenR);
	ADD_TEST_TO_SUITE(PrivilegeMonitorParserTestCases, parseValidStat);
	ADD_TEST_TO_SUITE(PrivilegeMonitorParserTestCases, parseValidRemoveDir);

	return testSuite;
}

