/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
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
#include "OW_MemTracerTestCases.hpp"

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
	PopenStreams rval = Exec::safePopen(cmd);
	String er = rval.err()->readAll();
	size_t idx = er.indexOf("UNDERRUN");
	unitAssert(idx != String::npos);

	cmd.clear();
	cmd.append(execName);
	cmd.append(String(OVERRUN));
	rval = Exec::safePopen(cmd);
	er = rval.err()->readAll();
	idx = er.indexOf("OVERRUN");
	unitAssert(idx != String::npos);

	cmd.clear();
	cmd.append(execName);
	cmd.append(String(UNKNOWN_ADDR));
	rval = Exec::safePopen(cmd);
	er = rval.err()->readAll();
	idx = er.indexOf("UNKNOWN ADDRESS");
	unitAssert(idx != String::npos);

	cmd.clear();
	cmd.append(execName);
	cmd.append(String(DOUBLE_DELETE));
	rval = Exec::safePopen(cmd);
	er = rval.err()->readAll();
	idx = er.indexOf("DOUBLE DELETE");
	unitAssert(idx != String::npos);

	putenv("OW_MEM_NOFREE=1");

	cmd.clear();
	cmd.append(execName);
	cmd.append(String(DOUBLE_DELETE));
	rval = Exec::safePopen(cmd);
	er = rval.err()->readAll();
	idx = er.indexOf("DOUBLE DELETE (NOFREE)");
	unitAssert(idx != String::npos);

	putenv("OW_MEM_NOFREE=0");

	cmd.clear();
	cmd.append(execName);
	cmd.append(String(AGGRESSIVE));
	rval = Exec::safePopen(cmd);
	er = rval.err()->readAll();
	idx = rval.getExitStatus();
	unitAssert(idx == 0);

	putenv("OW_MEM_AGGRESSIVE=1");

	cmd.clear();
	cmd.append(execName);
	cmd.append(String(AGGRESSIVE));
	rval = Exec::safePopen(cmd);
	er = rval.err()->readAll();
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

	testSuite->addTest (new TestCaller <OW_MemTracerTestCases> 
			("testSomething", 
			&OW_MemTracerTestCases::testSomething));

	return testSuite;
}

