
#include <sys/types.h>
#include <signal.h>
#include "OW_CppInstanceProvider.hpp"
#include "OW_CppMethodProvider.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMException.hpp"
#include "OW_Exec.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_CIMValue.hpp"
#include "OW_InetAddress.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_Format.hpp"


class LinuxProcess : public OW_CppInstanceProvider, public OW_CppMethodProvider
{
public:

   virtual ~LinuxProcess() {}

   /**
   * This method enumerates all names of instances of the class which is
   * specified in cop.
   *
   * @param cop The object path specifies the class that must be enumerated.
   * @param deep If true, deep enumeration is done, otherwise shallow.
   * @param cimClass The class reference
   *
   * @returns An array of OW_CIMObjectPath containing names of the
   * 	enumerated instances.
   * @throws OW_CIMException - throws in the CIMObjectPath is incorrect
   * 	or does not exist.
   */
   virtual OW_CIMObjectPathEnumeration enumInstances(
	  OW_LocalCIMOMHandle& hdl,
	  OW_CIMObjectPath cop,
	  OW_Bool deep, 
	  OW_CIMClass cimClass );

   /**
   * This method enumerates
   * all instances of the class which is specified in cop.  The entire
   * instances and not just the names are returned.  Deep or shallow
   * enumeration is possible.
   *
   * @param cop The object path specifies the class that must be
   * 	enumerated.
   *
   * @param deep If true, deep enumeration must be done, otherwise shallow.
   *
   * @param cimClass The class reference.
   *
   * @param localOnly If true, only the non-inherited properties are to be
   * 	returned, otherwise all properties are required.
   *
   * @returns An array of OW_CIMInstance containing names of the enumerated
   * 	instances.
   *
   * @throws OW_CIMException - thrown if cop is incorrect or does not exist.
   */
   virtual OW_CIMInstanceEnumeration enumInstances(
	  OW_LocalCIMOMHandle& hdl,
	  OW_CIMObjectPath cop,
	  OW_Bool deep, 
	  OW_CIMClass cimClass, 
	  OW_Bool localOnly );

   /**
   * This method retrieves the instance specified in the object path.
   *
   * @param cop The name of the instance to be retrieved.
   *
   * @param cimClass The class to which the instance belongs.  This is
   * 	useful for providers which do not want to create instances from
   * 	scratch.  They can call the class newInstance() routine to create
   * 	a template for the new instance.
   *
   * @param localOnly If true, only the non-inherited properties are to be
   * 	returned, otherwise all properties are required.
   *
   * @returns The retrieved instance
   *
   * @throws OW_CIMException - thrown if cop is incorrect or does not exist
   */
   virtual OW_CIMInstance getInstance(
	  OW_LocalCIMOMHandle& hdl,
	  OW_CIMObjectPath cop,
	  OW_CIMClass cimClass, 
	  OW_Bool localOnly );

   /**
   * This method creates the instance specified in the object path.  If the
   * instance does exist an OW_CIMException with ID CIM_ERR_ALREADY_EXISTS
   * must be thrown.  The parameter should be the instance name.
   *
   * @param cop The path to the instance to be set.  The import part in
   * 	this parameter is the namespace component.
   *
   * @param cimInstance The instance to be set
   *
   * @returns A CIM ObjectPath of the instance that was created.
   *
   * @throws OW_CIMException
   */
   virtual OW_CIMObjectPath createInstance(
	  OW_LocalCIMOMHandle& hdl,
	  OW_CIMObjectPath cop,
	  OW_CIMInstance cimInstance );

   /**
   * This method sets the instance specified in the object path.  If the
   * instance does not exist an OW_CIMException with ID CIM_ERR_NOT_FOUND
   * must be thrown.  The parameter should be the instance name.
   *
   * @param cop The path of the instance to be set.  The important part in
   * 	this parameter is the namespace component.
   *
   * @param cimInstance The instance to be set.
   *
   * @throws OW_CIMException
   */
   virtual void setInstance(
	  OW_LocalCIMOMHandle& hdl,
	  OW_CIMObjectPath cop,
	  OW_CIMInstance cimInstance);

   /**
   * This method deletes the instance specified in the object path
   *
   * @param cop The instance to be deleted
   *
   * @throws OW_CIMException
   */
   virtual void deleteInstance(
	  OW_LocalCIMOMHandle& hdl,
	  OW_CIMObjectPath cop);


	/**
	 * The CIMOM calls this method when the method specified in the parameters
	 * is to be invoked.
	 *
	 * @param cop Contains the path to the instance whose method must be
	 * 	invoked.
	 * @param methodName The name of the method.
	 * @param inParams An array of OW_CIMValues which are the input parameters
	 * 	for this method.
	 * @param outParams An array of OW_CIMValues which are the output
	 * 	parameters for this method.
	 *
	 * @returns OW_CIMValue - The return value of the method.  Must be a
	 *    valid OW_CIMValue.
	 *
	 * @throws OW_CIMException
	 */
	virtual OW_CIMValue invokeMethod(
			OW_LocalCIMOMHandle& hdl,
			const OW_CIMObjectPath& cop,
			const OW_String& methodName, 
			const OW_CIMValueArray& in,
			OW_CIMValueArray& out );

};



//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration 
LinuxProcess::enumInstances(
   OW_LocalCIMOMHandle& hdl,
   OW_CIMObjectPath cop,
   OW_Bool deep, 
   OW_CIMClass cimClass )
{
	(void)deep;
	(void)cimClass;
	(void)hdl;
	OW_CIMObjectPathEnumeration rval;
	OW_String cmd = "/bin/ps ax --no-heading -eo pid,comm,vsz";
	OW_PopenStreams pos = OW_Exec::safePopen(cmd.tokenize());

	OW_String output = pos.out()->readAll();

	if (pos.getExitStatus() != 0)
	{
		//OW_THROWCIMMSG(OW_CIMException::NOT_SUPPORTED, "Bad exit status from popen");
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
			OW_CIMValue(OW_InetAddress::getAnyLocalHost().getName()));
		newCop.addKey(OW_String("CSCreationClassName"), 
			OW_CIMValue(OW_String("CIM_ComputerSystem")));
		rval.addElement(newCop);
	}

	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration 
LinuxProcess::enumInstances(
   OW_LocalCIMOMHandle& hdl,
   OW_CIMObjectPath cop,
   OW_Bool deep, 
   OW_CIMClass cimClass, 
   OW_Bool localOnly )
{
	(void)hdl;
	(void)cop;
	(void)deep;
	(void)localOnly;
	OW_CIMInstanceEnumeration rval;

	OW_String cmd = "/bin/ps ax --no-heading -eo pid,comm,vsize,pcpu";
	OW_PopenStreams pos = OW_Exec::safePopen(cmd.tokenize());
	if (pos.getExitStatus() != 0)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_SUPPORTED, "Bad exit status from popen");
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
			OW_CIMValue(OW_InetAddress::getAnyLocalHost().getName()));
		newInst.setProperty(OW_String("CSCreationClassName"), 
			OW_CIMValue(OW_String("CIM_ComputerSystem")));
		newInst.setProperty(OW_String("Name"), OW_CIMValue(proc[1]));
		newInst.setProperty(OW_String("VirtualMemorySize"), OW_CIMValue(proc[2].toUInt32()));
		newInst.setProperty(OW_String("PercentCPU"), OW_CIMValue(proc[3].toUInt32()));
		rval.addElement(newInst);
	}
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance 
LinuxProcess::getInstance(
   OW_LocalCIMOMHandle& hdl,
   OW_CIMObjectPath cop,
   OW_CIMClass cimClass, 
   OW_Bool localOnly )
{
	(void)hdl;
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
	inst.setProperty(OW_String("VirtualMemorySize"), OW_CIMValue(proc[1].toUInt32()));
	inst.setProperty(OW_String("PercentCPU"), OW_CIMValue(proc[2].toUInt32()));
	return inst;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath 
LinuxProcess::createInstance(
   OW_LocalCIMOMHandle& hdl,
   OW_CIMObjectPath cop,
   OW_CIMInstance cimInstance )
{
   (void)hdl;
   (void)cop;
   (void)cimInstance;
   OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
}

//////////////////////////////////////////////////////////////////////////////
void 
LinuxProcess::setInstance(
   OW_LocalCIMOMHandle& hdl,
   OW_CIMObjectPath cop,
   OW_CIMInstance cimInstance)
{
   (void)hdl;
   (void)cop;
   (void)cimInstance;
   OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
}

//////////////////////////////////////////////////////////////////////////////
void 
LinuxProcess::deleteInstance(
   OW_LocalCIMOMHandle& hdl,
   OW_CIMObjectPath cop)
{
	(void)hdl;
	(void)cop;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue 
LinuxProcess::invokeMethod(
			OW_LocalCIMOMHandle& hdl,
			const OW_CIMObjectPath& cop,
			const OW_String& methodName, 
			const OW_CIMValueArray& in,
			OW_CIMValueArray& out )
{
	(void)hdl;
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
			cerr << "Calling kill with pid " << pid << " and sig " << sig << endl;
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


OW_PROVIDERFACTORY(LinuxProcess)

