/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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
*  - Neither the name of Center 7 nor the names of its
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

#include <sys/types.h>
#include <signal.h>
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CppMethodProviderIFC.hpp"
#include "OW_CppIndicationProviderIFC.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMException.hpp"
#include "OW_Exec.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_CIMValue.hpp"
#include "OW_SocketAddress.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_Format.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_CIMObjectPath.hpp"
#include <iostream> // for cerr

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
		(void)cimClass;
		(void)env;
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
		(void)env;
		(void)ns;

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
				OW_THROWCIMMSG(CIMException::FAILED, "Provider failed parsing output from ps");
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
		(void)env;
		(void)ns;
		(void)cimClass;
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
			OW_THROWCIMMSG(CIMException::FAILED, "Provider failed parsing output from ps");
		}
		return inst.clone(localOnly,includeQualifiers,includeClassOrigin,propertyList);
	}

	////////////////////////////////////////////////////////////////////////////
	virtual CIMObjectPath createInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& cimInstance )
	{
		(void)env;
		(void)ns;
		(void)cimInstance;
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
		(void)env;
		(void)ns;
		(void)modifiedInstance;
		(void)previousInstance;
		(void)includeQualifiers;
		(void)propertyList;
		(void)theClass;
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support modifyInstance");
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void deleteInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& cop)
	{
		(void)env;
		(void)ns;
		(void)cop;
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
		(void)env;
		(void)ns;
		(void)out;
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
			OW_THROWCIMMSG(CIMException::FAILED, format("Provider does not support "
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
		// all the life-cycle indications that may be generated by this provider
		IndicationProviderInfoEntry e("CIM_InstCreation");
		e.classes.push_back("Linux_Process");
		e.classes.push_back("CIM_Process");
		e.classes.push_back("CIM_EnabledLogicalElement");
		e.classes.push_back("CIM_LogicalElement");
		e.classes.push_back("CIM_ManagedSystemElement");
		e.classes.push_back("CIM_ManagedElement");

		info.addInstrumentedClass(e);
		e.indicationName = "CIM_InstModification";
		info.addInstrumentedClass(e);
		e.indicationName = "CIM_InstDeletion";
		info.addInstrumentedClass(e);
		e.indicationName = "CIM_InstIndication";
		info.addInstrumentedClass(e);
		e.indicationName = "CIM_Indication";
		info.addInstrumentedClass(e);
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



	
