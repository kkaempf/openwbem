/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
* Copyright (C) 2004 Novell, Inc. All rights reserved.
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
/**
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_ProviderAgent.hpp"
#include "OW_String.hpp"
#include "OW_Array.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_ConfigOpts.hpp"
#include "OW_Semaphore.hpp"
#include "OW_XMLExecute.hpp"
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMQualifier.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_NonAuthenticatingAuthenticator.hpp"
#include <signal.h>
#include <iostream> // for cout and cerr

using namespace OpenWBEM;
using namespace WBEMFlags;

using std::cout;
using std::endl;
using std::cerr;

class RPALogger : public Logger
{
protected:
	virtual void doLogMessage(const String &message, const ELogLevel) const
	{
		cerr << message << endl;
	}
};

// TODO: Using a Semaphore from a signal handler isn't safe.  It could deadlock.
// Figure out a better way to do it.
Semaphore shutdownSem;
extern "C"
void sig_handler(int)
{
	shutdownSem.signal();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

struct TestInstanceData
{
	String name;
	StringArray params;
};

static Array<TestInstanceData> g_saa;

class TestInstance: public CppInstanceProviderIFC
{
public:
	virtual ~TestInstance(){}

	void getInstanceProviderInfo(InstanceProviderInfo& info)
	{
		info.addInstrumentedClass("TestInstance");
	}

//////////////////////////////////////////////////////////////////////////////
	void
		enumInstanceNames(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		const CIMClass& cimClass )
	{
		(void)env;
		(void)cimClass;
		for (Array<TestInstanceData>::const_iterator iter = g_saa.begin();
			iter != g_saa.end(); iter++)
		{
			CIMObjectPath instCop(className, ns);
			instCop.setKeyValue("Name", CIMValue(iter->name));
			result.handle(instCop);
		}
	}

//////////////////////////////////////////////////////////////////////////////
	void
		enumInstances(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMInstanceResultHandlerIFC& result,
		ELocalOnlyFlag localOnly, 
		EDeepFlag deep, 
		EIncludeQualifiersFlag includeQualifiers, 
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList,
		const CIMClass& requestedClass,
		const CIMClass& cimClass )
	{
		(void)ns;
		(void)className;
		(void)env;

		{
			TestInstanceData tid; 
			tid.name = "name"; 
			tid.params.push_back("val1"); 
			tid.params.push_back("val2"); 
			g_saa.push_back(tid); 
		}

		for (Array<TestInstanceData>::const_iterator iter = g_saa.begin();
			iter != g_saa.end(); iter++)
		{
			//CIMInstance inst = cimClass.newInstance();
			CIMInstance inst("TestInstance"); 
			CIMQualifier cq(CIMQualifier::CIM_QUAL_KEY); 
			cq.setValue(CIMValue(Bool(true))); 
			CIMProperty prop("Name", CIMDataType::STRING); 
			prop.addQualifier(cq); 
			prop.setValue(CIMValue(iter->name)); 
			inst.setProperty(prop); 
			prop = CIMProperty("Params", CIMDataType::STRING); 
			prop.setValue(CIMValue(iter->params)); 
			inst.setProperty(prop); 
			result.handle(inst); 
			//result.handle(inst.clone(localOnly,deep,includeQualifiers,includeClassOrigin,propertyList,requestedClass,cimClass));
		}
	}

//////////////////////////////////////////////////////////////////////////////

	CIMInstance
		getInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& instanceName,
		ELocalOnlyFlag localOnly,
		EIncludeQualifiersFlag includeQualifiers, 
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList, 
		const CIMClass& cimClass )
	{
		(void)ns;
		(void)env;
		CIMInstance rval = cimClass.newInstance();
		String name;
		instanceName.getKeys()[0].getValue().get(name);
		for (Array<TestInstanceData>::const_iterator iter = g_saa.begin();
			iter != g_saa.end(); iter++)
		{
			if (iter->name == name)
			{
				rval.setProperty("Name", CIMValue(name));
				rval.setProperty("Params", CIMValue(iter->params));
				break;
			}
		}
		return rval.clone(localOnly, includeQualifiers, includeClassOrigin, 
			propertyList);
	}

//////////////////////////////////////////////////////////////////////////////
	CIMObjectPath
		createInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& cimInstance )
	{

		(void)env;
		(void)ns;
		String name;
		StringArray params;
		cimInstance.getProperty("Name").getValue().get(name);

		for (Array<TestInstanceData>::const_iterator iter = g_saa.begin();
			iter != g_saa.end(); iter++)
		{
			if (iter->name == name)
			{
				OW_THROWCIM(CIMException::ALREADY_EXISTS);
				break;
			}
		}

		cimInstance.getProperty("Params").getValue().get(params);
		TestInstanceData newInst;
		newInst.name = name;
		newInst.params = params;
		g_saa.push_back(newInst);
		return CIMObjectPath(ns, cimInstance);
	}

//////////////////////////////////////////////////////////////////////////////
	void
		modifyInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& modifiedInstance,
		const CIMInstance& previousInstance,
		EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList,
		const CIMClass& theClass)
	{
		env->getLogger()->logDebug("TestInstance::modifyInstance");
		(void)ns;
		(void)previousInstance;
		(void)includeQualifiers;
		(void)propertyList;
		(void)theClass;
		String name;
		StringArray params;
		modifiedInstance.getProperty("Name").getValue().get(name);
		modifiedInstance.getProperty("Params").getValue().get(params);

		for (Array<TestInstanceData>::iterator iter = g_saa.begin();
			iter != g_saa.end(); iter++)
		{
			if (iter->name == name)
			{
				iter->params = params;
				return;
			}
		}
		// new instance
		TestInstanceData newInst;
		newInst.name = name;
		newInst.params = params;
		g_saa.push_back(newInst);
	}

//////////////////////////////////////////////////////////////////////////////
	void
		deleteInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& cop)
	{
		(void)env;
		(void)ns;
		String name;
		cop.getKeys()[0].getValue().get(name);
		for (Array<TestInstanceData>::iterator iter = g_saa.begin();
			iter != g_saa.end(); iter++)
		{
			if (iter->name == name)
			{
				g_saa.erase(iter);
				break;
			}
		}
	}


};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	try
	{
		signal(SIGINT, sig_handler);
		String url(argv[1]);
		String ns(argv[2]);
		String query(argv[3]);
		LoggerRef logger(new RPALogger);

		ConfigFile::ConfigMap cmap; 
		cmap[ConfigOpts::HTTP_PORT_opt] = String(0);
		cmap[ConfigOpts::HTTPS_PORT_opt] = String(-1);
		cmap[ConfigOpts::MAX_CONNECTIONS_opt] = String(10);
		cmap[ConfigOpts::SINGLE_THREAD_opt] = "false";
		cmap[ConfigOpts::ENABLE_DEFLATE_opt] = "true";
		cmap[ConfigOpts::HTTP_USE_DIGEST_opt] = "false";
		cmap[ConfigOpts::USE_UDS_opt] = "false";
		cmap[OpenWBEM::ConfigOpts::DUMP_SOCKET_IO_opt] = "/tmp/"; 

		Reference<AuthenticatorIFC> authenticator(new NonAuthenticatingAuthenticator); 
		RequestHandlerIFCRef rh(SharedLibraryRef(0), new XMLExecute); 
		Array<RequestHandlerIFCRef> rha; 
		rha.push_back(rh); 
		Reference<CppProviderBaseIFC> provider(new TestInstance); 
		ProviderAgent pa(cmap, provider, rha, authenticator, logger);
		// wait until we get a SIGINT
		shutdownSem.wait();
		cout << "Shutting down." << endl;
		pa.shutdownHttpServer();
		return 0;
	}
	catch(Exception& e)
	{
		cerr << e << endl;
	}
	catch(std::exception& e)
	{
		cerr << e.what() << endl;
	}
	catch(...)
	{
		cerr << "Caught unknown exception in main" << endl;
	}
	return 1;
}

