/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
#include "OW_ConfigOpts.hpp"
#include "OW_OperationContext.hpp"

using namespace OpenWBEM;

void OW_ProviderManagerTestCases::setUp()
{
	g_testEnvironment->setConfigItem(
		ConfigOpts::CPPIFC_PROV_LOC_opt,
		"this is set to a dummy value so that the default won't be used, "
		"which may break things, if providers are actually installed there." );
}

void OW_ProviderManagerTestCases::tearDown()
{
}

void OW_ProviderManagerTestCases::testInit()
{
	ProviderManager pm;
	pm.load(testCreateMuxLoader());
}



void OW_ProviderManagerTestCases::testGetInstanceProvider()
{
	ProviderManager mgr;
	mgr.load(testCreateMuxLoader());
	OperationContext context("");
	LocalCIMOMHandle hdl = LocalCIMOMHandle(CIMOMEnvironmentRef(), RepositoryIFCRef(), context);
	mgr.init(createProvEnvRef(hdl));

	// test qualifier on class
	CIMClass c1("TestClass");
	CIMQualifier provQual(CIMQualifier::CIM_QUAL_PROVIDER);
	provQual.setValue(CIMValue("lib1::TestInstanceProvider"));
	c1.addQualifier(provQual);
	InstanceProviderIFCRef provRef = mgr.getInstanceProvider(
		createProvEnvRef(hdl), "root", c1);
	unitAssert(provRef);

	// bad qualifier
	CIMClass c2("TestClass");
	provQual.setValue(CIMValue("lib1::bad"));
	c2.addQualifier(provQual);
	unitAssertThrows(provRef = mgr.getInstanceProvider(
		createProvEnvRef(hdl), "root", c2));

	// self-registering provider all namespaces
	CIMClass c3("SelfReg");
	provRef = mgr.getInstanceProvider(
		createProvEnvRef(hdl), "root", c3);
	unitAssert(provRef);
	provRef = mgr.getInstanceProvider(
		createProvEnvRef(hdl), "root/foo", c3);
	unitAssert(provRef);
	
	// self-registering provider two namespaces
	CIMClass c4("SelfRegTwoNamespaces");
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
	CIMClass c5("Nothing");
	provRef = mgr.getInstanceProvider(
		createProvEnvRef(hdl), "root", c5);
	unitAssert(!provRef);

	// self-registering provider all namespaces - case insensitivity
	CIMClass c6("selFreG");
	provRef = mgr.getInstanceProvider(
		createProvEnvRef(hdl), "Root", c6);
	unitAssert(provRef);
	provRef = mgr.getInstanceProvider(
		createProvEnvRef(hdl), "rooT/fOo", c6);
	unitAssert(provRef);
	

}

void OW_ProviderManagerTestCases::testGetMethodProvider()
{
	ProviderManager mgr;
	mgr.load(testCreateMuxLoader());
	OperationContext context("");
	LocalCIMOMHandle hdl = LocalCIMOMHandle(CIMOMEnvironmentRef(), RepositoryIFCRef(), context);
	mgr.init(createProvEnvRef(hdl));

	// test qualifier on method
	CIMClass c1("TestClass");
	CIMMethod mwq("TestMethod");
	CIMQualifier provQual(CIMQualifier::CIM_QUAL_PROVIDER);
	provQual.setValue(CIMValue("lib1::TestMethodProvider"));
	mwq.addQualifier(provQual);
	c1.addMethod(mwq);
	MethodProviderIFCRef provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root", c1, mwq);
	unitAssert(provRef);

	// test qualifier on class, not on method
	CIMMethod m("TestMethod");
	CIMClass c2("TestClass");
	c2.addQualifier(provQual);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root", c2, m);
	unitAssert(provRef);

	// bad qualifier
	CIMClass c2_2("TestClass");
	provQual.setValue(CIMValue("lib1::bad"));
	c2_2.addQualifier(provQual);
	unitAssertThrows(provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root", c2_2, m));

	// self-registering provider all namespaces
	CIMClass c3("SelfReg");
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root", c3, m);
	unitAssert(provRef);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root/foo", c3, m);
	unitAssert(provRef);
	
	// self-registering provider two namespaces
	CIMClass c4("SelfRegTwoNamespaces");
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
	CIMClass c5("Nothing");
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root", c5, m);
	unitAssert(!provRef);

	// self-registering provider all namespaces - case insensitivity
	CIMClass c6("selFreG");
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "Root", c6, m);
	unitAssert(provRef);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "rooT/fOo", c6, m);
	unitAssert(provRef);
	
	// self-registering provider all namespaces - one method
	CIMClass c7("SelfRegOneMethod");
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root", c7, m);
	unitAssert(provRef);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root/foo", c7, m);
	unitAssert(provRef);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root/foo", c7, CIMMethod("BadMethod"));
	unitAssert(!provRef);
	
	// self-registering provider one namespaces - one method
	CIMClass c8("SelfRegOneNamespaceOneMethod");
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root", c8, m);
	unitAssert(provRef);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "Root", c8, CIMMethod("testMETHOD"));
	unitAssert(provRef);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root/foo", c8, m);
	unitAssert(!provRef);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root", c8, CIMMethod("BadMethod"));
	unitAssert(!provRef);
	provRef = mgr.getMethodProvider(
		createProvEnvRef(hdl), "root/foo", c8, CIMMethod("BadMethod"));
	unitAssert(!provRef);
	
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
void OW_ProviderManagerTestCases::testGetAssociatorProvider()
{
	ProviderManager mgr;
	mgr.load(testCreateMuxLoader());
	OperationContext context("");
	LocalCIMOMHandle hdl = LocalCIMOMHandle(CIMOMEnvironmentRef(), RepositoryIFCRef(), context);
	mgr.init(createProvEnvRef(hdl));

	// test qualifier on class
	CIMClass c1("TestClass");
	CIMQualifier provQual(CIMQualifier::CIM_QUAL_PROVIDER);
	provQual.setValue(CIMValue("lib1::TestAssociatorProvider"));
	c1.addQualifier(provQual);
	AssociatorProviderIFCRef provRef = mgr.getAssociatorProvider(
		createProvEnvRef(hdl), "root", c1);
	unitAssert(provRef);

	// bad qualifier
	CIMClass c2("TestClass");
	provQual.setValue(CIMValue("lib1::bad"));
	c2.addQualifier(provQual);
	unitAssertThrows(provRef = mgr.getAssociatorProvider(
		createProvEnvRef(hdl), "root", c2));

	// self-registering provider all namespaces
	CIMClass c3("SelfReg");
	provRef = mgr.getAssociatorProvider(
		createProvEnvRef(hdl), "root", c3);
	unitAssert(provRef);
	provRef = mgr.getAssociatorProvider(
		createProvEnvRef(hdl), "root/foo", c3);
	unitAssert(provRef);
	
	// self-registering provider two namespaces
	CIMClass c4("SelfRegTwoNamespaces");
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
	CIMClass c5("Nothing");
	provRef = mgr.getAssociatorProvider(
		createProvEnvRef(hdl), "root", c5);
	unitAssert(!provRef);

	// self-registering provider all namespaces - case insensitivity
	CIMClass c6("selFreG");
	provRef = mgr.getAssociatorProvider(
		createProvEnvRef(hdl), "Root", c6);
	unitAssert(provRef);
	provRef = mgr.getAssociatorProvider(
		createProvEnvRef(hdl), "rooT/fOo", c6);
	unitAssert(provRef);
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

void OW_ProviderManagerTestCases::testGetIndicationProvider()
{
	ProviderManager mgr;
	mgr.load(testCreateMuxLoader());
	OperationContext context("");
	LocalCIMOMHandle hdl = LocalCIMOMHandle(CIMOMEnvironmentRef(), RepositoryIFCRef(), context);
	mgr.init(createProvEnvRef(hdl));

	// self-registering provider all namespaces
	IndicationProviderIFCRefArray provRefs = mgr.getIndicationProviders(
		createProvEnvRef(hdl), "root", "SelfReg", "");
	unitAssert(provRefs.size() == 1);
	provRefs = mgr.getIndicationProviders(
		createProvEnvRef(hdl), "root/foo", "SelfReg", "");
	unitAssert(provRefs.size() == 1);
	
	provRefs = mgr.getIndicationProviders(
		createProvEnvRef(hdl), "root", "SelfReg2", "");
	unitAssert(provRefs.size() == 2);
	provRefs = mgr.getIndicationProviders(
		createProvEnvRef(hdl), "root/foo", "SelfReg2", "");
	unitAssert(provRefs.size() == 2);
	
	// self-registering provider two namespaces
	String c4("SelfRegTwoNamespaces");
	provRefs = mgr.getIndicationProviders(
		createProvEnvRef(hdl), "root", c4, "");
	unitAssert(provRefs.size() == 1);
	provRefs = mgr.getIndicationProviders(
		createProvEnvRef(hdl), "root/foo", c4, "");
	unitAssert(provRefs.size() == 0);
	provRefs = mgr.getIndicationProviders(
		createProvEnvRef(hdl), "root/good", c4, "");
	unitAssert(provRefs.size() == 1);
	
	// nothing
	String c5("Nothing");
	provRefs = mgr.getIndicationProviders(
		createProvEnvRef(hdl), "root", c5, "");
	unitAssert(provRefs.size() == 0);

	// self-registering provider all namespaces - case insensitivity
	String c6("selFreG");
	provRefs = mgr.getIndicationProviders(
		createProvEnvRef(hdl), "Root", c6, "");
	unitAssert(provRefs.size() == 1);
	provRefs = mgr.getIndicationProviders(
		createProvEnvRef(hdl), "rooT/fOo", c6, "");
	unitAssert(provRefs.size() == 1);

	// lifecycle providers
	provRefs = mgr.getIndicationProviders(
		createProvEnvRef(hdl), "root/cimv2", "CIM_InstCreation", "TestClass1");
	unitAssert(provRefs.size() == 1);
	provRefs = mgr.getIndicationProviders(
		createProvEnvRef(hdl), "root/cimv2", "CIM_InstModification", "TestClass2");
	unitAssert(provRefs.size() == 1);
	provRefs = mgr.getIndicationProviders(
		createProvEnvRef(hdl), "root/cimv2", "CIM_InstDeletion", "TestClass3");
	unitAssert(provRefs.size() == 1);
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
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	testSuite->addTest (new TestCaller <OW_ProviderManagerTestCases>
			("testGetAssociatorProvider",
			&OW_ProviderManagerTestCases::testGetAssociatorProvider));
#endif
	testSuite->addTest (new TestCaller <OW_ProviderManagerTestCases>
			("testGetIndicationProvider",
			&OW_ProviderManagerTestCases::testGetIndicationProvider));

	return testSuite;
}

