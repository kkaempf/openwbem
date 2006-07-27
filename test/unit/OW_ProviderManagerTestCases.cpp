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
#include "OW_LocalOperationContext.hpp"
#include "OW_CerrLogger.hpp"

using namespace OpenWBEM;

void OW_ProviderManagerTestCases::setUp()
{
	g_testEnvironment->setConfigItem(
		ConfigOpts::CPPPROVIFC_PROV_LOCATION_opt,
		"this is set to a dummy value so that the default won't be used, "
		"which may break things, if providers are actually installed there." );

	g_testEnvironment->setConfigItem(
		ConfigOpts::DISABLE_CPP_PROVIDER_INTERFACE_opt, "true");
}

void OW_ProviderManagerTestCases::tearDown()
{
}

void OW_ProviderManagerTestCases::testInit()
{
	ProviderManager pm;
	pm.load(testCreateMuxLoader(), g_testEnvironment);
}

namespace
{

class TestServiceEnvironmentIFC : public ServiceEnvironmentIFC
{
};

ServiceEnvironmentIFCRef createServiceEnvRef()
{
	return ServiceEnvironmentIFCRef(new TestServiceEnvironmentIFC);
}

}

void OW_ProviderManagerTestCases::testGetInstanceProvider()
{
	ProviderManager mgr;
	mgr.load(testCreateMuxLoader(), g_testEnvironment);
	LocalOperationContext context;
	LocalCIMOMHandle hdl = LocalCIMOMHandle(CIMOMEnvironmentRef(), RepositoryIFCRef(), context);
	mgr.init(createServiceEnvRef());

	// test qualifier on class
	CIMClass c1("TestClass");
	CIMQualifier provQual(CIMQualifier::CIM_QUAL_PROVIDER);
	provQual.setValue(CIMValue("lib1::TestInstanceProvider"));
	c1.addQualifier(provQual);
	InstanceProviderIFCRef provRef = mgr.getInstanceProvider("root", c1);
	unitAssert(provRef);

	// bad qualifier
	CIMClass c2("TestClass");
	provQual.setValue(CIMValue("lib1::bad"));
	c2.addQualifier(provQual);
	unitAssertThrows(provRef = mgr.getInstanceProvider("root", c2));

	// self-registering provider all namespaces
	CIMClass c3("SelfReg");
	provRef = mgr.getInstanceProvider("root", c3);
	unitAssert(provRef);
	provRef = mgr.getInstanceProvider("root/foo", c3);
	unitAssert(provRef);
	
	// self-registering provider two namespaces
	CIMClass c4("SelfRegTwoNamespaces");
	provRef = mgr.getInstanceProvider("root", c4);
	unitAssert(provRef);
	provRef = mgr.getInstanceProvider("root/foo", c4);
	unitAssert(!provRef);
	provRef = mgr.getInstanceProvider("root/good", c4);
	unitAssert(provRef);
	
	// nothing
	CIMClass c5("Nothing");
	provRef = mgr.getInstanceProvider("root", c5);
	unitAssert(!provRef);

	// self-registering provider all namespaces - case insensitivity
	CIMClass c6("selFreG");
	provRef = mgr.getInstanceProvider("Root", c6);
	unitAssert(provRef);
	provRef = mgr.getInstanceProvider("rooT/fOo", c6);
	unitAssert(provRef);
	

}

void OW_ProviderManagerTestCases::testGetMethodProvider()
{
	ProviderManager mgr;
	mgr.load(testCreateMuxLoader(), g_testEnvironment);
	LocalOperationContext context;
	LocalCIMOMHandle hdl = LocalCIMOMHandle(CIMOMEnvironmentRef(), RepositoryIFCRef(), context);
	mgr.init(createServiceEnvRef());

	// test qualifier on method
	CIMClass c1("TestClass");
	CIMMethod mwq("TestMethod");
	CIMQualifier provQual(CIMQualifier::CIM_QUAL_PROVIDER);
	provQual.setValue(CIMValue("lib1::TestMethodProvider"));
	mwq.addQualifier(provQual);
	c1.addMethod(mwq);
	MethodProviderIFCRef provRef = mgr.getMethodProvider("root", c1, mwq);
	unitAssert(provRef);

	// test qualifier on class, not on method
	CIMMethod m("TestMethod");
	CIMClass c2("TestClass");
	c2.addQualifier(provQual);
	provRef = mgr.getMethodProvider("root", c2, m);
	unitAssert(provRef);

	// bad qualifier
	CIMClass c2_2("TestClass");
	provQual.setValue(CIMValue("lib1::bad"));
	c2_2.addQualifier(provQual);
	unitAssertThrows(provRef = mgr.getMethodProvider("root", c2_2, m));

	// self-registering provider all namespaces
	CIMClass c3("SelfReg");
	provRef = mgr.getMethodProvider("root", c3, m);
	unitAssert(provRef);
	provRef = mgr.getMethodProvider("root/foo", c3, m);
	unitAssert(provRef);
	
	// self-registering provider two namespaces
	CIMClass c4("SelfRegTwoNamespaces");
	provRef = mgr.getMethodProvider("root", c4, m);
	unitAssert(provRef);
	provRef = mgr.getMethodProvider("root/foo", c4, m);
	unitAssert(!provRef);
	provRef = mgr.getMethodProvider("ROOT/FOO", c4, m);
	unitAssert(!provRef);
	provRef = mgr.getMethodProvider("root/good", c4, m);
	unitAssert(provRef);
	provRef = mgr.getMethodProvider("ROOT/GOOD", c4, m);
	unitAssert(provRef);
	
	// nothing
	CIMClass c5("Nothing");
	provRef = mgr.getMethodProvider("root", c5, m);
	unitAssert(!provRef);

	// self-registering provider all namespaces - case insensitivity
	CIMClass c6("selFreG");
	provRef = mgr.getMethodProvider("Root", c6, m);
	unitAssert(provRef);
	provRef = mgr.getMethodProvider("rooT/fOo", c6, m);
	unitAssert(provRef);
	
	// self-registering provider all namespaces - one method
	CIMClass c7("SelfRegOneMethod");
	provRef = mgr.getMethodProvider("root", c7, m);
	unitAssert(provRef);
	provRef = mgr.getMethodProvider("root/foo", c7, m);
	unitAssert(provRef);
	provRef = mgr.getMethodProvider("root/foo", c7, CIMMethod("BadMethod"));
	unitAssert(!provRef);
	
	// self-registering provider one namespaces - one method
	CIMClass c8("SelfRegOneNamespaceOneMethod");
	provRef = mgr.getMethodProvider("root", c8, m);
	unitAssert(provRef);
	provRef = mgr.getMethodProvider("Root", c8, CIMMethod("testMETHOD"));
	unitAssert(provRef);
	provRef = mgr.getMethodProvider("root/foo", c8, m);
	unitAssert(!provRef);
	provRef = mgr.getMethodProvider("root", c8, CIMMethod("BadMethod"));
	unitAssert(!provRef);
	provRef = mgr.getMethodProvider("root/foo", c8, CIMMethod("BadMethod"));
	unitAssert(!provRef);
	
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
void OW_ProviderManagerTestCases::testGetAssociatorProvider()
{
	ProviderManager mgr;
	mgr.load(testCreateMuxLoader(), g_testEnvironment);
	LocalOperationContext context;
	LocalCIMOMHandle hdl = LocalCIMOMHandle(CIMOMEnvironmentRef(), RepositoryIFCRef(), context);
	mgr.init(createServiceEnvRef());

	// test qualifier on class
	CIMClass c1("TestClass");
	CIMQualifier provQual(CIMQualifier::CIM_QUAL_PROVIDER);
	provQual.setValue(CIMValue("lib1::TestAssociatorProvider"));
	c1.addQualifier(provQual);
	AssociatorProviderIFCRef provRef = mgr.getAssociatorProvider("root", c1);
	unitAssert(provRef);

	// bad qualifier
	CIMClass c2("TestClass");
	provQual.setValue(CIMValue("lib1::bad"));
	c2.addQualifier(provQual);
	unitAssertThrows(provRef = mgr.getAssociatorProvider("root", c2));

	// self-registering provider all namespaces
	CIMClass c3("SelfReg");
	provRef = mgr.getAssociatorProvider("root", c3);
	unitAssert(provRef);
	provRef = mgr.getAssociatorProvider("root/foo", c3);
	unitAssert(provRef);
	
	// self-registering provider two namespaces
	CIMClass c4("SelfRegTwoNamespaces");
	provRef = mgr.getAssociatorProvider("root", c4);
	unitAssert(provRef);
	provRef = mgr.getAssociatorProvider("root/foo", c4);
	unitAssert(!provRef);
	provRef = mgr.getAssociatorProvider("root/good", c4);
	unitAssert(provRef);
	
	// nothing
	CIMClass c5("Nothing");
	provRef = mgr.getAssociatorProvider("root", c5);
	unitAssert(!provRef);

	// self-registering provider all namespaces - case insensitivity
	CIMClass c6("selFreG");
	provRef = mgr.getAssociatorProvider("Root", c6);
	unitAssert(provRef);
	provRef = mgr.getAssociatorProvider("rooT/fOo", c6);
	unitAssert(provRef);
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

void OW_ProviderManagerTestCases::testGetIndicationProvider()
{
	ProviderManager mgr;
	mgr.load(testCreateMuxLoader(), g_testEnvironment);
	LocalOperationContext context;
	LocalCIMOMHandle hdl = LocalCIMOMHandle(CIMOMEnvironmentRef(), RepositoryIFCRef(), context);
	mgr.init(createServiceEnvRef());

	CIMNameArray noLifeCycleClasses;
	// self-registering provider all namespaces
	IndicationProviderIFCRefArray provRefs = mgr.getIndicationProviders(
		"root", "SelfReg", noLifeCycleClasses);
	unitAssert(provRefs.size() == 1);
	provRefs = mgr.getIndicationProviders(
		"root/foo", "SelfReg", noLifeCycleClasses);
	unitAssert(provRefs.size() == 1);
	
	provRefs = mgr.getIndicationProviders(
		"root", "SelfReg2", noLifeCycleClasses);
	unitAssert(provRefs.size() == 2);
	provRefs = mgr.getIndicationProviders(
		"root/foo", "SelfReg2", noLifeCycleClasses);
	unitAssert(provRefs.size() == 2);
	
	// self-registering provider two namespaces
	String c4("SelfRegTwoNamespaces");
	provRefs = mgr.getIndicationProviders(
		"root", c4, noLifeCycleClasses);
	unitAssert(provRefs.size() == 1);
	provRefs = mgr.getIndicationProviders(
		"root/foo", c4, noLifeCycleClasses);
	unitAssert(provRefs.size() == 0);
	provRefs = mgr.getIndicationProviders(
		"root/good", c4, noLifeCycleClasses);
	unitAssert(provRefs.size() == 1);
	
	// nothing
	String c5("Nothing");
	provRefs = mgr.getIndicationProviders(
		"root", c5, noLifeCycleClasses);
	unitAssert(provRefs.size() == 0);

	// self-registering provider all namespaces - case insensitivity
	String c6("selFreG");
	provRefs = mgr.getIndicationProviders(
		"Root", c6, noLifeCycleClasses);
	unitAssert(provRefs.size() == 1);
	provRefs = mgr.getIndicationProviders(
		"rooT/fOo", c6, noLifeCycleClasses);
	unitAssert(provRefs.size() == 1);

	// lifecycle providers
	CIMNameArray lifeCycleClasses;
	lifeCycleClasses.push_back("TestClass1");
	provRefs = mgr.getIndicationProviders(
		"root/cimv2", "CIM_InstCreation", lifeCycleClasses);
	unitAssert(provRefs.size() == 1);
	
	lifeCycleClasses[0] = "TestClass2";
	provRefs = mgr.getIndicationProviders(
		"root/cimv2", "CIM_InstModification", lifeCycleClasses);
	unitAssert(provRefs.size() == 1);
	
	lifeCycleClasses[0] = "TestClass3";
	provRefs = mgr.getIndicationProviders(
		"root/cimv2", "CIM_InstDeletion", lifeCycleClasses);
	unitAssert(provRefs.size() == 1);

	provRefs = mgr.getIndicationProviders(
		"root/cimv2", "CIM_InstDeletion", noLifeCycleClasses);
	unitAssert(provRefs.size() == 2);
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

