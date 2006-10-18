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
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "PrivilegeConfigTestCases.hpp"
#include "OW_PrivilegeConfig.hpp"

using namespace OpenWBEM;

void PrivilegeConfigTestCases::setUp()
{
}

void PrivilegeConfigTestCases::tearDown()
{
}

void PrivilegeConfigTestCases::testunescapeString()
{
	unitAssert( PrivilegeConfig::unescapeString("\\a\\b\\f\\n\\r\\t\\v\\'\\\"\\?\\\\") == "\a\b\f\n\r\t\v\'\"\?\\" );
	unitAssert( PrivilegeConfig::unescapeString("\\x0\\xA\\x1\\x0F\\x7F") == "\x0\xA\x1\x0F\x7F" );
	unitAssert( PrivilegeConfig::unescapeString("\\0\\7\\1\\02\\003\\43\\165") == "\0\7\1\02\003\43\165" );
	unitAssert( PrivilegeConfig::unescapeString("only one escape\\n") == "only one escape\n" );

	// test too large hex
	unitAssertThrows(PrivilegeConfig::unescapeString("\\xFF"));
	// test too large oct
	unitAssertThrows(PrivilegeConfig::unescapeString("\\777"));
}

Test* PrivilegeConfigTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("PrivilegeConfig");

	ADD_TEST_TO_SUITE(PrivilegeConfigTestCases, testunescapeString);

	return testSuite;
}

