/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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

/*******************************************************************************
* This is an example of how to create an Instance and Method provider.
*******************************************************************************/

#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CppMethodProviderIFC.hpp"
// because we're creating a provider that is both an instance and method provider
#include "OW_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMValue.hpp"
#include "OW_SocketAddress.hpp"
#include "OW_CIMObjectPath.hpp"
// we're going to need these
#include "OW_Exec.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_Format.hpp"
// we use these internally
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>

namespace{

// We use multiple inheritance here because our provider is to be both an
// Instance and a method provider
class RPMIP : public OW_CppInstanceProviderIFC, public OW_CppMethodProviderIFC
{
public:

	virtual ~RPMIP()
	{
	}

	RPMIP()
	{
	}

	// Here are the virtual functions we'll implement.  Check the base
	// classes for the documentation.


//////////////////////////////////////////////////////////////////////////////
	void
		initialize(const OW_ProviderEnvironmentIFCRef&)
	{
		struct stat fs;
		if (stat("/usr/bin/dpkg", &fs) == 0)
		{
			// we have to provide isinstalled.
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
	void
		enumInstanceNames(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_CIMClass& cimClass )
	{
		(void)cimClass;
		(void)env;

		OW_String cmd = "/usr/bin/apt-cache search .*";
		OW_PopenStreams pos = OW_Exec::safePopen(cmd.tokenize());

		OW_StringArray lines = pos.out()->readAll().tokenize("\n");

		sort(lines.begin(), lines.end());

		if (pos.getExitStatus() != 0)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, "Bad exit status from popen");
		}


		for (OW_StringArray::const_iterator iter = lines.begin();
			iter != lines.end(); ++iter)
		{
			OW_CIMObjectPath newCop(className, ns);
			newCop.addKey("Name", OW_CIMValue(iter->tokenize()[0]));
			result.handle(newCop);
		}
	}


//////////////////////////////////////////////////////////////////////////////
	struct InstanceCreator : public OW_CIMObjectPathResultHandlerIFC
	{
		InstanceCreator(OW_CIMInstanceResultHandlerIFC& result, const OW_CIMClass& cimClass_) : m_result(result), cimClass(cimClass_) {}
		void doHandle(const OW_CIMObjectPath& path)
		{
			OW_CIMInstance rval = cimClass.newInstance();
			rval.setProperties(path.getKeys());

			if (!RPMIP::processPkg(rval))
			{
				OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
					"The Instance does not (any longer) exist");
			}
			m_result.handle(rval);
		}
		OW_CIMInstanceResultHandlerIFC& m_result;
		OW_CIMClass cimClass;
	};

	void
		enumInstances(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_String& className,
		OW_CIMInstanceResultHandlerIFC& result,
		OW_Bool /*localOnly*/, 
		OW_Bool /*deep*/, 
		OW_Bool /*includeQualifiers*/, 
		OW_Bool /*includeClassOrigin*/,
		const OW_StringArray* /*propertyList*/,
		const OW_CIMClass& /*requestedClass*/,
		const OW_CIMClass& cimClass )
	{
		InstanceCreator handler(result, cimClass);
		enumInstanceNames(env, ns, className, handler, cimClass );
	}

//////////////////////////////////////////////////////////////////////////////
	OW_CIMInstance
		getInstance(
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

		OW_CIMInstance rval = cimClass.newInstance();
		rval.setProperties(instanceName.getKeys());


		if (!RPMIP::processPkg(rval))
		{
			OW_THROWCIMMSG(OW_CIMException::NOT_FOUND,
				"The Instance does not (any longer) exist");
		}

		return rval.clone(localOnly, includeQualifiers, includeClassOrigin, 
			propertyList);
	}






//////////////////////////////////////////////////////////////////////////////
	OW_CIMObjectPath
		createInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMInstance& cimInstance )
	{
		// not applicable with our apt implementation.
		(void)env;
		(void)ns;
		(void)cimInstance;
        OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support createInstance");
	}

//////////////////////////////////////////////////////////////////////////////
	void
		modifyInstance(
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

//////////////////////////////////////////////////////////////////////////////
	void
		deleteInstance(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMObjectPath& cop)
	{
		// not applicable with our apt implementation.
		(void)env;
		(void)ns;
		(void)cop;
	}

//////////////////////////////////////////////////////////////////////////////
	OW_CIMValue
		invokeMethod(
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
		(void)in;

		OW_String pkgName;
		OW_CIMPropertyArray props = path.getKeys();
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


private:
	static OW_String _pkgHandler;

public:
	/**
	 * Fill in the information on a package
	 *
	 * @param inst The instance to fill out.  Note that the keys
	 *             must already be present before passing it in.
	 *
	 * @return True if successful, false if not (like the instance doesn't
	 *         exist)
	 */
	static OW_Bool
		processPkg(OW_CIMInstance& inst)
	{
		OW_String name;
		inst.getProperty("Name").getValue().get(name);

		// get package details
		OW_String cmd = "/usr/bin/apt-cache show ";
		cmd += name;
		OW_PopenStreams pos = OW_Exec::safePopen(cmd.tokenize());

		OW_StringArray lines = pos.out()->readAll().tokenize("\n");
		if (pos.getExitStatus() != 0)
		{
			return OW_Bool(false);
		}
		// TODO parse output and add properties

		bool hitPackage = false;
		OW_String curName, value;
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

			if (curName.equals("Package"))
			{
				if (hitPackage)
				{
					break;
				}
				else
				{
					hitPackage = true;
				}
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
				|| curName.equals("Installed-Size")
				|| curName.equals("Installed Size"))
			{
				if (!curName.equals("Size"))
				{
					curName = "Installed_Size";
				}
				try
				{
					inst.setProperty(curName, OW_CIMValue(value.toUInt32()));
				}
				catch (const OW_StringConversionException& e)
				{
					OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider failed parsing output");
				}
			}

			else if (curName.equals("Name")
				|| curName.equals("Version")
				|| curName.equals("Architecture")
				|| curName.equals("Section")
				|| curName.equals("MD5sum")
				|| curName.equals("Maintainer")
				|| curName.equals("Description")
				|| curName.equals("Priority")
				|| curName.equals("Filename")
				|| curName.equals("Status")
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
			inst.setProperty("Status", OW_CIMValue(OW_String("Installed")));
		}
		else
		{
			inst.setProperty("Status", OW_CIMValue(OW_String("Available")));
		}

		return OW_Bool(true);
	}
};

OW_String RPMIP::_pkgHandler;
//////////////////////////////////////////////////////////////////////////////

}


OW_PROVIDERFACTORY(RPMIP,rpmip)


	
