/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7, Inc nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Center 7, Inc OR THE CONTRIBUTORS
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
#include "CIMRepository2TestCases.hpp"
#include "OW_CIMRepository2.hpp"
#include "OW_Logger.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_OperationContext.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_ResultHandlers.hpp"

using namespace OpenWBEM;

void CIMRepository2TestCases::setUp()
{
	system("rm -rf /tmp/owcimrep2test");
}

void CIMRepository2TestCases::tearDown()
{
}

namespace {
	class testEnv : public ServiceEnvironmentIFC
	{
	public:
		virtual String getConfigItem(const String &name, const String &defRetVal="") const
		{
			return defRetVal;
		}
		virtual void setConfigItem(const String &item, const String &value, EOverwritePreviousFlag overwritePrevious=E_OVERWRITE_PREVIOUS)
		{
		}
		virtual void addSelectable(const SelectableIFCRef& obj, const SelectableCallbackIFCRef& cb)
		{
		}
		virtual void removeSelectable(const SelectableIFCRef& obj)
		{
		}
		virtual RequestHandlerIFCRef getRequestHandler(const String &id)
		{
			return RequestHandlerIFCRef();
		}
		virtual LoggerRef getLogger() const
		{
			return Logger::createLogger("", true);
		}
		virtual bool authenticate(String &userName, const String &info, String &details, OperationContext&)
		{
			return true;
		}
		virtual CIMOMHandleIFCRef getCIMOMHandle(OperationContext &context, ESendIndicationsFlag doIndications=E_SEND_INDICATIONS, EBypassProvidersFlag bypassProviders=E_USE_PROVIDERS)
		{
			return CIMOMHandleIFCRef();
		}
		virtual CIMInstanceArray getInteropInstances(const String& className) const
		{
			return CIMInstanceArray();
		}
		virtual void setInteropInstance(const CIMInstance& inst)
		{
		}

	};
}

void CIMRepository2TestCases::testopen()
{
	ServiceEnvironmentIFCRef env(new testEnv());
	CIMRepository2 r(env);
	unitAssertNoThrow(r.open("/tmp/owcimrep2test"));
	unitAssert(FileSystem::isDirectory("/tmp/owcimrep2test"));
	unitAssert(FileSystem::exists("/tmp/owcimrep2test/instancenamelists"));
	unitAssert(FileSystem::exists("/tmp/owcimrep2test/instances"));
	unitAssert(FileSystem::exists("/tmp/owcimrep2test/namespaces"));
	unitAssert(FileSystem::exists("/tmp/owcimrep2test/qualifiers"));
	unitAssert(FileSystem::exists("/tmp/owcimrep2test/classes"));
	unitAssert(FileSystem::exists("/tmp/owcimrep2test/subclasslists"));
	unitAssert(FileSystem::exists("/tmp/owcimrep2test/qualifierlists"));
	unitAssert(FileSystem::exists("/tmp/owcimrep2test/classassoc"));
	unitAssert(FileSystem::exists("/tmp/owcimrep2test/instanceassoc"));

	unitAssertNoThrow(r.close());

}

void CIMRepository2TestCases::testcreateNamespace()
{
	ServiceEnvironmentIFCRef env(new testEnv());
	CIMRepository2 r(env);
	r.open("/tmp/owcimrep2test");
	OperationContext context;
	unitAssertNoThrow(r.beginOperation(WBEMFlags::E_CREATE_NAMESPACE, context));
	unitAssertNoThrow(r.createNameSpace("test", context));
	unitAssertNoThrow(r.endOperation(WBEMFlags::E_CREATE_NAMESPACE, context, WBEMFlags::E_SUCCESS));
}

void CIMRepository2TestCases::testenumNamespaces()
{
	ServiceEnvironmentIFCRef env(new testEnv());
	CIMRepository2 r(env);
	r.open("/tmp/owcimrep2test");
	OperationContext context;
	unitAssertNoThrow(r.beginOperation(WBEMFlags::E_CREATE_NAMESPACE, context));
	unitAssertNoThrow(r.createNameSpace("test1", context));
	unitAssertNoThrow(r.endOperation(WBEMFlags::E_CREATE_NAMESPACE, context, WBEMFlags::E_SUCCESS));
	unitAssertNoThrow(r.beginOperation(WBEMFlags::E_CREATE_NAMESPACE, context));
	unitAssertNoThrow(r.createNameSpace("test2", context));
	unitAssertNoThrow(r.endOperation(WBEMFlags::E_CREATE_NAMESPACE, context, WBEMFlags::E_SUCCESS));
	unitAssertNoThrow(r.beginOperation(WBEMFlags::E_CREATE_NAMESPACE, context));
	unitAssertNoThrow(r.createNameSpace("test3", context));
	unitAssertNoThrow(r.endOperation(WBEMFlags::E_CREATE_NAMESPACE, context, WBEMFlags::E_SUCCESS));

	StringArray namespaces;
	StringArrayBuilder sab(namespaces);
	unitAssertNoThrow(r.beginOperation(WBEMFlags::E_CREATE_NAMESPACE, context));
	unitAssertNoThrow(r.enumNameSpace(sab, context));
	unitAssertNoThrow(r.endOperation(WBEMFlags::E_CREATE_NAMESPACE, context, WBEMFlags::E_SUCCESS));

	unitAssert(namespaces.size() == 3);
	unitAssert(namespaces[0] == "test1");
	unitAssert(namespaces[1] == "test2");
	unitAssert(namespaces[2] == "test3");
}

Test* CIMRepository2TestCases::suite()
{
	TestSuite *testSuite = new TestSuite ("CIMRepository2");

	ADD_TEST_TO_SUITE(CIMRepository2TestCases, testopen);
	ADD_TEST_TO_SUITE(CIMRepository2TestCases, testcreateNamespace);
	ADD_TEST_TO_SUITE(CIMRepository2TestCases, testenumNamespaces);

	return testSuite;
}

