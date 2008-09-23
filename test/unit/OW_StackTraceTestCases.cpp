/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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

/**
 * @author Dan Nuffer
 */


#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_StackTraceTestCases.hpp"

AUTO_UNIT_TEST_SUITE_NAMED(OW_StackTraceTestCases,"OW_StackTrace");

#include "OW_StackTrace.hpp"
#include "OW_ConfigOpts.hpp"
#include <fstream>

using std::ifstream;

using namespace OpenWBEM;

void OW_StackTraceTestCases::setUp()
{
}

void OW_StackTraceTestCases::tearDown()
{
}

void OW_StackTraceTestCases::testGetStackTrace()
{
	if (getenv("OW_STACKTRACE"))
	{
		ifstream file("/usr/bin/gdb");
		if (file)
		{
			file.close();
			/*
			OW_StackTrace* stackTrace = OW_StackTrace::getStackTrace();
			unitAssert(stackTrace);
			delete stackTrace;
			*/
		}
	}
}

Test* OW_StackTraceTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_StackTrace");

#ifdef OW_ENABLE_STACK_TRACE_ON_EXCEPTIONS

	testSuite->addTest (new TestCaller <OW_StackTraceTestCases>
			("testGetStackTrace",
			&OW_StackTraceTestCases::testGetStackTrace));
#endif

	return testSuite;
}

