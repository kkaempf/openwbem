/*******************************************************************************
* Copyright (C) 2005 Vintela, Inc. All rights reserved.
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


#include "OW_config.h"
#include "OW_CppProviderIncludes.hpp"
#include "OW_SessionLanguage.hpp"

using namespace std;
using namespace OpenWBEM;
using namespace WBEMFlags;

namespace
{
const String COMPONENT_NAME("ow.test.testOOPProv");

OW_DECLARE_EXCEPTION(Test);
OW_DEFINE_EXCEPTION(Test);

class testOOPProv: public CppInstanceProviderIFC, 
	public CppMethodProviderIFC, 
	public CppIndicationProviderIFC,
	public CppPolledProviderIFC
{
public:
	testOOPProv()
		: m_indicationsActive(false)
	{
	}

	virtual ~testOOPProv(){}

	void getInstanceProviderInfo(InstanceProviderInfo& info)
	{
		info.addInstrumentedClass("ooptest");
	}

//////////////////////////////////////////////////////////////////////////////
	void enumInstanceNames(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		const CIMClass& cimClass )
	{
		Logger logger(COMPONENT_NAME);
		OW_LOG_DEBUG(logger, "testOOPProv::enumInstanceNames() start");

		for (size_t i = 0; i < 10; ++i)
		{
			CIMInstance ci = cimClass.newInstance();
			ci.updatePropertyValue("id", CIMValue(String(i)));
			result.handle(CIMObjectPath(ns, ci));
		}
		CIMInstance ci = cimClass.newInstance();
		ci.updatePropertyValue("id", CIMValue(String("AlreadyExists")));
		result.handle(CIMObjectPath(ns, ci));
	}

//////////////////////////////////////////////////////////////////////////////
	void enumInstances(
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
		Logger logger(COMPONENT_NAME);
		OW_LOG_DEBUG(logger, "testOOPProv::enumInstances() start");
		OW_LOG_DEBUG(logger, Format("env->getUserName() = %1" , env->getUserName()));
		if (env->getUserName() != "test1")
		{
			OW_LOG_DEBUG(logger, "aborting!");
			abort();
		}

		for (size_t i = 0; i < 10; ++i)
		{
			CIMInstance ci = cimClass.newInstance();
			ci.updatePropertyValue("id", CIMValue(String(i)));
			result.handle(ci.clone(localOnly, deep, includeQualifiers, includeClassOrigin, propertyList, requestedClass, cimClass));
		}
		CIMInstance ci = cimClass.newInstance();
		ci.updatePropertyValue("id", CIMValue(String("AlreadyExists")));
		result.handle(ci.clone(localOnly, deep, includeQualifiers, includeClassOrigin, propertyList, requestedClass, cimClass));
	}

//////////////////////////////////////////////////////////////////////////////

	CIMInstance getInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& instanceName,
		ELocalOnlyFlag localOnly,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList,
		const CIMClass& cimClass )
	{
		Logger logger(COMPONENT_NAME);
		OW_LOG_DEBUG(logger, "testOOPProv::getInstance() start");
		if (!idExists(instanceName.getKeyValue("id").toString()))
		{
			OW_THROWCIMMSG(CIMException::NOT_FOUND, Format("id = %1", instanceName.getKeyValue("id").toString()).c_str());
		}
		CIMInstance rval = cimClass.newInstance();
		rval.updatePropertyValue("id", instanceName.getKeyValue("id"));
		return rval.clone(localOnly, includeQualifiers, includeClassOrigin,
			propertyList);
	}

	bool idExists(const String& id)
	{
		if (id == "AlreadyExists")
		{
			return true;
		}
		Int32 iid = -1;
		try
		{
			iid = id.toInt32();
		}
		catch (StringConversionException&)
		{
		}
		if (iid < 0 || iid >= 10)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
//////////////////////////////////////////////////////////////////////////////
	CIMObjectPath createInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& cimInstance )
	{
		Logger logger(COMPONENT_NAME);
		OW_LOG_DEBUG(logger, "testOOPProv::createInstance() start");

		// special case so we can cause the mof compiler to call modifyInstance
		if (idExists(cimInstance.getPropertyValue("id").toString()))
		{
			OW_THROWCIM(CIMException::ALREADY_EXISTS);
		}
		return CIMObjectPath(ns, cimInstance);
	}

//////////////////////////////////////////////////////////////////////////////
	void modifyInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& modifiedInstance,
		const CIMInstance& previousInstance,
		EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList,
		const CIMClass& theClass)
	{
		Logger logger(COMPONENT_NAME);
		OW_LOG_DEBUG(logger, "testOOPProv::modifyInstance");
		
		if (!idExists(previousInstance.getPropertyValue("id").toString()))
		{
			OW_THROWCIM(CIMException::NOT_FOUND);
		}

	}

//////////////////////////////////////////////////////////////////////////////
	void deleteInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& cop)
	{
		Logger logger(COMPONENT_NAME);
		OW_LOG_DEBUG(logger, "testOOPProv::deleteInstance");
		if (!idExists(cop.getKeyValue("id").toString()))
		{
			OW_THROWCIM(CIMException::NOT_FOUND);
		}
	}


	virtual void getMethodProviderInfo(MethodProviderInfo& info)
	{
		info.addInstrumentedClass("ooptest");
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
		OW_LOG_DEBUG(lgr, Format("testOOPProv::invokeMethod() methodName = %1", methodName));
		if (methodName == "method1")
		{
			return CIMValue(String(Format("%1:%2", methodName, in[0].getValue())));
		}
		else if (methodName == "testUnknownException")
		{
			throw 1234;
		}
		else if (methodName == "testStdException")
		{
			throw std::exception();
		}
		else if (methodName == "testOWException")
		{
			OW_LOG_DEBUG(lgr, "About to throw TestException");
			OW_THROW(TestException, "test");
		}
		else if (methodName == "testCIMException")
		{
			OW_THROWCIMMSG(CIMException::FAILED, "test");
		}
		else if (methodName == "testExit")
		{
			exit(11);
		}
		else if (methodName == "testOperationContextGetStringData")
		{
			return CIMValue(env->getOperationContext().getStringData(in[0].getValue().toString()));
		}
		else if (methodName == "testOperationContextSessionLanguage")
		{
			return CIMValue(procAcceptLanguage(env));
		}
		else if (methodName == "testOperationContextSetStringData")
		{
			String key = in[0].getValue().toString();
			String value = in[1].getValue().toString();
			env->getOperationContext().setStringData(key, value);
            if (env->getOperationContext().getStringData(key) == value)
            {
                return CIMValue(true);
            }
            else
            {
                return CIMValue(false);
            }
		}
		else if (methodName == "testOperationContextRemoveData")
		{
			String key = in[0].getValue().toString();
			OperationContext& oc(env->getOperationContext());
			if (!oc.keyHasData(key))
			{
				oc.setStringData(key, "test");
			}
			oc.removeData(key);
			if (oc.keyHasData(key))
			{
				return CIMValue(false);
			}
			else
			{
				return CIMValue(true);
			}

		}
		else if (methodName == "testOperationContextKeyHasData")
		{
			String key = in[0].getValue().toString();
			OperationContext& oc(env->getOperationContext());
			return CIMValue(oc.keyHasData(key));
		}
		else if (methodName == "testCIMOMHandle")
		{
			CIMOMHandleIFCRef hdl = env->getCIMOMHandle();

			// test a WQL query
			CIMInstanceArray cia = hdl->execQueryA(ns, "select * from ooptest where id=\"1\"", "wql1");
			if (cia.size() != 1)
			{
				OW_THROWCIMMSG(CIMException::FAILED, Format("Expected one instance from WQL. Got %1", cia.size()).c_str());
			}

			// test some methods
			CIMParamValueArray inargs;
			inargs.push_back(CIMParamValue("arg1", CIMValue("param1")));
			CIMParamValueArray outargs;
			CIMValue rv = hdl->invokeMethod(ns, path, "method1", inargs, outargs);
			if (rv.toString() != "method1:param1")
			{
				return CIMValue(false);
			}
			// hit it a second time.
			inargs[0].setValue(CIMValue("param1-2"));
			rv = hdl->invokeMethod(ns, path, "method1", inargs, outargs);
			return CIMValue(rv.toString() == "method1:param1-2");
		}
		else if (methodName == "testRepositoryCIMOMHandle")
		{
			CIMOMHandleIFCRef hdl = env->getRepositoryCIMOMHandle();
			OW_LOG_DEBUG(lgr, "env->getRepositoryCIMOMHandle() returned");
			CIMParamValueArray inargs;
			inargs.push_back(CIMParamValue("arg1", CIMValue("param1")));
			CIMParamValueArray outargs;
			try
			{
				CIMValue rv = hdl->invokeMethod(ns, path, "method1", inargs, outargs);
				OW_LOG_DEBUG(lgr, Format("invokeMethod returned (incorrectly): %1", rv));
				// should only reach this if the expected throw doesn't happen
				return CIMValue(false);
			}
			catch (CIMException& e)
			{
				OW_LOG_DEBUG(lgr, Format("Caught CIMException: %1", e));
				if (e.getErrNo() != CIMException::NOT_SUPPORTED)
				{
					return CIMValue(false);
				}
			}

			// hit it a second time.
			OW_LOG_DEBUG(lgr, Format("calling getClass(%1, %2)", ns, path.getClassName()));
			CIMClass c = hdl->getClass(ns, path.getClassName());
			OW_LOG_DEBUG(lgr, Format("Retrieved class: %1", c.toString()));
			return CIMValue(true);
		}
		else if (methodName == "testGetConfig")
		{
			if (env->getConfigItem("log.main.type") != "file")
			{
				OW_LOG_DEBUG(lgr, Format("failed because log.main.type: %1", env->getConfigItem("log.main.type")));
				return CIMValue(false);
			}

			if (env->getConfigItem("nonexistent") != "")
			{
				OW_LOG_DEBUG(lgr, Format("failed because nonexistent: %1", env->getConfigItem("nonexistent")));
				return CIMValue(false);
			}

			if (env->getConfigItem("nonexistent", "a default") != "a default")
			{
				OW_LOG_DEBUG(lgr, Format("failed because nonexistent: %1", env->getConfigItem("nonexistent", "a default")));
				return CIMValue(false);
			}

			StringArray vals = env->getMultiConfigItem("owcimomd.allowed_users", StringArray());
			if (vals.size() != 1)
			{
				OW_LOG_DEBUG(lgr, Format("failed because vals.size(): %1", vals.size()));
				return CIMValue(false);
			}
			if (!vals[0].startsWith("test test1 test2 test3"))
			{
				OW_LOG_DEBUG(lgr, Format("failed because vals[0]: %1", vals[0]));
				return CIMValue(false);
			}

			vals = env->getMultiConfigItem("owcimomd.allowed_users", StringArray(), " ");
			if (vals.size() != 6)
			{
				OW_LOG_DEBUG(lgr, Format("failed because vals.size(): %1", vals.size()));
				return CIMValue(false);
			}
			if (vals[0] != "test")
			{
				OW_LOG_DEBUG(lgr, Format("failed because vals[0]: %1", vals[0]));
				return CIMValue(false);
			}

			return CIMValue(true);
		}

		OW_THROWCIMMSG(CIMException::METHOD_NOT_AVAILABLE, Format("Provider hasn't implemented %1", methodName).c_str());
	}

	String
	procAcceptLanguage(const ProviderEnvironmentIFCRef& env)
	{
		Logger logger(COMPONENT_NAME);
		OW_LOG_DEBUG(logger, "TestInstance::procAcceptLanguage");

		String al;
		OperationContext& oc(env->getOperationContext());
		SessionLanguageRef slref = oc.getDataAs<SessionLanguage>(OperationContext::SESSION_LANGUAGE_KEY);
		if (slref)
		{
			al = slref->getAcceptLanguageString();
			OW_LOG_DEBUG(logger, "testOOPProv::procAcceptLanguage"
				" setting content-language in SessionLanguage object");
			String cl = slref->getContentLanguage();
			OW_LOG_DEBUG(logger, Format(
				"TestInstance::procAcceptLanguage content-language now is %1", cl));
			slref->addContentLanguage("x-testinst");
			OW_LOG_DEBUG(logger, "setting session language");
			env->getOperationContext().setData(OperationContext::SESSION_LANGUAGE_KEY, slref);
		}
		else
		{
			OW_LOG_DEBUG(logger, "testOOPProv::procAcceptLanguage"
				" didn't find SessionLanguage object in opctx");
		}

		return al;
	}

	int mustPoll(
		const ProviderEnvironmentIFCRef & env, 
		const WQLSelectStatement & filter, 
		const String& eventType, 
		const String& nameSpace, 
		const StringArray& classes) 
	{
		return 0; // means don't poll enumInstances.
	}


	void activateFilter(
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter, 
		const String& eventType, 
		const String& nameSpace,
		const StringArray& classes, 
		bool firstActivation
		)
	{
		if (firstActivation)
		{
			m_indicationsActive = true;
		}
	}

	void authorizeFilter(
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter, 
		const String& eventType, 
		const String& nameSpace,
		const StringArray& classes, 
		const String& owner
		)
	{
		// do nothing, since we don't care who subscribes in this test
	}
	void deActivateFilter(
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter, 
		const String& eventType, 
		const String& nameSpace,
		const StringArray& classes, 
		bool lastActivation
		)
	{
		if (lastActivation)
		{
			m_indicationsActive = false;
		}
	}


	Int32 poll(const ProviderEnvironmentIFCRef& env)
	{
		Logger lgr(COMPONENT_NAME);
		OW_LOG_DEBUG(lgr, Format("testOOPProv m_indicationsActive = %1", m_indicationsActive));
		if (m_indicationsActive)
		{
			// export some indications
			CIMOMHandleIFCRef hdl = env->getCIMOMHandle();
			OperationContext& oc(env->getOperationContext());
			CIMInstance inst = hdl->getClass("root/ooptest", "ooptest").newInstance();

			/**/
			try
			{
				if (oc.getStringData("USER_NAME") != "test1")
				{
					oc.setStringData("USER_NAME", "test1");
				}
			}
			catch (ContextDataNotFoundException& e)
			{
				oc.setStringData("USER_NAME", "test1");
			}
			CIMInstanceArray insts = hdl->enumInstancesA("root/ooptest", "ooptest");
			/**/

			OW_LOG_DEBUG(lgr, "testOOPProv got class ooptest");
			inst.updatePropertyValue("id", CIMValue("indication"));
			CIMInstance expInst("CIM_InstCreation");
			expInst.setProperty("SourceInstance", CIMValue(inst));
			expInst.setProperty("IndicationTime", CIMValue(CIMDateTime(DateTime::getCurrent())));
			hdl->exportIndication(expInst, "root/ooptest");
			OW_LOG_DEBUG(lgr, "testOOPProv exported indication");

			return 1;
		}

		return 1;
	}
	
	Int32 getInitialPollingInterval(const ProviderEnvironmentIFCRef& env)
	{
		return 2;
	}

	bool m_indicationsActive;
};


} // end unnamed namespace



//////////////////////////////////////////////////////////////////////////////

OW_PROVIDERFACTORY(testOOPProv, ooptest)



