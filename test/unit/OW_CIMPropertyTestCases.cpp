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
#include "OW_CIMPropertyTestCases.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMtoXML.hpp"
#include "OW_TempFileStream.hpp"
#include "OW_CIMtoXML.hpp"
#include "OW_XMLNode.hpp"
#include "OW_XMLCIMFactory.hpp"
#include "OW_XMLParser.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMQualifierType.hpp"


void OW_CIMPropertyTestCases::setUp()
{
}

void OW_CIMPropertyTestCases::tearDown()
{
}

void OW_CIMPropertyTestCases::testEmbeddedClass()
{
	OW_CIMClass c1(true);
	c1.setName("test");
	OW_CIMQualifierType cqt("Description");
	cqt.setDataType(OW_CIMDataType::STRING);
	c1.addQualifier(OW_CIMQualifier("Description", cqt));

	OW_CIMProperty p1("testprop", OW_CIMValue(c1));
	OW_CIMClass c2;
	p1.getValue().get(c2);
	unitAssert( c1 == c2 );

	OW_TempFileStream ostr;
	OW_CIMtoXML(p1, ostr, OW_CIMtoXMLFlags::notLocalOnly, OW_CIMtoXMLFlags::includeQualifiers,
		OW_CIMtoXMLFlags::includeClassOrigin);
	ostr.rewind();
	OW_XMLParser parser(&ostr);

	OW_XMLNode node;
	unitAssertNoThrow(node = parser.parse());

	OW_CIMProperty p2 = OW_XMLCIMFactory::createProperty(node);

	unitAssert(p1 == p2);

}

void OW_CIMPropertyTestCases::testEmbeddedInstance()
{
	OW_CIMClass c1(true);
	c1.setName("test");
	OW_CIMQualifierType cqt("Description");
	cqt.setDataType(OW_CIMDataType::STRING);
	c1.addQualifier(OW_CIMQualifier("Description", cqt));

	OW_CIMInstance i1 = c1.newInstance();

	OW_CIMProperty p1("testprop", OW_CIMValue(i1));
	OW_CIMInstance i2;
	p1.getValue().get(i2);
	unitAssert( i1 == i2 );

	OW_TempFileStream ostr;
	OW_CIMtoXML(p1, ostr, OW_CIMtoXMLFlags::notLocalOnly, OW_CIMtoXMLFlags::includeQualifiers,
		OW_CIMtoXMLFlags::includeClassOrigin);
	ostr.rewind();
	OW_XMLParser parser(&ostr);

	OW_XMLNode node;
	unitAssertNoThrow(node = parser.parse());

	OW_CIMProperty p2 = OW_XMLCIMFactory::createProperty(node);

	unitAssert(p1 == p2);

}

Test* OW_CIMPropertyTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_CIMProperty");

	ADD_TEST_TO_SUITE(OW_CIMPropertyTestCases, testEmbeddedClass);
	ADD_TEST_TO_SUITE(OW_CIMPropertyTestCases, testEmbeddedInstance);

	return testSuite;
}

