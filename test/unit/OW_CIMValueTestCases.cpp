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
#include "OW_CIMValueTestCases.hpp"
AUTO_UNIT_TEST_SUITE_NAMED(OW_CIMValueTestCases,"OW_CIMValue");
#include "OW_CIMValue.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_Format.hpp"
#include "OW_CIMDateTime.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMProperty.hpp"

using namespace OpenWBEM;

void OW_CIMValueTestCases::setUp()
{
}

void OW_CIMValueTestCases::tearDown()
{
}

void OW_CIMValueTestCases::testGetArraySize()
{
	CIMClassArray ca;
	ca.push_back(CIMClass(CIMNULL));
	ca.push_back(CIMClass(CIMNULL));
	ca.push_back(CIMClass(CIMNULL));
	CIMValue v1(ca);
	unitAssert( v1.getArraySize() == 3 );

	CIMInstanceArray ia;
	ia.push_back(CIMInstance(CIMNULL));
	ia.push_back(CIMInstance(CIMNULL));
	ia.push_back(CIMInstance(CIMNULL));
	ia.push_back(CIMInstance(CIMNULL));
	ia.push_back(CIMInstance(CIMNULL));
	CIMValue v2(ia);
	unitAssert(v2.getArraySize() == 5);

	CIMValue v3(v1);
	unitAssert(v3.getArraySize() == 3);

	CIMValue v4(v2);
	unitAssert(v4.getArraySize() == 5);
	unitAssert(v1 == v3);
	unitAssert(v2 == v4);
	unitAssert(v1 != v2);
	unitAssert(v3 != v4);
}

void OW_CIMValueTestCases::testInserterOp()
{
	CIMValue cv = CIMValue(String("String One"));
	String str = Format("%1", cv);
	unitAssert(str.equals("String One"));
}

void OW_CIMValueTestCases::testToMOF()
{
	unitAssertEquals(CIMValue(CIMDateTime("00000000000000.000000:000")).toMOF(), "\"00000000000000.000000:000\"");
	unitAssertEquals(CIMValue(CIMDateTimeArray(2, CIMDateTime("00000000000000.000000:000"))).toMOF(), "{\"00000000000000.000000:000\",\"00000000000000.000000:000\"}");

	UInt8Array a(5);
	for (size_t i = 0; i < a.size(); ++i)
	{
		a[i] = i;
	}
	CIMValue v(a);
	unitAssertEquals(v.toString(), "0,1,2,3,4");
	unitAssertEquals(v.toMOF(), "{0,1,2,3,4}");

	unitAssertEquals(CIMValue(CIMNULL).toMOF(), "NULL");
	unitAssertEquals(CIMValue(true).toMOF(), "true");
	unitAssertEquals(CIMValue(false).toMOF(), "false");
	unitAssertEquals(CIMValue(BoolArray(2, false)).toMOF(), "{false,false}");
	unitAssertEquals(CIMValue("s").toString(), "s");
	unitAssertEquals(CIMValue("s").toMOF(), "\"s\"");
	unitAssertEquals(CIMValue(StringArray(2, "s")).toString(), "s,s");
	unitAssertEquals(CIMValue(StringArray(2, "s")).toMOF(), "{\"s\",\"s\"}");
	unitAssertEquals(CIMValue(StringArray(2, "\"s\"")).toMOF(), "{\"\\\"s\\\"\",\"\\\"s\\\"\"}");
	{
		CIMObjectPath path("ns", "cls");
		path.setKeyValue("p", CIMValue("v"));
		unitAssertEquals(CIMValue(path).toMOF(), Format("%1", path.toMOF()).toString());
		unitAssertEquals(CIMValue(CIMObjectPathArray(2, path)).toMOF(), Format("{%1,%1}", path.toMOF()).toString());
	}
	{
		CIMClass c("c");
		c.addProperty(CIMProperty("p", CIMDataType::STRING));
		unitAssertEquals(CIMValue(c).toMOF(), Format("\"%1\"", CIMObjectPath::escape(c.toMOF())).toString());
		unitAssertEquals(CIMValue(CIMClassArray(2, c)).toMOF(), Format("{\"%1\",\n\"%1\"}", CIMObjectPath::escape(c.toMOF())).toString());
	}
	{
		CIMInstance i("i");
		i.setProperty("p", CIMValue("s"));
		unitAssertEquals(CIMValue(i).toMOF(), Format("\"%1\"", CIMObjectPath::escape(i.toMOF()).toString()));
		unitAssertEquals(CIMValue(CIMInstanceArray(2, i)).toMOF(), Format("{\"%1\",\n\"%1\"}", CIMObjectPath::escape(i.toMOF())).toString());
	}
}

void OW_CIMValueTestCases::testToString()
{
	unitAssert(CIMValue(CIMDateTime("00000000000000.000000:000")).toString() == "00000000000000.000000:000");

	UInt8Array a(5);
	for (size_t i = 0; i < a.size(); ++i)
	{
		a[i] = i;
	}
	CIMValue v(a);
	unitAssert(v.toString() == "0,1,2,3,4");
}

void OW_CIMValueTestCases::test_createSimpleValue()
{
	CIMValue v(CIMNULL);
	unitAssertNoThrow(v = CIMValue::createSimpleValue("datetime", "00000000000000.000000:000"));
	unitAssert(v.toString() == "00000000000000.000000:000");
	unitAssert(v.toMOF() == "\"00000000000000.000000:000\"");
	unitAssert(v.getType() == CIMDataType::DATETIME);
	unitAssert(v.isArray() == false);
	unitAssert(v.toCIMDateTime() == CIMDateTime());
	CIMDateTime cdt(CIMNULL);
	v.get(cdt);
	unitAssert(cdt == CIMDateTime());
	unitAssert(cdt.toString() == v.toString());
}

Test* OW_CIMValueTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_CIMValue");

	ADD_TEST_TO_SUITE(OW_CIMValueTestCases, testGetArraySize);
	ADD_TEST_TO_SUITE(OW_CIMValueTestCases, testInserterOp);
	ADD_TEST_TO_SUITE(OW_CIMValueTestCases, testToMOF);
	ADD_TEST_TO_SUITE(OW_CIMValueTestCases, testToString);
	ADD_TEST_TO_SUITE(OW_CIMValueTestCases, test_createSimpleValue);

	return testSuite;
}

