
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>


class RPMIP : public OW_CppInstanceProvider, public OW_CppMethodProvider
{
public:

   virtual ~RPMIP() {}

	RPMIP();

	/**
	 * Called by the CIMOM when the provider is initialized
	 * @param hdl The handle to the cimom
	 * @throws OW_CIMException
	 */
	virtual void initialize(OW_LocalCIMOMHandle& hdl); 

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


private:
	OW_String _pkgHandler;

	/** 
	 * Fill in the information on a package
	 *
	 * @param inst The instance to fill out.  Note that the keys
	 *             must already be present before passing it in.
	 * 
	 * @return True if successful, false if not (like the instance doesn't
	 *         exist)
	 */
	OW_Bool RPMIP::processPkg(OW_CIMInstance& inst);
};

//////////////////////////////////////////////////////////////////////////////
RPMIP::RPMIP()
	: _pkgHandler()
{
}

//////////////////////////////////////////////////////////////////////////////
void 
RPMIP::initialize(OW_LocalCIMOMHandle& hdl)
{
	(void)hdl;
	struct stat fs;
	if (stat("/usr/bin/dpkg", &fs) == 0)
	{
		_pkgHandler = "/usr/bin/isinstalled ";
	}
	else if (stat("/usr/bin/rpm", &fs) == 0)
	{
		_pkgHandler = "/usr/bin/rpm -q ";
	}
	else if (stat("/bin/rpm", &fs) == 0)
	{
		_pkgHandler = "/bin/rpm -q ";
	}
	else
	{
		_pkgHandler = "";
	}
}


//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration 
RPMIP::enumInstances(
   OW_LocalCIMOMHandle& hdl,
   OW_CIMObjectPath cop,
   OW_Bool deep, 
   OW_CIMClass cimClass )
{
	(void)deep;
	(void)cimClass;
	(void)hdl;

	OW_CIMObjectPathEnumeration rval;
	OW_String cmd = "/usr/bin/apt-cache search .*";
	OW_PopenStreams pos = OW_Exec::safePopen(cmd.tokenize());

	OW_StringArray lines = pos.out()->readAll().tokenize("\n");

	sort(lines.begin(), lines.end());

	int rv = pos.getExitStatus();
	cout << "*** getExitStatus() returned " << rv << endl;
	/*
	if (pos.getExitStatus() != 0)
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_SUPPORTED, "Bad exit status from popen");
	}
	*/


	for (OW_StringArray::const_iterator iter = lines.begin(); 
		  iter != lines.end(); ++iter)
	{
		OW_CIMObjectPath newCop = cop;
		newCop.addKey("Name", OW_CIMValue(iter->tokenize()[0]));
		rval.addElement(newCop);
	}

	return rval;
}


//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration 
RPMIP::enumInstances(
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

	OW_String cmd = "/bin/ps ax --no-heading -eo pid,comm";
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
		rval.addElement(newInst);
	}
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstance 
RPMIP::getInstance(
   OW_LocalCIMOMHandle& hdl,
   OW_CIMObjectPath cop,
   OW_CIMClass cimClass, 
   OW_Bool localOnly )
{
	(void)hdl;
	(void)cop;
	(void)cimClass;
	(void)localOnly;

	OW_CIMInstance rval = cimClass.newInstance();
	rval.setProperties(cop.getKeys());
	

	if (!processPkg(rval))
	{
		OW_THROWCIMMSG(OW_CIMException::NOT_FOUND, 
			"The Instance does not (any longer) exist");
	}

	cerr << "*** MOF" << endl;
	cerr << rval.toMOF() << endl;
	cerr << "*** end MOF " << endl;
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_Bool
RPMIP::processPkg(OW_CIMInstance& inst)
{
	OW_String name;
	inst.getProperty("Name").getValue().get(name);

	// get package details
	OW_String cmd = "/usr/bin/apt-cache show ";
	cmd += name;
	OW_PopenStreams pos = OW_Exec::safePopen(cmd.tokenize());

	if (pos.getExitStatus() != 0)
	{
		return OW_Bool(false);
	}
	// TODO parse output and add properties

	OW_String curName, value;
	OW_StringArray lines = pos.out()->readAll().tokenize("\n");
	for (OW_StringArray::const_iterator iter = lines.begin(); 
		  iter != lines.end(); ++iter)
	{
		if ((*iter)[0] == ' ')
		{
			if (curName.length())
			{
				value += (*iter);
			}
			else
			{
				continue;
			}
		}
		else
		{
			unsigned int idx = iter->indexOf(':');
			curName = iter->substring(0, idx);
			value = iter->substring(idx + 1);
			value.trim();
		}


		if (curName.equals("Depends")
			 || curName.equals("Depends")
			 || curName.equals("Suggests")
			 || curName.equals("Provides")
			 || curName.equals("Conflicts")
			 || curName.equals("Recommends"))

		{
			inst.setProperty(curName, OW_CIMValue(value.tokenize(",")));
		}

		else if (curName.equals("Size")
					|| curName.equals("Installed-Size"))
		{
			if (curName.equals("Installed-Size"))
			{
				curName = "Installed_Size";
			}
			inst.setProperty(curName, OW_CIMValue(value.toUInt32()));
		}

		else if (curName.equals("Name")
					|| curName.equals("Version")
					|| curName.equals("Architecture")
					|| curName.equals("Section")
					|| curName.equals("MD5Sum")
					|| curName.equals("Maintainer")
					|| curName.equals("Description")
					|| curName.equals("Priority")
					|| curName.equals("Filename")
					|| curName.equals("State")
					|| curName.equals("Caption")
					)
		{
			inst.setProperty(curName, OW_CIMValue(value));
		}
		else
		{
			curName = "";
			continue;
		}

	}

	// determine if package is installed.
	cmd = _pkgHandler + name;
	if (OW_Exec::safeSystem(cmd.tokenize()) == 0)
	{
		inst.setProperty("State", OW_CIMValue(OW_String("Installed")));
	}
	else
	{
		inst.setProperty("State", OW_CIMValue(OW_String("Available")));
	}

	return OW_Bool(true);
}





//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath 
RPMIP::createInstance(
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
RPMIP::setInstance(
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
RPMIP::deleteInstance(
   OW_LocalCIMOMHandle& hdl,
   OW_CIMObjectPath cop)
{
	(void)hdl;
	(void)cop;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue 
RPMIP::invokeMethod(
			OW_LocalCIMOMHandle& hdl,
			const OW_CIMObjectPath& cop,
			const OW_String& methodName, 
			const OW_CIMValueArray& in,
			OW_CIMValueArray& out )
{
	(void)hdl;
	(void)out;
	(void)in;

	OW_String pkgName;
	OW_CIMPropertyArray props = cop.getKeys();
	for (OW_CIMPropertyArray::const_iterator iter = props.begin(); 
		  iter != props.end(); ++iter)
	{
		if (iter->getName().equalsIgnoreCase("name"))
		{
			iter->getValue().get(pkgName);
			break;
		}
	}

	OW_StringArray cmd;
	cmd.push_back(OW_String("/usr/bin/apt-get"));
	cmd.push_back(OW_String("-y"));
	cmd.push_back(methodName);
	cmd.push_back(pkgName);

	int rc = OW_Exec::safeSystem(cmd);

	return OW_CIMValue(OW_Int32(rc));
}


OW_PROVIDERFACTORY(RPMIP)

