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
#include "OW_CIMMethod.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"

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
	mgr.init(createProvEnvRef(hdl));

	// test qualifier on class
	OW_CIMClass c1("TestClass");
	OW_CIMQualifier provQual(OW_CIMQualifier::CIM_QUAL_PROVIDER);
	provQual.setValue(OW_CIMValue("lib1::TestInstanceProvider"));
	c1.addQualifier(provQual);
	OW_InstanceProviderIFCRef provRef = mgr.getInstanceProvider(
		createProvEnvRef(hdl), "root", c1);
	unitAssert(provRef);

	// bad qualifier
	OW_CIMClass c2("TestClass");
	provQual.setValue(OW_CIMValue("lib1::bad"));
	c2.addQualifier(provQual);
	unitAssertThrows(provRef = mgr.getInstanceProvider(
		createProvEnvRef(hdl), "root", c2));

	// self-registering provider all namespaces
	OW_CIMClass c3("SelfReg");
	provRef = mgr.getInstanceProvider(
		createProvEnvRef(hdl), "root", c3);
	unitAssert(provRef);
	provRef = mgr.getInstanceProvider(
		createProvEnvRef(hdl), "root/foo", c3);
	unitAssert(provRef);
	
	// self-registering provider two namespaces
	OW_CIMClass c4("SelfRegTwoNamespaces");
	provRef = mgr.getInstanceProvider(
		createProvEnvRef(hdl), "root", c4);
	unitAssert(provRef);
	provRef = mgr.getInstanceProvider(
		createProvEnvRef(hdl), "root/foo", c4);
	unitAssert(!provRef);
	provRef = mgr.getInstanceProvider(
		createProvEnvRef(hdl), "root/good", c4);
	unitAssert(provRef);
	
	// nothing
	OW_CIMClass c5("Nothing");
	provRef = mgr.getInstanceProvider(
		createProvEnvRef(hdl), "root", c5);
	unitAssert(!provRef);

	// self-registering provider all namespaces - case insensitivity
	OW_CIMClass c6("selFreG");
	provRef = mgr.getInstanceProvider(
		createProvEnvRef(hdl), "Root", c6);
	unitAssert(provRef);
	provRef = mgr.getInstanceProvider(
		createProvEnvRef(hdl), "rooT/fOo", c6);
	unitAssert(provRef);
	

}

void OW_ProviderManagerTestCases::testGetMethodProvider()
{
	OW_ProviderManager mgr;
	mgr.load(testCreateMuxLoader());
	OW_LocalCIMOMHandle hdl;
	mgr.init(createProvEnvRef(hdl));

	// test qualifier on method
	OW_CIMClass c1("TestClass");
	OW_CIMMethod mwq("TestMethod");
	OW_CIMQualifier provQual(OW_CIMQualifier::CIM_QUAL_PROVIDER);
	provQual.setValue(OW_CIMValue("lib1::TestMethodProvider"));
	mwq.addQualifier(provQual);
	c1.addMethod(mwq);
	OW_MethodProviderIFCRef provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root", c1, mwq);
	unitAssert(provRef);

	// test qualifier on class, not on method
	OW_CIMMethod m("TestMethod");
	OW_CIMClass c2("TestClass");
	c2.addQualifier(provQual);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root", c2, m);
	unitAssert(provRef);

	// bad qualifier
	OW_CIMClass c2_2("TestClass");
	provQual.setValue(OW_CIMValue("lib1::bad"));
	c2_2.addQualifier(provQual);
	unitAssertThrows(provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root", c2_2, m));

	// self-registering provider all namespaces
	OW_CIMClass c3("SelfReg");
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root", c3, m);
	unitAssert(provRef);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root/foo", c3, m);
	unitAssert(provRef);
	
	// self-registering provider two namespaces
	OW_CIMClass c4("SelfRegTwoNamespaces");
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root", c4, m);
	unitAssert(provRef);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root/foo", c4, m);
	unitAssert(!provRef);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "ROOT/FOO", c4, m);
	unitAssert(!provRef);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root/good", c4, m);
	unitAssert(provRef);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "ROOT/GOOD", c4, m);
	unitAssert(provRef);
	
	// nothing
	OW_CIMClass c5("Nothing");
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root", c5, m);
	unitAssert(!provRef);

	// self-registering provider all namespaces - case insensitivity
	OW_CIMClass c6("selFreG");
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "Root", c6, m);
	unitAssert(provRef);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "rooT/fOo", c6, m);
	unitAssert(provRef);
	
	// self-registering provider all namespaces - one method
	OW_CIMClass c7("SelfRegOneMethod");
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root", c7, m);
	unitAssert(provRef);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root/foo", c7, m);
	unitAssert(provRef);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root/foo", c7, OW_CIMMethod("BadMethod"));
	unitAssert(!provRef);
	
	// self-registering provider one namespaces - one method
	OW_CIMClass c8("SelfRegOneNamespaceOneMethod");
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root", c8, m);
	unitAssert(provRef);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "Root", c8, OW_CIMMethod("testMETHOD"));
	unitAssert(provRef);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root/foo", c8, m);
	unitAssert(!provRef);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root", c8, OW_CIMMethod("BadMethod"));
	unitAssert(!provRef);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root/foo", c8, OW_CIMMethod("BadMethod"));
	unitAssert(!provRef);
	
}

void OW_ProviderManagerTestCases::testGetPropertyProvider()
{
	OW_ProviderManager mgr;
	mgr.load(testCreateMuxLoader());
	OW_LocalCIMOMHandle hdl;
	mgr.init(createProvEnvRef(hdl));

	// test qualifier on property
	OW_CIMClass c1("TestClass");
	OW_CIMProperty pwq("TestProperty");
	OW_CIMQualifier provQual(OW_CIMQualifier::CIM_QUAL_PROVIDER);
	provQual.setValue(OW_CIMValue("lib1::TestPropertyProvider"));
	pwq.addQualifier(provQual);
	c1.addProperty(pwq);
	OW_PropertyProviderIFCRef provRef;
	unitAssertNoThrow(provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), "root", c1, pwq));
	unitAssert(provRef);

	// bad qualifier
	OW_CIMClass c2("TestClass");
	OW_CIMProperty p2("TestProperty");
	provQual.setValue(OW_CIMValue("lib1::BAD"));
	p2.addQualifier(provQual);
	c2.addProperty(p2);
	unitAssertThrows(provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), "root", c1, p2));

	// self-registering provider all namespaces
	OW_CIMProperty p("TestProperty");
	OW_CIMClass c3("SelfReg");
	unitAssertNoThrow(provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), "root", c3, p));
	unitAssert(provRef);
	unitAssertNoThrow(provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), "root/foo", c3, p));
	unitAssert(provRef);
	
	// self-registering provider two namespaces
	OW_CIMClass c4("SelfRegTwoNamespaces");
	unitAssertNoThrow(provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), "root", c4, p));
	unitAssert(provRef);
	unitAssertNoThrow(provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), "root/foo", c4, p));
	unitAssert(!provRef);
	unitAssertNoThrow(provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), "ROOT/FOO", c4, p));
	unitAssert(!provRef);
	unitAssertNoThrow(provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), "root/good", c4, p));
	unitAssert(provRef);
	unitAssertNoThrow(provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), "ROOT/GOOD", c4, p));
	unitAssert(provRef);
	
	// nothing
	OW_CIMClass c5("Nothing");
	unitAssertNoThrow(provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), "root", c5, p));
	unitAssert(!provRef);

	// self-registering provider all namespaces - case insensitivity
	OW_CIMClass c6("selFreG");
	unitAssertNoThrow(provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), "Root", c6, p));
	unitAssert(provRef);
	unitAssertNoThrow(provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), "rooT/fOo", c6, p));
	unitAssert(provRef);
	
	// self-registering provider all namespaces - one Property
	OW_CIMClass c7("SelfRegOneProperty");
	unitAssertNoThrow(provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), "root", c7, p));
	unitAssert(provRef);
	unitAssertNoThrow(provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), "root/foo", c7, p));
	unitAssert(provRef);
	unitAssertNoThrow(provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), "root/foo", c7, OW_CIMProperty("BadProperty")));
	unitAssert(!provRef);
	
	// self-registering provider one namespaces - one Property
	OW_CIMClass c8("SelfRegOneNamespaceOneProperty");
	unitAssertNoThrow(provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), "root", c8, p));
	unitAssert(provRef);
	unitAssertNoThrow(provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), "Root", c8, OW_CIMProperty("testProperty")));
	unitAssert(provRef);
	unitAssertNoThrow(provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), "root/foo", c8, p));
	unitAssert(!provRef);
	unitAssertNoThrow(provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), "root", c8, OW_CIMProperty("BadProperty")));
	unitAssert(!provRef);
	unitAssertNoThrow(provRef = mgr.getPropertyProvider(
		createProvEnvRef(hdl), "root/foo", c8, OW_CIMProperty("BadProperty")));
	unitAssert(!provRef);
}

void OW_ProviderManagerTestCases::testGetAssociatorProvider()
{
	OW_ProviderManager mgr;
	mgr.load(testCreateMuxLoader());
	OW_LocalCIMOMHandle hdl;
	mgr.init(createProvEnvRef(hdl));

	// test qualifier on class
	OW_CIMClass c1("TestClass");
	OW_CIMQualifier provQual(OW_CIMQualifier::CIM_QUAL_PROVIDER);
	provQual.setValue(OW_CIMValue("lib1::TestAssociatorProvider"));
	c1.addQualifier(provQual);
	OW_AssociatorProviderIFCRef provRef = mgr.getAssociatorProvider(
		createProvEnvRef(hdl), "root", c1);
	unitAssert(provRef);

	// bad qualifier
	OW_CIMClass c2("TestClass");
	provQual.setValue(OW_CIMValue("lib1::bad"));
	c2.addQualifier(provQual);
	unitAssertThrows(provRef = mgr.getAssociatorProvider(
		createProvEnvRef(hdl), "root", c2));

	// self-registering provider all namespaces
	OW_CIMClass c3("SelfReg");
	provRef = mgr.getAssociatorProvider(
		createProvEnvRef(hdl), "root", c3);
	unitAssert(provRef);
	provRef = mgr.getAssociatorProvider(
		createProvEnvRef(hdl), "root/foo", c3);
	unitAssert(provRef);
	
	// self-registering provider two namespaces
	OW_CIMClass c4("SelfRegTwoNamespaces");
	provRef = mgr.getAssociatorProvider(
		createProvEnvRef(hdl), "root", c4);
	unitAssert(provRef);
	provRef = mgr.getAssociatorProvider(
		createProvEnvRef(hdl), "root/foo", c4);
	unitAssert(!provRef);
	provRef = mgr.getAssociatorProvider(
		createProvEnvRef(hdl), "root/good", c4);
	unitAssert(provRef);
	
	// nothing
	OW_CIMClass c5("Nothing");
	provRef = mgr.getAssociatorProvider(
		createProvEnvRef(hdl), "root", c5);
	unitAssert(!provRef);

	// self-registering provider all namespaces - case insensitivity
	OW_CIMClass c6("selFreG");
	provRef = mgr.getAssociatorProvider(
		createProvEnvRef(hdl), "Root", c6);
	unitAssert(provRef);
	provRef = mgr.getAssociatorProvider(
		createProvEnvRef(hdl), "rooT/fOo", c6);
	unitAssert(provRef);
	

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

