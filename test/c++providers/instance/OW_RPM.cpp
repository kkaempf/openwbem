/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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

using namespace OpenWBEM;
using namespace WBEMFlags;

namespace{

// We use multiple inheritance here because our provider is to be both an
// Instance and a method provider
class RPMIP : public CppInstanceProviderIFC, public CppMethodProviderIFC
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
		initialize(const ProviderEnvironmentIFCRef&)
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
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		const CIMClass& cimClass )
	{
		(void)cimClass;
		(void)env;

		String cmd = "/usr/bin/apt-cache search .*";
		PopenStreams pos = Exec::safePopen(cmd.tokenize());

		StringArray lines = pos.out()->readAll().tokenize("\n");

		std::sort(lines.begin(), lines.end());

		if (pos.getExitStatus() != 0)
		{
			OW_THROWCIMMSG(CIMException::FAILED, "Bad exit status from popen");
		}


		for (StringArray::const_iterator iter = lines.begin();
			iter != lines.end(); ++iter)
		{
			CIMObjectPath newCop(className, ns);
			newCop.setKeyValue("Name", CIMValue(iter->tokenize()[0]));
			result.handle(newCop);
		}
	}


//////////////////////////////////////////////////////////////////////////////
	struct InstanceCreator : public CIMObjectPathResultHandlerIFC
	{
		InstanceCreator(CIMInstanceResultHandlerIFC& result, const CIMClass& cimClass_) : m_result(result), cimClass(cimClass_) {}
		void doHandle(const CIMObjectPath& path)
		{
			CIMInstance rval = cimClass.newInstance();
			rval.setProperties(path.getKeys());

			if (!RPMIP::processPkg(rval))
			{
				OW_THROWCIMMSG(CIMException::NOT_FOUND,
					"The Instance does not (any longer) exist");
			}
			m_result.handle(rval);
		}
		CIMInstanceResultHandlerIFC& m_result;
		CIMClass cimClass;
	};

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
		env->getLogger()->logDebug("in RPM::enumInstances");
		String cmd = "/usr/bin/apt-cache search .*";
		PopenStreams pos = Exec::safePopen(cmd.tokenize());

		StringArray lines = pos.out()->readAll().tokenize("\n");

		std::sort(lines.begin(), lines.end());

		if (pos.getExitStatus() != 0)
		{
			OW_THROWCIMMSG(CIMException::FAILED, "Bad exit status from popen");
		}


		CIMInstanceArray insts;
		for (StringArray::const_iterator iter = lines.begin();
			iter != lines.end(); ++iter)
		{
			CIMObjectPath newCop(className, ns);
			newCop.setKeyValue("Name", CIMValue(iter->tokenize()[0]));
			CIMInstance rval = cimClass.newInstance();
			rval.setProperties(newCop.getKeys());
			insts.push_back(rval);
		}
		processPkgs(insts);
		for (size_t i = 0; i < insts.size(); ++i)
		{
			result.handle(insts[i].clone(localOnly, deep, includeQualifiers, includeClassOrigin, propertyList, requestedClass, cimClass));
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
		(void)env;
		(void)ns;
		(void)cimClass;

		CIMInstance rval = cimClass.newInstance();
		rval.setProperties(instanceName.getKeys());


		if (!RPMIP::processPkg(rval))
		{
			OW_THROWCIMMSG(CIMException::NOT_FOUND,
				"The Instance does not (any longer) exist");
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
		// not applicable with our apt implementation.
		(void)env;
		(void)ns;
		(void)cimInstance;
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support createInstance");
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
		(void)env;
		(void)ns;
		(void)modifiedInstance;
		(void)previousInstance;
		(void)includeQualifiers;
		(void)propertyList;
		(void)theClass;
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support modifyInstance");
	}

//////////////////////////////////////////////////////////////////////////////
	void
		deleteInstance(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMObjectPath& cop)
	{
		// not applicable with our apt implementation.
		(void)env;
		(void)ns;
		(void)cop;
	}

//////////////////////////////////////////////////////////////////////////////
	CIMValue
		invokeMethod(
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
		(void)in;

		String pkgName;
		CIMPropertyArray props = path.getKeys();
		for (CIMPropertyArray::const_iterator iter = props.begin();
			iter != props.end(); ++iter)
		{
			if (iter->getName().equalsIgnoreCase("name"))
			{
				iter->getValue().get(pkgName);
				break;
			}
		}

		StringArray cmd;
		cmd.push_back(String("/usr/bin/apt-get"));
		cmd.push_back(String("-y"));
		cmd.push_back(methodName);
		cmd.push_back(pkgName);

		int rc = Exec::safeSystem(cmd);

		return CIMValue(Int32(rc));
	}


private:
	static String _pkgHandler;

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
	static bool
		processPkg(CIMInstance& inst)
	{
		String name;
		inst.getProperty("Name").getValue().get(name);

		// get package details
		String cmd = "/usr/bin/apt-cache --no-a show ";
		cmd += name;
		PopenStreams pos = Exec::safePopen(cmd.tokenize());

		StringArray lines = pos.out()->readAll().tokenize("\n");
		if (pos.getExitStatus() != 0)
		{
			return false;
		}
		// TODO parse output and add properties

		bool hitPackage = false;
		String curName, value;
		for (StringArray::const_iterator iter = lines.begin();
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
				size_t idx = iter->indexOf(':');
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
				inst.setProperty(curName, CIMValue(value.tokenize(",")));
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
					inst.setProperty(curName, CIMValue(value.toUInt32()));
				}
				catch (const StringConversionException& e)
				{
					OW_THROWCIMMSG(CIMException::FAILED, "Provider failed parsing output");
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
				inst.setProperty(curName, CIMValue(value));
			}
			else
			{
				curName = "";
				continue;
			}

		}

		// determine if package is installed.
		cmd = _pkgHandler + name;
		if (Exec::safeSystem(cmd.tokenize()) == 0)
		{
			inst.setProperty("Status", CIMValue(String("Installed")));
		}
		else
		{
			inst.setProperty("Status", CIMValue(String("Available")));
		}

		return true;
	}

		/**
	 * Fill in the information on a package
	 *
	 * @param inst The instance to fill out.  Note that the keys
	 *             must already be present before passing it in.
	 *
	 * @return True if successful, false if not (like the instance doesn't
	 *         exist)
	 */
	static bool
		processPkgs(CIMInstanceArray& insts)
	{
		String cmd = "/usr/bin/apt-cache --no-a show ";
		for (size_t i = 0; i < insts.size(); ++i)
		{
			String name;
			insts[i].getPropertyT("Name").getValueT().get(name);

			// get package details
			cmd += name;
			cmd += " ";
		}
		PopenStreams pos = Exec::safePopen(cmd.tokenize());

		StringArray lines = pos.out()->readAll().tokenize("\n");
		if (pos.getExitStatus() != 0)
		{
			return false;
		}
		
		size_t curInst = size_t(-1);

		String curName, value;
		for (StringArray::const_iterator iter = lines.begin();
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
				size_t idx = iter->indexOf(':');
				curName = iter->substring(0, idx);
				value = iter->substring(idx + 1);
				value.trim();
			}

			if (curName.equals("Package"))
			{
				++curInst;

				// apt-cache may skip a package or two, so just skip ahead to the current one.
				while (curInst < insts.size() && insts[curInst].getProperty("Name").getValue().toString() != value)
					curInst++;

				if (curInst >= insts.size())
					return false; // we've gone past the end.  Something is horribly wrong :-(

				// determine if package is installed.
				cmd = _pkgHandler + value;
				if (Exec::safeSystem(cmd.tokenize()) == 0)
				{
					insts[curInst].setProperty("Status", CIMValue(String("Installed")));
				}
				else
				{
					insts[curInst].setProperty("Status", CIMValue(String("Available")));
				}

			}

			CIMInstance& inst = insts[curInst];


			if (curName.equals("Depends")
				|| curName.equals("Depends")
				|| curName.equals("Suggests")
				|| curName.equals("Provides")
				|| curName.equals("Conflicts")
				|| curName.equals("Recommends"))
			{
				inst.setProperty(curName, CIMValue(value.tokenize(",")));
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
					inst.setProperty(curName, CIMValue(value.toUInt32()));
				}
				catch (const StringConversionException& e)
				{
					OW_THROWCIMMSG(CIMException::FAILED, "Provider failed parsing output");
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
				inst.setProperty(curName, CIMValue(value));
			}
			else
			{
				curName = "";
				continue;
			}

		}

		return true;
	}

};

String RPMIP::_pkgHandler;
//////////////////////////////////////////////////////////////////////////////

}


OW_PROVIDERFACTORY(RPMIP,rpmip)


	
