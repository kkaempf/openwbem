/*******************************************************************************
* Copyright (C) 2005 Quest Software, Inc. All rights reserved.
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
#include "ProcessTestCases.hpp"
AUTO_UNIT_TEST_SUITE_NAMED(ProcessTestCases,"Process");
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_Format.hpp"
#include "OW_Process.hpp"
#include "OW_FileSystem.hpp"
#include "OW_Timeout.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_Exec.hpp"
#include "OW_Types.hpp"

#include <iostream>

using namespace OpenWBEM;

void ProcessTestCases::setUp()
{
}

void ProcessTestCases::tearDown()
{
}

void ProcessTestCases::testSubprocessBlockingOutput()
{
	String executablePath = Format("%1%2%3",  FileSystem::Path::getCurrentWorkingDirectory(), OW_FILENAME_SEPARATOR, "fillOutputBuffer");
	String commandTemplate = Format("%1 %%1", executablePath);

	// Non-blocking (because there is no output)
	ProcessRef noc = Exec::spawn(Format(commandTemplate.c_str(), 0).toString().tokenize());
	unitAssertNoThrow( noc->waitCloseTerm(Timeout::relative(5.0), Timeout::relative(10.0), Timeout::relative(15.0)) );
	unitAssert( noc->processStatus().terminatedSuccessfully() );

	// Blocking output possible (likely due to large amounts of output)
	const size_t bytesToRead = 32768;
	ProcessRef hoc = Exec::spawn(Format(commandTemplate.c_str(), bytesToRead).toString().tokenize());
	// This should be done in a separate thread.  If things are horribly broken,
	// this will block on both ends and the tests will never complete.
	String output = hoc->out()->readAll();
	unitAssert( bytesToRead == output.length() );
	unitAssertNoThrow( hoc->waitCloseTerm(Timeout::relative(5.0),Timeout::relative(10.0),Timeout::relative(15.0)) );
	unitAssert( hoc->processStatus().terminatedSuccessfully() );
}

void ProcessTestCases::testSpawn()
{
	ProcessRef rval = Exec::spawn( String("/bin/cat").tokenize() );
	rval->in()->write("hello world\n", 12);
	rval->in()->close();
	String out = rval->out()->readAll();
	rval->waitCloseTerm();
	unitAssert(rval->processStatus().terminatedSuccessfully());
	unitAssert(out == "hello world\n");

	unitAssertThrows(Exec::spawn(String("/a/non-existent/binary").tokenize()));
}


Test* ProcessTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("Process");

	ADD_TEST_TO_SUITE(ProcessTestCases, testSubprocessBlockingOutput);
	ADD_TEST_TO_SUITE(ProcessTestCases, testSpawn);

	return testSuite;
}

