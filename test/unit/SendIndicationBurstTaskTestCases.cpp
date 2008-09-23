/*******************************************************************************
* Copyright (C) 2008 Quest Software, Inc. All rights reserved.
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
#include "SendIndicationBurstTaskTestCases.hpp"
AUTO_UNIT_TEST_SUITE_NAMED(SendIndicationBurstTaskTestCases,"SendIndicationBurstTask");
#include "OW_SendIndicationBurstTask.hpp"
#include "CIMInstanceUtils.hpp"

using namespace OpenWBEM;
using namespace CIMInstanceUtils;

void SendIndicationBurstTaskTestCases::setUp()
{
	m_indicationExporter = new MockIndicationExporter();
}

void SendIndicationBurstTaskTestCases::tearDown()
{
	m_indicationExporter = 0;
}

namespace
{
	template <std::size_t n>
	char const * s(char const (&arr)[n])
	{
		return arr;
	}

	CIMProperty props1[] = { cimProp("p1", s("v1")), cimProp("p2", 37) };
	CIMInstance const inst1 = cimInst("c1", props1);

	CIMProperty props2[] = { cimProp("prop", false) };
	CIMInstance const inst2 = cimInst("c2", props2);
}

void SendIndicationBurstTaskTestCases::testSendOne()
{
	CIMInstance arr[1] = { inst1 };
	Array<CIMInstance> indications(arr, arr + 1);
	SendIndicationBurstTask task(m_indicationExporter, indications);

	m_indicationExporter->expectIndication(inst1);

	task.run();

	m_indicationExporter->verify();
}

void SendIndicationBurstTaskTestCases::testSendTwo()
{
	CIMInstance arr[2] = { inst1, inst2 };
	Array<CIMInstance> indications(arr, arr + 2);
	SendIndicationBurstTask task(m_indicationExporter, indications);

	m_indicationExporter->expectIndication(inst1);
	m_indicationExporter->expectIndication(inst2);

	task.run();

	m_indicationExporter->verify();
}

Test* SendIndicationBurstTaskTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("SendIndicationBurstTask");

	ADD_TEST_TO_SUITE(SendIndicationBurstTaskTestCases, testSendOne);
	ADD_TEST_TO_SUITE(SendIndicationBurstTaskTestCases, testSendTwo);

	return testSuite;
}

