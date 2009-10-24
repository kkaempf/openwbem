/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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

/**
 * @author Dan Nuffer
 */


#include "OW_config.h"
#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_StringArrayPropertyTestCases.hpp"
AUTO_UNIT_TEST_SUITE_NAMED(OW_StringArrayPropertyTestCases,"OW_StringArrayProperty");
#include "blocxx/String.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMClass.hpp"
#include <iostream>

using namespace OpenWBEM;

void OW_StringArrayPropertyTestCases::setUp()
{
}

void OW_StringArrayPropertyTestCases::tearDown()
{
}

void OW_StringArrayPropertyTestCases::testSomething()
{
	try
	{
		CIMClass cc;
		cc.setName("Class");
		CIMProperty prop = CIMProperty("SA");
		CIMDataType dt = CIMDataType::STRING;
		dt.setToArrayType(-1);
		prop.setDataType(dt);
		CIMInstance inst = cc.newInstance();
		StringArray sa;
		sa.push_back(String("one"));
		sa.push_back(String("two"));
		inst.setProperty(String("SA"), CIMValue(sa));
		StringArray newSA;
		inst.getProperty("SA").getValue().get(newSA);
		unitAssert(newSA.size() == 2);
		unitAssert(newSA[0].equals("one"));
		unitAssert(newSA[1].equals("two"));
		newSA.clear();
		newSA.push_back("ONE");
		newSA.push_back("TWO");
		newSA.push_back("THREE");
		inst.setProperty(CIMProperty(String("SA2"),CIMValue(newSA)));
		newSA.clear();
		inst.getProperty("SA2").getValue().get(newSA);
		unitAssert(newSA.size() == 3);
		unitAssert(newSA[0].equals("ONE"));
		unitAssert(newSA[1].equals("TWO"));
		unitAssert(newSA[2].equals("THREE"));
	}
	catch(CIMException& ce)
	{
		std::cerr << ce.getMessage() << std::endl;
		unitAssert(false);
	}
	catch(Exception& e)
	{
		std::cerr << e.getMessage() << std::endl;
		//std::cerr << e.getStackTrace() << std::endl;
		unitAssert(false);
	}
}

Test* OW_StringArrayPropertyTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_StringArrayProperty");

	testSuite->addTest (new TestCaller <OW_StringArrayPropertyTestCases>
			("testSomething",
			&OW_StringArrayPropertyTestCases::testSomething));

	return testSuite;
}

