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

using namespace std;
using namespace OpenWBEM;
using namespace WBEMFlags;

namespace
{
const String COMPONENT_NAME("ow.test.testMethodLockingProv");

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
			FileSystem::createFile(startedFile); // this is the signal to the caller that the method has started to wait
			OW_LOG_DEBUG(lgr, "created startedFile, waiting for existence of waitForFile");
			do
			{
				Thread::yield();
			} while (!FileSystem::exists(waitForFile));
			OW_LOG_DEBUG(lgr, "waitForFile now exists. Exiting.");
			return CIMValue(Int32(0));
		}
		else if (methodName == "test")
		{
			return CIMValue(Int32(0));
		}

		OW_THROWCIMMSG(CIMException::METHOD_NOT_AVAILABLE, Format("Provider hasn't implemented %1", methodName).c_str());
	}

};


} // end unnamed namespace



//////////////////////////////////////////////////////////////////////////////

OW_PROVIDERFACTORY(testMethodLockingProv, methodLockingTest)



