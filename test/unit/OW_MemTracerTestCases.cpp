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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */


#include "OW_config.h"
#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_MemTracerTestCases.hpp"

AUTO_UNIT_TEST_SUITE_NAMED(OW_MemTracerTestCases,"OW_MemTracer");

#include <iostream>

#include "OW_Exec.hpp"
#include "OW_Array.hpp"
#include "OW_UnnamedPipe.hpp"

using namespace OpenWBEM;

void OW_MemTracerTestCases::setUp()
{
}

void OW_MemTracerTestCases::tearDown()
{
}

void OW_MemTracerTestCases::testSomething()
{
#ifdef OW_DEBUG_MEMORY
	enum codes
	{
		UNDERRUN=1,
		OVERRUN=2,
		UNKNOWN_ADDR=3,
		DOUBLE_DELETE=4,
		DOUBLE_DELETE_NOFREE=5,
		AGGRESSIVE=7
	};


	String execName = "./MemTracerTest";

	bool wasDisabled = false;
	if (getenv("OW_MEM_DISABLE") && getenv("OW_MEM_DISABLE")[0] == '1')
	{
		wasDisabled = true;
		putenv("OW_MEM_DISABLE=0");
	}

	putenv("OW_MEM_AGGRESSIVE=0");
	putenv("OW_MEM_NOFREE=0");

	Array<String> cmd;
	cmd.append(execName);
	cmd.append(String(UNDERRUN));
	ProcessRef rval = Exec::spawn(cmd);
	String er = rval->err()->readAll();
	rval->waitCloseTerm();
	size_t idx = er.indexOf("UNDERRUN");
	unitAssert(idx != String::npos);

	cmd.clear();
	cmd.append(execName);
	cmd.append(String(OVERRUN));
	rval = Exec::spawn(cmd);
	er = rval->err()->readAll();
	rval->waitCloseTerm();
	idx = er.indexOf("OVERRUN");
	unitAssert(idx != String::npos);

	cmd.clear();
	cmd.append(execName);
	cmd.append(String(UNKNOWN_ADDR));
	rval = Exec::spawn(cmd);
	er = rval->err()->readAll();
	rval->waitCloseTerm();
	idx = er.indexOf("UNKNOWN ADDRESS");
	unitAssert(idx != String::npos);

	cmd.clear();
	cmd.append(execName);
	cmd.append(String(DOUBLE_DELETE));
	rval = Exec::spawn(cmd);
	er = rval->err()->readAll();
	rval->waitCloseTerm();
	idx = er.indexOf("DOUBLE DELETE");
	unitAssert(idx != String::npos);

	putenv("OW_MEM_NOFREE=1");

	cmd.clear();
	cmd.append(execName);
	cmd.append(String(DOUBLE_DELETE));
	rval = Exec::spawn(cmd);
	er = rval->err()->readAll();
	rval->waitCloseTerm();
	idx = er.indexOf("DOUBLE DELETE (NOFREE)");
	unitAssert(idx != String::npos);

	putenv("OW_MEM_NOFREE=0");

	cmd.clear();
	cmd.append(execName);
	cmd.append(String(AGGRESSIVE));
	rval = Exec::spawn(cmd);
	er = rval->err()->readAll();
	rval->waitCloseTerm();
	unitAssert(rval->processStatus().terminatedSuccessfully());

	putenv("OW_MEM_AGGRESSIVE=1");

	cmd.clear();
	cmd.append(execName);
	cmd.append(String(AGGRESSIVE));
	rval = Exec::spawn(cmd);
	er = rval->err()->readAll();
	rval->waitCloseTerm();
	idx = er.indexOf("OVERRUN");
	unitAssert(idx != String::npos);

	putenv("OW_MEM_AGGRESSIVE=0");
	if (wasDisabled)
	{
		putenv("OW_MEM_DISABLE=1");
	}
#endif // #ifdef OW_DEBUG_MEMORY
}

Test* OW_MemTracerTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_MemTracer");

#if !defined(OW_WIN32)

	testSuite->addTest (new TestCaller <OW_MemTracerTestCases>
			("testSomething",
			&OW_MemTracerTestCases::testSomething));
#endif

	return testSuite;
}

