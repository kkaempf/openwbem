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
#include "OW_ProviderManagerTestCases.hpp"
#include "OW_ProviderManager.hpp"
#include "testSharedLibraryLoader.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMClass.hpp"

void OW_ProviderManagerTestCases::setUp()
{
}

void OW_ProviderManagerTestCases::tearDown()
{
}

void OW_ProviderManagerTestCases::testInit()
{
	OW_ProviderManager pm;
	pm.load(testCreateMuxLoader());
}



void OW_ProviderManagerTestCases::testGetInstanceProvider()
{
	OW_ProviderManager mgr;
	mgr.load(testCreateMuxLoader());
	OW_LocalCIMOMHandle hdl;
	OW_CIMClass c("TestClass");
	c.addQualifier(OW_CIMQualifier("TestInstanceProvider"));
	OW_InstanceProviderIFCRef provRef = mgr.getInstanceProvider(
		createProvEnvRef(hdl), "root", c);
	unitAssert(provRef.isNull());
}

void OW_ProviderManagerTestCases::testGetMethodProvider()
{
	OW_ProviderManager mgr;
	mgr.load(testCreateMuxLoader());
	OW_LocalCIMOMHandle hdl;
	OW_MethodProviderIFCRef provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), OW_CIMQualifier("TestMethodProvider"));
	unitAssert(provRef.isNull());
}

void OW_ProviderManagerTestCases::testGetPropertyProvider()
{
	OW_ProviderManager mgr;
	mgr.load(testCreateMuxLoader());
	OW_LocalCIMOMHandle hdl;
	OW_PropertyProviderIFCRef provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), OW_CIMQualifier("TestPropertyProvider"));
	unitAssert(provRef.isNull());
}

void OW_ProviderManagerTestCases::testGetAssociatorProvider()
{
	OW_ProviderManager mgr;
	mgr.load(testCreateMuxLoader());
	OW_LocalCIMOMHandle hdl;
	OW_CIMClass c("TestClass");
	c.addQualifier(OW_CIMQualifier("TestAssociatorProvider"));
	OW_AssociatorProviderIFCRef provRef = mgr.getAssociatorProvider(
		createProvEnvRef(hdl), "root", c);
	unitAssert(provRef.isNull());
}


Test* OW_ProviderManagerTestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("OW_ProviderManager");

	testSuite->addTest (new TestCaller <OW_ProviderManagerTestCases>
			("testInit",
			&OW_ProviderManagerTestCases::testInit));

	testSuite->addTest (new TestCaller <OW_ProviderManagerTestCases>
			("testGetInstanceProvider",
			&OW_ProviderManagerTestCases::testGetInstanceProvider));
	testSuite->addTest (new TestCaller <OW_ProviderManagerTestCases>
			("testGetMethodProvider",
			&OW_ProviderManagerTestCases::testGetMethodProvider));
	testSuite->addTest (new TestCaller <OW_ProviderManagerTestCases>
			("testGetPropertyProvider",
			&OW_ProviderManagerTestCases::testGetPropertyProvider));
	testSuite->addTest (new TestCaller <OW_ProviderManagerTestCases>
			("testGetAssociatorProvider",
			&OW_ProviderManagerTestCases::testGetAssociatorProvider));

	return testSuite;
}

