
#include <sys/types.h>
#include <signal.h>
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CppMethodProviderIFC.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMException.hpp"
#include "OW_Exec.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_CIMValue.hpp"
#include "OW_SocketAddress.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_Format.hpp"
#include <iostream> // for cerr

namespace
{
class LinuxProcess : public OW_CppInstanceProviderIFC,
	public OW_CppMethodProviderIFC
{
public:

	////////////////////////////////////////////////////////////////////////////
	virtual ~LinuxProcess()
	{
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void enumInstanceNames(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& cop,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_Bool& deep,
		const OW_CIMClass& cimClass )
	{
		(void)deep;
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
			OW_CIMObjectPath newCop = cop;
			newCop.addKey(OW_String("Handle"), OW_CIMValue(proc[0]));
			newCop.addKey(OW_String("CreationClassName"),
				OW_CIMValue(cop.getObjectName()));
			newCop.addKey(OW_String("OSName"), OW_CIMValue(OW_String("Linux")));
			newCop.addKey(OW_String("OSCreationClassName"),
				OW_CIMValue(OW_String("CIM_OperatingSystem")));
			newCop.addKey(OW_String("CSName"),
				OW_CIMValue(OW_SocketAddress::getAnyLocalHost().getName()));
			newCop.addKey(OW_String("CSCreationClassName"),
				OW_CIMValue(OW_String("CIM_ComputerSystem")));
			result.handleObjectPath(newCop);
		}
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void enumInstances(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& cop,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_Bool& deep,
		const OW_CIMClass& cimClass,
		const OW_Bool& localOnly )
	{
		(void)env;
		(void)cop;
		(void)deep;
		(void)localOnly;

		OW_String cmd = "/bin/ps ax --no-heading -eo pid,comm,vsize,pcpu";
		OW_PopenStreams pos = OW_Exec::safePopen(cmd.tokenize());
		if (pos.getExitStatus() != 0)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, "Bad exit status from popen");
		}

		OW_String output = pos.out()->readAll();
		OW_StringArray lines = output.tokenize("\n");
		for (OW_StringArray::const_iterator iter = lines.begin();
			iter != lines.end(); iter++)
		{
			OW_StringArray proc = iter->tokenize();
			OW_CIMInstance newInst = cimClass.newInstance();
			newInst.setProperty(OW_String("CreationClassName"),
				OW_CIMValue(cop.getObjectName()));
			newInst.setProperty(OW_String("Handle"), OW_CIMValue(proc[0]));
			newInst.setProperty(OW_String("OSName"), OW_CIMValue(OW_String("Linux")));
			newInst.setProperty(OW_String("OSCreationClassName"),
				OW_CIMValue(OW_String("CIM_OperatingSystem")));
			newInst.setProperty(OW_String("CSName"),
				OW_CIMValue(OW_SocketAddress::getAnyLocalHost().getName()));
			newInst.setProperty(OW_String("CSCreationClassName"),
				OW_CIMValue(OW_String("CIM_ComputerSystem")));
			newInst.setProperty(OW_String("Name"), OW_CIMValue(proc[1]));
			try
			{
				newInst.setProperty(OW_String("VirtualMemorySize"), OW_CIMValue(proc[2].toUInt32()));
				newInst.setProperty(OW_String("PercentCPU"), OW_CIMValue(proc[3].toUInt32()));
			}
			catch (const OW_StringConversionException& e)
			{
				OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider failed parsing output from ps");
			}
			result.handleInstance(newInst);
		}
	}

	////////////////////////////////////////////////////////////////////////////
	virtual OW_CIMInstance getInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& cop,
		const OW_CIMClass& cimClass,
		const OW_Bool& localOnly )
	{
		(void)env;
		(void)cop;
		(void)cimClass;
		(void)localOnly;
		OW_CIMInstance inst = cimClass.newInstance();
		inst.setProperties(cop.getKeys());
		OW_String pid;
		inst.getProperty("Handle").getValue().get(pid);
		OW_String cmd("/bin/ps p ");
		cmd += pid;
		cmd += " --no-headers -o comm,vsize,pcpu";
		OW_PopenStreams pos = OW_Exec::safePopen(cmd.tokenize());
		if (pos.getExitStatus() != 0)
		{
			OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
				"The Instance does not (any longer) exist");
		}
		OW_StringArray proc = pos.out()->readAll().tokenize();
		inst.setProperty(OW_String("Name"), OW_CIMValue(proc[0]));
		try
		{
			inst.setProperty(OW_String("VirtualMemorySize"), OW_CIMValue(proc[1].toUInt32()));
			inst.setProperty(OW_String("PercentCPU"), OW_CIMValue(proc[2].toUInt32()));
		}
		catch (const OW_StringConversionException& e)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider failed parsing output from ps");
		}
		return inst;
	}

	////////////////////////////////////////////////////////////////////////////
	virtual OW_CIMObjectPath createInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& cop,
		const OW_CIMInstance& cimInstance )
	{
		(void)env;
		(void)cop;
		(void)cimInstance;
		OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void modifyInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& cop,
		const OW_CIMInstance& cimInstance)
	{
		(void)env;
		(void)cop;
		(void)cimInstance;
		OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
	}

	////////////////////////////////////////////////////////////////////////////
	virtual void deleteInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& cop)
	{
		(void)env;
		(void)cop;
		OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
	}


	////////////////////////////////////////////////////////////////////////////
	virtual OW_CIMValue invokeMethod(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_CIMObjectPath& cop,
		const OW_String& methodName,
		const OW_CIMValueArray& in,
		OW_CIMValueArray& out )
	{
		(void)env;
		(void)out;
		if (methodName.equalsIgnoreCase("sendsignal"))
		{
			OW_Int32 sig;
			in[0].get(sig);
			OW_CIMPropertyArray keys = cop.getKeys();
			pid_t pid = 0;
			for (OW_CIMPropertyArray::const_iterator iter = keys.begin();
				iter != keys.end(); iter++)
			{
				if (iter->getName().equalsIgnoreCase("handle"))
				{
					OW_String spid;
					iter->getValue().get(spid);
					pid = (pid_t)spid.toInt32();
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
			OW_THROWCIMMSG(OW_CIMException::METHOD_NOT_FOUND, format("Cannot find "
				"method: %1", methodName).c_str());
		}
		return OW_CIMValue(OW_String("Fail"));
	}

};


}




OW_PROVIDERFACTORY(LinuxProcess, linuxprocess)



	
