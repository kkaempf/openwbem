/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "TestSuite.hpp"
#include "TestCaller.hpp"
#include "OW_CIMValueTestCases.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_Format.hpp"

void OW_CIMValueTestCases::setUp()
{
}

void OW_CIMValueTestCases::tearDown()
{
}

void OW_CIMValueTestCases::testGetArraySize()
{
    OW_CIMClassArray ca;
    ca.push_back(OW_CIMClass(OW_CIMNULL));
    ca.push_back(OW_CIMClass(OW_CIMNULL));
    ca.push_back(OW_CIMClass(OW_CIMNULL));
    OW_CIMValue v1(ca);
	unitAssert( v1.getArraySize() == 3 );

    OW_CIMInstanceArray ia;
    ia.push_back(OW_CIMInstance());
    ia.push_back(OW_CIMInstance());
    ia.push_back(OW_CIMInstance());
    ia.push_back(OW_CIMInstance());
    ia.push_back(OW_CIMInstance());
    OW_CIMValue v2(ia);
    unitAssert(v2.getArraySize() == 5);

    OW_CIMValue v3(v1);
    unitAssert(v3.getArraySize() == 3);

    OW_CIMValue v4(v2);
    unitAssert(v4.getArraySize() == 5);
    unitAssert(v1 == v3);
    unitAssert(v2 == v4);
    unitAssert(v1 != v2);
    unitAssert(v3 != v4);
}

void OW_CIMValueTestCases::testInserterOp()
{
	OW_CIMValue cv = OW_CIMValue(OW_String("String One"));
	OW_String str = OW_Format("%1", cv);
	unitAssert(str.equals("String One"));
}

Test* OW_CIMValueTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_CIMValue");

	testSuite->addTest (new TestCaller <OW_CIMValueTestCases> 
			("testGetArraySize", 
			&OW_CIMValueTestCases::testGetArraySize));
	testSuite->addTest (new TestCaller <OW_CIMValueTestCases> 
			("testInserterOp", 
			&OW_CIMValueTestCases::testInserterOp));

	return testSuite;
}

