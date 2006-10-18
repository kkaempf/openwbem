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
  unlink { }

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
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "PrivilegeMonitorParserTestCases.hpp"

#include "OW_PrivilegeConfig.hpp"
#include "OW_StringStream.hpp"
#include "OW_Format.hpp"

using namespace OpenWBEM;

void PrivilegeMonitorParserTestCases::setUp()
{
}

void PrivilegeMonitorParserTestCases::tearDown()
{
}

namespace // anonymous
{
	//
	// Helper functions for parsing privilege strings
	//
	bool parsePrivilegeStream(std::istream& s, OpenWBEM::PrivilegeConfig::Privileges& privileges, OpenWBEM::PrivilegeConfig::ParseError error)
	{
		openwbem_privconfig_Lexer lex(s);
		return openwbem_privconfig_parse(&privileges, &error, &lex) == 0;
	}

	bool parsePrivilegeString(const String& s, OpenWBEM::PrivilegeConfig::Privileges& privileges, OpenWBEM::PrivilegeConfig::ParseError error)
	{
		IStringStream str(s);
		return parsePrivilegeStream(str, privileges, error);
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
		StringArray foo = var.tokenize();
		if( !foo.empty() )
		{
			foo.erase(foo.begin());
		}
		return foo;
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
check_path { }\n\
rename_from { }\n\
rename_to { }\n\
rename_from_to { }\n\
unlink { }\n\
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
		unitAssert(privileges.user_exec.match(cmd1, user));
		unitAssert(privileges.user_exec.match(cmd2, user));
		unitAssert(privileges.user_exec.match(cmd3, user));
		unitAssert(!privileges.user_exec.match(bad_cmd, user));

		// All of the commands should fail to match when executed as an invalid (unspecified) user.
		unitAssert(!privileges.user_exec.match(cmd1, bad_user));
		unitAssert(!privileges.user_exec.match(cmd2, bad_user));
		unitAssert(!privileges.user_exec.match(cmd3, bad_user));
		unitAssert(!privileges.user_exec.match(bad_cmd, bad_user));
	}
	// Test the user_exec_check_args section
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
		String bad_user("occidare");
		String input(Format("\
user_exec_check_args	                           \n\
{                                               \n\
  /bin/foo @ %1                                 \n\
  /bin/foo a b c @ %1                           \n\
  # Allow a single argument to be anything      \n\
  /bin/foo * f g @ %1                           \n\
  /bin/foo \\* @ %1                             \n\
  /bin/foo \\* \\@ \\* @ %1                     \n\
}                                                 \
", user, cmd1, cmd2, cmd3));
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeString(input, privileges, error));

#define CHECKCOMMAND(cmd, user)														\
		privileges.user_exec_check_args.match(getExecutable(cmd), getArguments(cmd), user)

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


void PrivilegeMonitorParserTestCases::parseValidMonitoredUserExec()
{
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
monitored_user_exec                             \n\
{                                               \n\
  %3 @ %1 @ %2                                  \n\
  %4 @ %1 @ %2                                  \n\
  %5 @ %1 @ %2                                  \n\
  # Note the lack of /bin/quux                  \n\
}                                                 \
",
				user, ident, cmd1, cmd2, cmd3));
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeString(input, privileges, error));
		// All of the commands should match when executed as the correct user.
		unitAssert(privileges.monitored_user_exec.match(cmd1, user, ident));
		unitAssert(privileges.monitored_user_exec.match(cmd2, user, ident));
		unitAssert(privileges.monitored_user_exec.match(cmd3, user, ident));
		unitAssert(!privileges.monitored_user_exec.match(bad_cmd, user, ident));

		// All of the commands should fail to match when executed as an invalid (unspecified) user.
		unitAssert(!privileges.monitored_user_exec.match(cmd1, bad_user, ident));
		unitAssert(!privileges.monitored_user_exec.match(cmd2, bad_user, ident));
		unitAssert(!privileges.monitored_user_exec.match(cmd3, bad_user, ident));
		unitAssert(!privileges.monitored_user_exec.match(bad_cmd, bad_user, ident));
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
  /bin/foo a b c @ %1 @ %2                      \n\
  # Allow a single argument to be anything      \n\
  /bin/foo * f g @ %1 @ %2                      \n\
  /bin/foo \\* @ %1 @ %2                        \n\
  /bin/foo \\* \\@ \\* @ %1 @ %2                \n\
}                                                 \
", user, ident, cmd1, cmd2, cmd3));
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeString(input, privileges, error));

#define CHECKCOMMAND(cmd, user)														\
		privileges.monitored_user_exec_check_args.match(getExecutable(cmd), getArguments(cmd), user, ident)

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
		unitAssert(privileges.monitored_exec.match(cmd1, user));
		unitAssert(privileges.monitored_exec.match(cmd2, user));
		unitAssert(privileges.monitored_exec.match(cmd3, user));
		unitAssert(!privileges.monitored_exec.match(bad_cmd, user));

		// All of the commands should fail to match when executed as an invalid (unspecified) user.
		unitAssert(!privileges.monitored_exec.match(cmd1, bad_user));
		unitAssert(!privileges.monitored_exec.match(cmd2, bad_user));
		unitAssert(!privileges.monitored_exec.match(cmd3, bad_user));
		unitAssert(!privileges.monitored_exec.match(bad_cmd, bad_user));
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
  /bin/foo a b c @ %1                           \n\
  # Allow a single argument to be anything      \n\
  /bin/foo * f g @ %1                           \n\
  /bin/foo \\* @ %1                             \n\
  /bin/foo \\* \\@ \\* @ %1                     \n\
}                                                 \
",
				user, cmd1, cmd2, cmd3));
		OpenWBEM::PrivilegeConfig::Privileges privileges;
		OpenWBEM::PrivilegeConfig::ParseError error;
		unitAssert(parsePrivilegeString(input, privileges, error));

#define CHECKCOMMAND(cmd, user)														\
		privileges.monitored_exec_check_args.match(getExecutable(cmd), getArguments(cmd), user)

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


Test* PrivilegeMonitorParserTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("PrivilegeMonitorParser");

	ADD_TEST_TO_SUITE(PrivilegeMonitorParserTestCases, parseEmpty);
	ADD_TEST_TO_SUITE(PrivilegeMonitorParserTestCases, parseInvalid);
	ADD_TEST_TO_SUITE(PrivilegeMonitorParserTestCases, parseValidSimple);
	ADD_TEST_TO_SUITE(PrivilegeMonitorParserTestCases, parseValidUserExec);
	ADD_TEST_TO_SUITE(PrivilegeMonitorParserTestCases, parseValidMonitoredExec);
	ADD_TEST_TO_SUITE(PrivilegeMonitorParserTestCases, parseValidMonitoredUserExec);

	return testSuite;
}
