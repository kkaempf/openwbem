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

#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_ExecTestCases.hpp"
#include "OW_Exec.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_Array.hpp"

#if defined(OW_HAVE_SYS_WAIT_H) && defined(OW_WIFEXITED_NEEDS_WAIT_H)
#include <sys/wait.h>
#endif

using namespace OpenWBEM;

void OW_ExecTestCases::setUp()
{
}

void OW_ExecTestCases::tearDown()
{
}

void OW_ExecTestCases::testSafePopen()
{
	Array<String> command;
	command.push_back("/bin/cat");
	PopenStreams rval = Exec::safePopen( command );
	rval.in()->write("hello world\n", 12);
	rval.in()->close();
	String out = rval.out()->readAll();
	rval.getExitStatus();
	unitAssert(out == "hello world\n");

	PopenStreams rval2 = Exec::safePopen( command, "hello to world\n" );
	rval2.in()->close();
	out = rval2.out()->readAll();
	rval2.getExitStatus();
	unitAssert( out == "hello to world\n" );
}

void OW_ExecTestCases::testExecuteProcessAndGatherOutput()
{
	String output;
	int processstatus(0);
	Exec::executeProcessAndGatherOutput(String(OW_TRUE_PATHNAME).tokenize(), output, processstatus);
	unitAssert(output.empty());
	unitAssert(WIFEXITED(processstatus));
	unitAssert(WEXITSTATUS(processstatus) == 0);

	processstatus = 0;
	output.erase();
	Exec::executeProcessAndGatherOutput(String(OW_FALSE_PATHNAME).tokenize(), output, processstatus);
	unitAssert(output.empty());
	unitAssert(WIFEXITED(processstatus));
	unitAssert(WEXITSTATUS(processstatus) == 1);

	processstatus = 0;
	output.erase();
	Exec::executeProcessAndGatherOutput(String("/bin/echo false").tokenize(), output, processstatus);
	StringArray out_array = output.tokenize();
	unitAssert(out_array.size() == 1)
	unitAssert(*out_array.begin() == "false");
	unitAssert(WIFEXITED(processstatus));
	unitAssert(WEXITSTATUS(processstatus) == 0);

	// do a timeout
	processstatus = 0;
	output.erase();
	try
	{
		StringArray cmd;
		cmd.push_back("/bin/sh");
		cmd.push_back("-c");
		
		// We want a delay in this test before any output occurs.  This is
		// important because on some platforms (like Linux), the output could be
		// received even if the timeout was set as 0.  With a sleep at the
		// beginning of the test, this should hopefully avoid any scheduling
		// issues with a child process going too fast on an unloaded machine.

		// The sequence should go like this:
		// 1. sleep 1 - test blocks
		// 2. echo before - test gets it then resets timeout to 2 seconds.
		// 3. sleep 3 starts
		// 4. test times out after 2 seconds and throws.
		cmd.push_back("sleep 1; echo before; sleep 3; echo after");
		Exec::executeProcessAndGatherOutput(cmd, output, processstatus, 2);
		unitAssert(0);
	}
	catch (const ExecTimeoutException& e)
	{
	}
	unitAssert(output == "before\n");

	// test output limit
	processstatus = 0;
	output.erase();
	try
	{
		Exec::executeProcessAndGatherOutput(String("/bin/echo 12345").tokenize(), output, processstatus, -1, 4);
		unitAssert(0);
	}
	catch (const ExecBufferFullException& e)
	{
	}
	unitAssert(output == "1234");

	// test both
	//runtest("for x in `seq 10`; do echo $x; sleep $x; done", 5, 100);
	//runtest("for x in `seq 10`; do echo $x; sleep $x; done", 100, 5);

}

Test* OW_ExecTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_Exec");

	testSuite->addTest (new TestCaller <OW_ExecTestCases> 
			("testSafePopen", 
			&OW_ExecTestCases::testSafePopen));

	testSuite->addTest (new TestCaller <OW_ExecTestCases> 
			("testExecuteProcessAndGatherOutput", 
			&OW_ExecTestCases::testExecuteProcessAndGatherOutput));
	return testSuite;
}

