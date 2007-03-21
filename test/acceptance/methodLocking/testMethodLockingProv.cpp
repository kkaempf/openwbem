/*******************************************************************************
* Copyright (C) 2007 Quest Software. All rights reserved.
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
*  - Neither the name of Quest Software nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software OR THE CONTRIBUTORS
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


#include "OW_config.h"
#include "OW_CppProviderIncludes.hpp"
#include "OW_FileSystem.hpp"
#include "OW_Thread.hpp" // for yield()
#include "OW_Assertion.hpp"
#include "OW_Timeout.hpp"
#include "OW_TimeoutTimer.hpp"
#include "OW_NonRecursiveMutex.hpp"
#include "OW_NonRecursiveMutexLock.hpp"
#include "testMethodLocking.hpp"

using namespace std;
using namespace OpenWBEM;
using namespace WBEMFlags;

namespace
{
const String COMPONENT_NAME("ow.test.testMethodLockingProv");

class ProviderEnvCIMOMHandleSource : public MethodLocking::CIMOMHandleSource
{
public:
	ProviderEnvCIMOMHandleSource(const ProviderEnvironmentIFCRef& env)
	: m_env(env)
	{
	}

	virtual CIMOMHandleIFCRef getCIMOMHandle() const
	{
		// ProviderEnvironments aren't thread safe
		NonRecursiveMutexLock lock(m_mtx);
		ProviderEnvironmentIFCRef clonedEnv(m_env->clone());
		m_clonedEnvs.push_back(clonedEnv);
		return clonedEnv->getCIMOMHandle();
	}

	void clearClonedEnvs()
	{
		m_clonedEnvs.clear();
	}
private:
	ProviderEnvironmentIFCRef m_env;
	mutable Array<ProviderEnvironmentIFCRef> m_clonedEnvs; // we need to keep them around for a while
	mutable NonRecursiveMutex m_mtx;
};


class testMethodLockingProv: public CppMethodProviderIFC
{
public:
	testMethodLockingProv()
	{
	}

	virtual ~testMethodLockingProv(){}

	void getMethodProviderInfo(MethodProviderInfo& info)
	{
		StringArray namespaces(1, "root/methodLockingTest-cpp");
		StringArray methods;
		info.addInstrumentedClass(MethodProviderInfo::ClassInfo("no", namespaces, methods));
		info.addInstrumentedClass(MethodProviderInfo::ClassInfo("read", namespaces, methods));
		info.addInstrumentedClass(MethodProviderInfo::ClassInfo("write", namespaces, methods));
	}

	virtual ELockType getLockTypeForMethod(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& path,
		const String& methodName,
		const CIMParamValueArray& in)
	{
		Logger logger(COMPONENT_NAME);
		if (path.getClassName() == "no")
		{
			OW_LOG_DEBUG(logger, "got classname no, returning E_NO_LOCK");
			return CppMethodProviderIFC::E_NO_LOCK;
		}
		else if (path.getClassName() == "read")
		{
			OW_LOG_DEBUG(logger, "got classname read, returning E_READ_LOCK");
			return CppMethodProviderIFC::E_READ_LOCK;
		}
		else if (path.getClassName() == "write")
		{
			OW_LOG_DEBUG(logger, "got classname write, returning E_WRITE_LOCK");
			return CppMethodProviderIFC::E_WRITE_LOCK;
		}
		else
		{
			abort();
		}
	}

	CIMValue invokeMethod(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& path,
		const String &methodName,
		const CIMParamValueArray &in,
		CIMParamValueArray &out)
	{
		Logger lgr(COMPONENT_NAME);
		OW_LOG_DEBUG(lgr, Format("testMethodLockingProv::invokeMethod() methodName = %1", methodName));
		if (methodName == "wait")
		{
			String startedFile = in[0].getValue().toString();
			String waitForFile = in[1].getValue().toString();
			OW_LOG_DEBUG(lgr, Format("startedFile = \"%1\", waitForFile = \"%2\"", startedFile, waitForFile));
			if (!FileSystem::createFile(startedFile)) // this is the signal to the caller that the method has started to wait
			{
				OW_LOG_ERROR(lgr, Format("Failed to create %1: %2", startedFile, strerror(errno)));
			}
			OW_LOG_DEBUG(lgr, "created startedFile, waiting for existence of waitForFile");
			Timeout to = Timeout::relative(10.0);
			TimeoutTimer timer(to);
			do
			{
				Thread::yield();
				timer.loop();
			} while (!FileSystem::exists(waitForFile) && !timer.expired());
			if (!FileSystem::exists(waitForFile))
			{
				OW_LOG_DEBUG(lgr, "timed out waiting for waitForFile. Exiting with failure.");
				return CIMValue(false);
			}
			OW_LOG_DEBUG(lgr, "waitForFile now exists. Exiting.");
			return CIMValue(true);
		}
		else if (methodName == "test")
		{
			return CIMValue(true);
		}
		else if (methodName == "runAllTestsInProvider")
		{
			String testDir = in[0].getValue().toString();
			// the tests rely on the current working directory to place the syncronization files. By default OOP providers have the cwd set to /. 
			// It needs to be set to the test stage dir so it can be written to.
			FileSystem::changeDirectory(testDir);
			ProviderEnvCIMOMHandleSource chs(env);
			const char* const cppNS = "root/methodLockingTest-cpp";
			const char* const oopNS = "root/methodLockingTest-oop";
			const char* const no = "no";
			const char* const no2 = "no2";
			const char* const read = "read";
			const char* const write = "write";
			int testCount = 1;
#define TEST_ASSERT(CON) OW_LOG_DEBUG(lgr, Format("About to run test %1: %2", testCount++, #CON)); if (!(CON)) throw AssertionException(__FILE__, __LINE__, #CON); chs.clearClonedEnvs()
			// first oop
			TEST_ASSERT(MethodLocking::test(oopNS, no, read, chs));
			TEST_ASSERT(MethodLocking::test(oopNS, no, read, read, chs));
			TEST_ASSERT(MethodLocking::test(oopNS, no, write, chs));
			TEST_ASSERT(MethodLocking::test(oopNS, no, write, no, chs));
			TEST_ASSERT(MethodLocking::test(oopNS, read, read, chs));
			TEST_ASSERT(MethodLocking::test(oopNS, read, read, read, chs));
			TEST_ASSERT(MethodLocking::test(oopNS, write, no, chs));
			TEST_ASSERT(MethodLocking::test(oopNS, no2, no, chs));
			// cpp
			TEST_ASSERT(MethodLocking::test(cppNS, no, no, chs));
			TEST_ASSERT(MethodLocking::test(cppNS, no, write, chs));
			TEST_ASSERT(!MethodLocking::test(cppNS, read, no, write, chs));
			TEST_ASSERT(!MethodLocking::test(cppNS, write, read, chs));
			return CIMValue(true);
		}

		OW_THROWCIMMSG(CIMException::METHOD_NOT_AVAILABLE, Format("Provider hasn't implemented %1", methodName).c_str());
	}

};


} // end unnamed namespace



//////////////////////////////////////////////////////////////////////////////

OW_PROVIDERFACTORY(testMethodLockingProv, methodLockingTest)



