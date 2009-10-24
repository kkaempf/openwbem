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


#define PROVIDE_AUTO_TEST_MAIN
#include "AutoTest.hpp"
#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_CIMClassTestCases.hpp"
AUTO_UNIT_TEST_SUITE_NAMED(OW_CIMClassTestCases,"OW_CIMClass");
#include "OW_CIMClass.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMDataType.hpp"
#include "OW_CIMInstance.hpp"
#include "blocxx/String.hpp"

using namespace OpenWBEM;

void OW_CIMClassTestCases::setUp()
{
}

void OW_CIMClassTestCases::tearDown()
{
}

void OW_CIMClassTestCases::testNewInstance()
{
	CIMClass c("c");
	CIMValue v(UInt16(10));
	CIMProperty p("p");
	p.setDataType(CIMDataType::UINT16);
	p.setValue(v);
	c.addProperty(p);
	CIMInstance i = c.newInstance();
	unitAssert(i.getProperty(String("p")).getValue() == v);
}

Test* OW_CIMClassTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_CIMClass");

	testSuite->addTest (new TestCaller <OW_CIMClassTestCases>
			("testNewInstance",
			&OW_CIMClassTestCases::testNewInstance));

	return testSuite;
}

