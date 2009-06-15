/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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

#include "OW_config.h"
#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "EnvironmentVariablePatternsTestCases.hpp"
AUTO_UNIT_TEST_SUITE_NAMED(EnvironmentVariablePatternsTestCases,"EnvironmentVariablePatterns");
#include "OW_PrivilegeConfig.hpp"
#include "blocxx/Secure.hpp"
#include "blocxx/Environ.hpp"

using namespace OpenWBEM;
using namespace OpenWBEM::PrivilegeConfig;

void EnvironmentVariablePatternsTestCases::setUp()
{
}

void EnvironmentVariablePatternsTestCases::tearDown()
{
}

void EnvironmentVariablePatternsTestCases::testaddPattern()
{
	EnvironmentVariablePatterns evp;
	unitAssertNoThrow(evp.addPattern("FOO1", 0, EnvironmentVariablePatterns::E_ANYTHING_PATTERN));
	unitAssertNoThrow(evp.addPattern("FOO2", "BAR", EnvironmentVariablePatterns::E_LITERAL_PATTERN));
	unitAssertNoThrow(evp.addPattern("FOO3", "/a/b/c", EnvironmentVariablePatterns::E_PATH_PATTERN));
}

void EnvironmentVariablePatternsTestCases::testmatch()
{
	EnvironmentVariablePatterns evp;
	unitAssertNoThrow(evp.addPattern("FOO1", 0, EnvironmentVariablePatterns::E_ANYTHING_PATTERN));
	unitAssertNoThrow(evp.addPattern("FOO2", 0, EnvironmentVariablePatterns::E_ANYTHING_PATTERN));
	StringArray env = String("FOO1=x").tokenize();
	unitAssert(evp.match(env));
	StringArray badenv = String("BAR=y").tokenize();
	unitAssert(!evp.match(badenv));
	badenv = String("BAR=").tokenize();
	unitAssert(!evp.match(badenv));
	badenv = String("BAR").tokenize();
	unitAssert(!evp.match(badenv));
	badenv = String("FOO1").tokenize();
	unitAssert(!evp.match(badenv));
	env = String("FOO1=").tokenize();
	unitAssert(evp.match(env));

	// test multiple vars
	env = String("FOO1=x FOO2=y").tokenize();
	unitAssert(evp.match(env));
	badenv = String("FOO1=x FOO3=y").tokenize();
	unitAssert(!evp.match(badenv));
	badenv = String("BAD=y").tokenize();
	unitAssert(!evp.match(badenv));

	// test E_LITERAL_PATTERN
	unitAssertNoThrow(evp.addPattern("LIT", "abc", EnvironmentVariablePatterns::E_LITERAL_PATTERN));
	env = String("FOO1=abc LIT=abc").tokenize();
	unitAssert(evp.match(env));
	badenv = String("LIT=bad").tokenize();
	unitAssert(!evp.match(badenv));
	unitAssertNoThrow(evp.addPattern("LIT", "def", EnvironmentVariablePatterns::E_LITERAL_PATTERN));
	env = String("LIT=abc").tokenize();
	unitAssert(evp.match(env));
	env = String("LIT=def").tokenize();
	unitAssert(evp.match(env));
	badenv = String("LIT=xyz").tokenize();
	unitAssert(!evp.match(badenv));

	// test E_PATH_PATTERN
	unitAssertNoThrow(evp.addPattern("PAT", "/a/dir/*", EnvironmentVariablePatterns::E_PATH_PATTERN));
	env = String("PAT=/a/dir/foo").tokenize();
	unitAssert(evp.match(env));
	badenv = String("PAT=/a/nother/dir").tokenize();
	unitAssert(!evp.match(badenv));

	// test Secure::minimalEnvironment()
	unitAssert(evp.match(Secure::minimalEnvironment()));

	// Test the current environment.
	StringArray currEnv;
	for (char** p = environ; *p; ++p)
	{
		currEnv.push_back(*p);
	}
	unitAssert(evp.match(currEnv));
}

Test* EnvironmentVariablePatternsTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("EnvironmentVariablePatterns");

	ADD_TEST_TO_SUITE(EnvironmentVariablePatternsTestCases, testaddPattern);
	ADD_TEST_TO_SUITE(EnvironmentVariablePatternsTestCases, testmatch);

	return testSuite;
}

