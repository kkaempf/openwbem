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

namespace
{
class LinuxProcess : public OW_CppInstanceProviderIFC,
	public OW_CppMethodProviderIFC, public OW_CppIndicationProviderIFC
{
public:

	////////////////////////////////////////////////////////////////////////////
	virtual ~LinuxProcess()
	{
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void enumInstanceNames(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_CIMClass& cimClass )
	{
		(void)cimClass;
		(void)env;
		OW_String cmd = "/bin/ps ax --no-heading -eo pid,comm,vsz";
		OW_PopenStreams pos = OW_Exec::safePopen(cmd.tokenize());

		OW_String output = pos.out()->readAll();

		if (pos.getExitStatus() != 0)
		{
			//OW_THROWCIMMSG(OW_CIMException::FAILED, "Bad exit status from popen");
		}

		OW_StringArray lines = output.tokenize("\n");
		for (OW_StringArray::const_iterator iter = lines.begin();
			iter != lines.end(); iter++)
		{
			OW_StringArray proc = iter->tokenize();
			OW_CIMObjectPath newCop(className, ns);
			newCop.addKey(OW_String("Handle"), OW_CIMValue(proc[0]));
			newCop.addKey(OW_String("CreationClassName"),
				OW_CIMValue(className));
			newCop.addKey(OW_String("OSName"), OW_CIMValue(OW_String("Linux")));
			newCop.addKey(OW_String("OSCreationClassName"),
				OW_CIMValue(OW_String("CIM_OperatingSystem")));
			newCop.addKey(OW_String("CSName"),
				OW_CIMValue(OW_SocketAddress::getAnyLocalHost().getName()));
			newCop.addKey(OW_String("CSCreationClassName"),
				OW_CIMValue(OW_String("CIM_ComputerSystem")));
			result.handle(newCop);
		}
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void enumInstances(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_String& className,
		OW_CIMInstanceResultHandlerIFC& result,
		OW_Bool localOnly, 
		OW_Bool deep, 
		OW_Bool includeQualifiers, 
		OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList,
		const OW_CIMClass& requestedClass,
		const OW_CIMClass& cimClass )
	{
		(void)env;
		(void)ns;

		OW_String cmd = "/bin/ps ax --no-heading -eo pid,vsize,pcpu,comm";
		OW_PopenStreams pos = OW_Exec::safePopen(cmd.tokenize());
		OW_String output = pos.out()->readAll();
		if (pos.getExitStatus() != 0)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, "Bad exit status from popen");
		}

		OW_StringArray lines = output.tokenize("\n");
		for (OW_StringArray::const_iterator iter = lines.begin();
			iter != lines.end(); iter++)
		{
			OW_StringArray proc = iter->tokenize();
			OW_CIMInstance newInst = cimClass.newInstance();
			newInst.setProperty(OW_String("CreationClassName"),
				OW_CIMValue(className));
			newInst.setProperty(OW_String("Handle"), OW_CIMValue(proc[0]));
			newInst.setProperty(OW_String("OSName"), OW_CIMValue(OW_String("Linux")));
			newInst.setProperty(OW_String("OSCreationClassName"),
				OW_CIMValue(OW_String("CIM_OperatingSystem")));
			newInst.setProperty(OW_String("CSName"),
				OW_CIMValue(OW_SocketAddress::getAnyLocalHost().getName()));
			newInst.setProperty(OW_String("CSCreationClassName"),
				OW_CIMValue(OW_String("CIM_ComputerSystem")));
			newInst.setProperty(OW_String("Name"), OW_CIMValue(proc[3]));
			try
			{
				newInst.setProperty(OW_String("VirtualMemorySize"), OW_CIMValue(proc[1].toUInt32()));
				newInst.setProperty(OW_String("PercentCPU"), OW_CIMValue(proc[2].toReal32()));
			}
			catch (const OW_StringConversionException& e)
			{
				OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider failed parsing output from ps");
			}
			result.handle(newInst.clone(localOnly,deep,includeQualifiers,
				includeClassOrigin,propertyList,requestedClass,cimClass));
		}
	}

	////////////////////////////////////////////////////////////////////////////
	virtual OW_CIMInstance getInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& instanceName,
		OW_Bool localOnly,
		OW_Bool includeQualifiers, 
		OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList, 
		const OW_CIMClass& cimClass )
	{
		(void)env;
		(void)ns;
		(void)cimClass;
		OW_CIMInstance inst = cimClass.newInstance();
		inst.setProperties(instanceName.getKeys());
		OW_String pid;
		inst.getProperty("Handle").getValue().get(pid);
		OW_String cmd("/bin/ps p ");
		cmd += pid;
		cmd += " --no-headers -o vsize,pcpu,comm";
		OW_PopenStreams pos = OW_Exec::safePopen(cmd.tokenize());
		OW_StringArray proc = pos.out()->readAll().tokenize();
		if (pos.getExitStatus() != 0)
		{
			OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
				"The Instance does not (any longer) exist");
		}
		inst.setProperty(OW_String("Name"), OW_CIMValue(proc[2]));
		try
		{
			inst.setProperty(OW_String("VirtualMemorySize"), OW_CIMValue(proc[0].toUInt32()));
			inst.setProperty(OW_String("PercentCPU"), OW_CIMValue(proc[1].toReal32()));
		}
		catch (const OW_StringConversionException& e)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider failed parsing output from ps");
		}
		return inst.clone(localOnly,includeQualifiers,includeClassOrigin,propertyList);
	}

	////////////////////////////////////////////////////////////////////////////
	virtual OW_CIMObjectPath createInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMInstance& cimInstance )
	{
		(void)env;
		(void)ns;
		(void)cimInstance;
        OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support createInstance");
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void modifyInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMInstance& modifiedInstance,
		const OW_CIMInstance& previousInstance,
		OW_Bool includeQualifiers,
		const OW_StringArray* propertyList,
		const OW_CIMClass& theClass)
	{
		(void)env;
		(void)ns;
		(void)modifiedInstance;
		(void)previousInstance;
		(void)includeQualifiers;
		(void)propertyList;
		(void)theClass;
        OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support modifyInstance");
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void deleteInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& cop)
	{
		(void)env;
		(void)ns;
		(void)cop;
        OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support deleteInstance");
	}


	////////////////////////////////////////////////////////////////////////////
	virtual OW_CIMValue invokeMethod(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& path,
		const OW_String& methodName,
		const OW_CIMParamValueArray& in,
		OW_CIMParamValueArray& out )
	{
		(void)env;
		(void)ns;
		(void)out;
		if (methodName.equalsIgnoreCase("sendsignal"))
		{
			OW_Int32 sig;
			in[0].getValue().get(sig);
			OW_CIMPropertyArray keys = path.getKeys();
			pid_t pid = 0;
			for (OW_CIMPropertyArray::const_iterator iter = keys.begin();
				iter != keys.end(); iter++)
			{
				if (iter->getName().equalsIgnoreCase("handle"))
				{
					OW_String spid;
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
					return OW_CIMValue(OW_String("Success"));
				}
				else
				{
					return OW_CIMValue(OW_String("Fail"));
				}
			}

		}
		else
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, format("Provider does not support "
				"method: %1", methodName).c_str());
		}
		return OW_CIMValue(OW_String("Fail"));
	}

	virtual int mustPoll(const OW_ProviderEnvironmentIFCRef&,
			const OW_WQLSelectStatement&, const OW_String&, const OW_String&,
			const OW_StringArray&)
	{
		return 1; // The poll interval in seconds.  0 means no polling.  1 is very resource intensive, so is only appropriate for this sample.
	}

	virtual void getProviderInfo(OW_IndicationProviderInfo& info)
	{
		// all the life-cycle indications that may be generated by this provider
		OW_IndicationProviderInfoEntry e("CIM_InstCreation");
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
	virtual void getProviderInfo(OW_InstanceProviderInfo&)
	{
	}

	// This doesn't need to do anything, it's just here to keep the compiler from spitting out warnings.
	virtual void getProviderInfo(OW_MethodProviderInfo&)
	{
	}

};


}




OW_PROVIDERFACTORY(LinuxProcess, linuxprocess)



	
