
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CppMethodProviderIFC.hpp"
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


class RPMIP : public OW_CppInstanceProviderIFC, public OW_CppMethodProviderIFC
{
public:

   virtual ~RPMIP() {}

   RPMIP();

   virtual void initialize(const OW_ProviderEnvironmentIFCRef&);

   virtual OW_CIMObjectPathEnumeration enumInstanceNames(
	  const OW_ProviderEnvironmentIFCRef& env,
	  OW_CIMObjectPath cop,
	  OW_Bool deep,
	  OW_CIMClass cimClass );


   virtual OW_CIMInstanceEnumeration enumInstances(
	  const OW_ProviderEnvironmentIFCRef& env,
	  OW_CIMObjectPath cop,
	  OW_Bool deep, 
	  OW_CIMClass cimClass, 
	  OW_Bool localOnly );

   virtual OW_CIMInstance getInstance(
	  const OW_ProviderEnvironmentIFCRef& env,
	  OW_CIMObjectPath cop,
	  OW_CIMClass cimClass, 
	  OW_Bool localOnly );

   virtual OW_CIMObjectPath createInstance(
	  const OW_ProviderEnvironmentIFCRef& env,
	  OW_CIMObjectPath cop,
	  OW_CIMInstance cimInstance );

   virtual void setInstance(
	  const OW_ProviderEnvironmentIFCRef& env,
	  OW_CIMObjectPath cop,
	  OW_CIMInstance cimInstance);

   virtual void deleteInstance(
	  const OW_ProviderEnvironmentIFCRef& env,
	  OW_CIMObjectPath cop);


   virtual OW_CIMValue invokeMethod(
	  const OW_ProviderEnvironmentIFCRef& env,
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
RPMIP::initialize(const OW_ProviderEnvironmentIFCRef&)
{
   struct stat fs;
   if(stat("/usr/bin/dpkg", &fs) == 0)
   {
	  _pkgHandler = "/usr/bin/isinstalled ";
   } else if(stat("/usr/bin/rpm", &fs) == 0)
   {
	  _pkgHandler = "/usr/bin/rpm -q ";
   } else if(stat("/bin/rpm", &fs) == 0)
   {
	  _pkgHandler = "/bin/rpm -q ";
   } else
   {
	  _pkgHandler = "";
   }
}


//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration 
   RPMIP::enumInstanceNames(
   const OW_ProviderEnvironmentIFCRef& env,
   OW_CIMObjectPath cop,
   OW_Bool deep,
   OW_CIMClass cimClass )
{
   (void)deep;
   (void)cimClass;
   (void)env;

   OW_CIMObjectPathEnumeration rval;
   OW_String cmd = "/usr/bin/apt-cache search .*";
   OW_PopenStreams pos = OW_Exec::safePopen(cmd.tokenize());

   OW_StringArray lines = pos.out()->readAll().tokenize("\n");

   sort(lines.begin(), lines.end());

   if (pos.getExitStatus() != 0)
   {
	  OW_THROWCIMMSG(OW_CIMException::NOT_SUPPORTED, "Bad exit status from popen");
   }


   for(OW_StringArray::const_iterator iter = lines.begin(); 
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
   const OW_ProviderEnvironmentIFCRef& env,
   OW_CIMObjectPath cop,
   OW_Bool deep, 
   OW_CIMClass cimClass, 
   OW_Bool localOnly )
{
   (void)env;
   (void)cop;
   (void)deep;
   (void)localOnly;
   OW_CIMInstanceEnumeration rval;

   OW_String cmd = "/bin/ps ax --no-heading -eo pid,comm";
   OW_PopenStreams pos = OW_Exec::safePopen(cmd.tokenize());
   if(pos.getExitStatus() != 0)
   {
	  OW_THROWCIMMSG(OW_CIMException::NOT_SUPPORTED, "Bad exit status from popen");
   }

   OW_String output = pos.out()->readAll();
   OW_StringArray lines = output.tokenize("\n");
   for(OW_StringArray::const_iterator iter = lines.begin(); 
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
   const OW_ProviderEnvironmentIFCRef& env,
   OW_CIMObjectPath cop,
   OW_CIMClass cimClass, 
   OW_Bool localOnly )
{
   (void)env;
   (void)cop;
   (void)cimClass;
   (void)localOnly;

   OW_CIMInstance rval = cimClass.newInstance();
   rval.setProperties(cop.getKeys());


   if(!processPkg(rval))
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

   OW_StringArray lines = pos.out()->readAll().tokenize("\n");
   if(pos.getExitStatus() != 0)
   {
	  return OW_Bool(false);
   }
   // TODO parse output and add properties

   OW_String curName, value;
   for(OW_StringArray::const_iterator iter = lines.begin(); 
	  iter != lines.end(); ++iter)
   {
	  if((*iter)[0] == ' ')
	  {
		 if(curName.length())
		 {
			value += (*iter);
		 } else
		 {
			continue;
		 }
	  } else
	  {
		 unsigned int idx = iter->indexOf(':');
		 curName = iter->substring(0, idx);
		 value = iter->substring(idx + 1);
		 value.trim();
	  }


	  if(curName.equals("Depends")
		 || curName.equals("Depends")
		 || curName.equals("Suggests")
		 || curName.equals("Provides")
		 || curName.equals("Conflicts")
		 || curName.equals("Recommends"))
	  {
		 inst.setProperty(curName, OW_CIMValue(value.tokenize(",")));
	  }

	  else if(curName.equals("Size")
		 || curName.equals("Installed-Size"))
	  {
		 if(curName.equals("Installed-Size"))
		 {
			curName = "Installed_Size";
		 }
		 inst.setProperty(curName, OW_CIMValue(value.toUInt32()));
	  }

	  else if(curName.equals("Name")
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
	  } else
	  {
		 curName = "";
		 continue;
	  }

   }

   // determine if package is installed.
   cmd = _pkgHandler + name;
   if(OW_Exec::safeSystem(cmd.tokenize()) == 0)
   {
	  inst.setProperty("State", OW_CIMValue(OW_String("Installed")));
   } else
   {
	  inst.setProperty("State", OW_CIMValue(OW_String("Available")));
   }

   return OW_Bool(true);
}





//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath 
   RPMIP::createInstance(
   const OW_ProviderEnvironmentIFCRef& env,
   OW_CIMObjectPath cop,
   OW_CIMInstance cimInstance )
{
   (void)env;
   (void)cop;
   (void)cimInstance;
   OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
}

//////////////////////////////////////////////////////////////////////////////
void 
   RPMIP::setInstance(
   const OW_ProviderEnvironmentIFCRef& env,
   OW_CIMObjectPath cop,
   OW_CIMInstance cimInstance)
{
   (void)env;
   (void)cop;
   (void)cimInstance;
   OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
}

//////////////////////////////////////////////////////////////////////////////
void 
   RPMIP::deleteInstance(
   const OW_ProviderEnvironmentIFCRef& env,
   OW_CIMObjectPath cop)
{
   (void)env;
   (void)cop;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMValue 
   RPMIP::invokeMethod(
   const OW_ProviderEnvironmentIFCRef& env,
   const OW_CIMObjectPath& cop,
   const OW_String& methodName, 
   const OW_CIMValueArray& in,
   OW_CIMValueArray& out )
{
   (void)env;
   (void)out;
   (void)in;

   OW_String pkgName;
   OW_CIMPropertyArray props = cop.getKeys();
   for(OW_CIMPropertyArray::const_iterator iter = props.begin(); 
	  iter != props.end(); ++iter)
   {
	  if(iter->getName().equalsIgnoreCase("name"))
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

   
