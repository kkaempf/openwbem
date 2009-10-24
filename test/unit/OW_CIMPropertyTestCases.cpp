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
#include "OW_CIMPropertyTestCases.hpp"
AUTO_UNIT_TEST_SUITE_NAMED(OW_CIMPropertyTestCases,"OW_CIMProperty");
#include "OW_CIMProperty.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMtoXML.hpp"
#include "blocxx/TempFileStream.hpp"
#include "OW_CIMtoXML.hpp"
#include "OW_XMLCIMFactory.hpp"
#include "OW_CIMXMLParser.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMQualifierType.hpp"


using namespace OpenWBEM;

void OW_CIMPropertyTestCases::setUp()
{
}

void OW_CIMPropertyTestCases::tearDown()
{
}

void OW_CIMPropertyTestCases::testEmbeddedClass()
{
	CIMClass c1;
	c1.setName("test");
	CIMQualifierType cqt("Description");
	cqt.setDataType(CIMDataType::STRING);
	c1.addQualifier(CIMQualifier(cqt));

	CIMProperty p1("testprop", CIMValue(c1));
	CIMClass c2(CIMNULL);
	p1.getValue().get(c2);
	unitAssert( c1 == c2 );

	TempFileStream ostr;
	CIMtoXML(p1, ostr);
	ostr.rewind();
	CIMXMLParser parser(ostr);

	unitAssert(parser);

	CIMProperty p2 = XMLCIMFactory::createProperty(parser);

	unitAssert(p1 == p2);

}

void OW_CIMPropertyTestCases::testEmbeddedInstance()
{
	CIMClass c1;
	c1.setName("test");
	CIMQualifierType cqt("Description");
	cqt.setDataType(CIMDataType::STRING);
	c1.addQualifier(CIMQualifier(cqt));

	CIMInstance i1 = c1.newInstance();

	CIMProperty p1("testprop", CIMValue(i1));
	CIMInstance i2(CIMNULL);
	p1.getValue().get(i2);
	unitAssert( i1 == i2 );

	TempFileStream ostr;
	CIMtoXML(p1, ostr);
	ostr.rewind();
	CIMXMLParser parser(ostr);

	unitAssert(parser);

	CIMProperty p2 = XMLCIMFactory::createProperty(parser);

	unitAssert(p1 == p2);

}

Test* OW_CIMPropertyTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_CIMProperty");

	ADD_TEST_TO_SUITE(OW_CIMPropertyTestCases, testEmbeddedClass);
	ADD_TEST_TO_SUITE(OW_CIMPropertyTestCases, testEmbeddedInstance);

	return testSuite;
}

