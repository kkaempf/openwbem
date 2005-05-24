/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All rights reserved.
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
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_CppProviderIncludes.hpp"

#include "OW_Exec.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_SocketAddress.hpp"

#include <iostream> // for cerr
#include <sys/types.h>
#include <signal.h>

using namespace OpenWBEM;
using namespace WBEMFlags;

namespace
{
class LinuxProcess : public CppInstanceProviderIFC,
	public CppMethodProviderIFC, public CppIndicationProviderIFC
{
public:

	////////////////////////////////////////////////////////////////////////////
	virtual ~LinuxProcess()
	{
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void enumInstanceNames(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		const CIMClass& cimClass )
	{
		String cmd = "/bin/ps ax --no-heading -eo pid,comm,vsz";
		PopenStreams pos = Exec::safePopen(cmd.tokenize());

		String output = pos.out()->readAll();

		if (pos.getExitStatus() != 0)
		{
			//OW_THROWCIMMSG(CIMException::FAILED, "Bad exit status from popen");
		}

		StringArray lines = output.tokenize("\n");
		for (StringArray::const_iterator iter = lines.begin();
			iter != lines.end(); iter++)
		{
			StringArray proc = iter->tokenize();
			CIMObjectPath newCop(className, ns);
			newCop.setKeyValue(String("Handle"), CIMValue(proc[0]));
			newCop.setKeyValue(String("CreationClassName"),
				CIMValue(className));
			newCop.setKeyValue(String("OSName"), CIMValue(String("Linux")));
			newCop.setKeyValue(String("OSCreationClassName"),
				CIMValue(String("CIM_OperatingSystem")));
			newCop.setKeyValue(String("CSName"),
				CIMValue(SocketAddress::getAnyLocalHost().getName()));
			newCop.setKeyValue(String("CSCreationClassName"),
				CIMValue(String("CIM_ComputerSystem")));
			result.handle(newCop);
		}
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void enumInstances(
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
		String cmd = "/bin/ps ax --no-heading -eo pid,vsize,pcpu,comm";
		PopenStreams pos = Exec::safePopen(cmd.tokenize());
		String output = pos.out()->readAll();
		if (pos.getExitStatus() != 0)
		{
			OW_THROWCIMMSG(CIMException::FAILED, "Bad exit status from popen");
		}

		StringArray lines = output.tokenize("\n");
		for (StringArray::const_iterator iter = lines.begin();
			iter != lines.end(); iter++)
		{
			StringArray proc = iter->tokenize();
			CIMInstance newInst = cimClass.newInstance();
			newInst.setProperty(String("CreationClassName"),
				CIMValue(className));
			newInst.setProperty(String("Handle"), CIMValue(proc[0]));
			newInst.setProperty(String("OSName"), CIMValue(String("Linux")));
			newInst.setProperty(String("OSCreationClassName"),
				CIMValue(String("CIM_OperatingSystem")));
			newInst.setProperty(String("CSName"),
				CIMValue(SocketAddress::getAnyLocalHost().getName()));
			newInst.setProperty(String("CSCreationClassName"),
				CIMValue(String("CIM_ComputerSystem")));
			newInst.setProperty(String("Name"), CIMValue(proc[3]));
			try
			{
				newInst.setProperty(String("VirtualMemorySize"), CIMValue(proc[1].toUInt32()));
				newInst.setProperty(String("PercentCPU"), CIMValue(proc[2].toReal32()));
			}
			catch (const StringConversionException& e)
			{
				OW_THROWCIMMSG_SUBEX(CIMException::FAILED, "Provider failed parsing output from ps", e);
			}
			result.handle(newInst.clone(localOnly,deep,includeQualifiers,
				includeClassOrigin,propertyList,requestedClass,cimClass));
		}
	}

	////////////////////////////////////////////////////////////////////////////
	virtual CIMInstance getInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& instanceName,
		ELocalOnlyFlag localOnly,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList,
		const CIMClass& cimClass )
	{
		CIMInstance inst = cimClass.newInstance();
		inst.updatePropertyValues(instanceName.getKeys());
		String pid;
		inst.getProperty("Handle").getValue().get(pid);
		String cmd("/bin/ps p ");
		cmd += pid;
		cmd += " --no-headers -o vsize,pcpu,comm";
		PopenStreams pos = Exec::safePopen(cmd.tokenize());
		StringArray proc = pos.out()->readAll().tokenize();
		if (pos.getExitStatus() != 0)
		{
			OW_THROWCIMMSG(CIMException::NOT_FOUND,
				"The Instance does not (any longer) exist");
		}
		inst.setProperty(String("Name"), CIMValue(proc[2]));
		try
		{
			inst.setProperty(String("VirtualMemorySize"), CIMValue(proc[0].toUInt32()));
			inst.setProperty(String("PercentCPU"), CIMValue(proc[1].toReal32()));
		}
		catch (const StringConversionException& e)
		{
			OW_THROWCIMMSG_SUBEX(CIMException::FAILED, "Provider failed parsing output from ps", e);
		}
		return inst.clone(localOnly,includeQualifiers,includeClassOrigin,propertyList);
	}

	////////////////////////////////////////////////////////////////////////////
	virtual CIMObjectPath createInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& cimInstance )
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support createInstance");
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void modifyInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& modifiedInstance,
		const CIMInstance& previousInstance,
		EIncludeQualifiersFlag includeQualifiers,
		const StringArray* propertyList,
		const CIMClass& theClass)
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support modifyInstance");
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void deleteInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& cop)
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support deleteInstance");
	}


	////////////////////////////////////////////////////////////////////////////
	virtual CIMValue invokeMethod(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& path,
		const String& methodName,
		const CIMParamValueArray& in,
		CIMParamValueArray& out )
	{
		if (methodName.equalsIgnoreCase("sendsignal"))
		{
			Int32 sig;
			in[0].getValue().get(sig);
			CIMPropertyArray keys = path.getKeys();
			pid_t pid = 0;
			for (CIMPropertyArray::const_iterator iter = keys.begin();
				iter != keys.end(); iter++)
			{
				if (iter->getName().equalsIgnoreCase("handle"))
				{
					String spid;
					iter->getValue().get(spid);
					pid = static_cast<pid_t>(spid.toInt32());
					break;
				}
			}

			if (pid != 0)
			{
				std::cerr << "Calling kill with pid " << pid << " and sig " << sig << std::endl;
				if (kill(pid, sig) == 0)
				{
					return CIMValue(String("Success"));
				}
				else
				{
					return CIMValue(String("Fail"));
				}
			}

		}
		else
		{
			OW_THROWCIMMSG(CIMException::FAILED, Format("Provider does not support "
				"method: %1", methodName).c_str());
		}
		return CIMValue(String("Fail"));
	}

	virtual int mustPoll(const ProviderEnvironmentIFCRef&,
			const WQLSelectStatement&, const String&, const String&,
			const StringArray&)
	{
		return 1; // The poll interval in seconds.  0 means no polling.  1 is very resource intensive, so is only appropriate for this sample.
	}

	virtual void getIndicationProviderInfo(IndicationProviderInfo& info)
	{
		// Add the class(es) we are monitoring for lifecycle indications. We only need to add the most derived classes to this array.
		const char* theMonitoredClasses[] =
			{
				"Linux_Process",
				0
			};

		// all the life-cycle indications that may be generated by this provider. We have to add all the base classes to this array.
		const char* instanceLifeCycleIndicationClassNames[] =
			{
				"CIM_InstCreation",
				"CIM_InstModification",
				"CIM_InstDeletion",
				"CIM_InstIndication",
				"CIM_Indication",
				0
			};

		for (const char** pIndicationClassName = instanceLifeCycleIndicationClassNames;
			  *pIndicationClassName != 0;
			  ++pIndicationClassName)
		{
			const char* indicationClassName = *pIndicationClassName;
			IndicationProviderInfoEntry e(indicationClassName);
			for (const char** pClassName = theMonitoredClasses;
				  *pClassName != 0;
				  ++pClassName)
			{
				const char* className = *pClassName;
				e.classes.push_back(className);
			}
			info.addInstrumentedClass(e);

		}
	}

	// This doesn't need to do anything, it's just here to keep the compiler from spitting out warnings.
	virtual void getInstanceProviderInfo(InstanceProviderInfo&)
	{
	}

	// This doesn't need to do anything, it's just here to keep the compiler from spitting out warnings.
	virtual void getMethodProviderInfo(MethodProviderInfo&)
	{
	}

};


}




OW_PROVIDERFACTORY(LinuxProcess, linuxprocess)



	
