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

#include "OW_config.h"
#include "OW_CppAssociatorProviderIFC.hpp"
#include "OW_CppReadOnlyInstanceProviderIFC.hpp"
#include "OW_CppSimpleInstanceProviderIFC.hpp"
#include "OW_Format.hpp"
#include "OW_String.hpp"
#include "OW_Bool.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMException.hpp"

#include <fstream>

namespace
{
	// anonymous namespace is to prevent possible linkage problems or identifier
	// conflict whens the library is dynamically loaded

	using std::ifstream;
	using namespace OW_WBEMFlags;

	static OW_String getDestClass(const OW_CIMInstance& ci)
	{

		OW_String classOfSource = ci.getClassName();
		if (classOfSource.equalsIgnoreCase("EXP_BionicComputerSystem"))
		{
			return "EXP_BionicComputerSystem2";
		}
		else if (classOfSource.equalsIgnoreCase("EXP_BionicComputerSystem2"))
		{
			return "EXP_BionicComputerSystem";
		}
		else
		{
			return "";
		}
	}

	class OW_AssociatorTest : public OW_CppAssociatorProviderIFC, public OW_CppSimpleInstanceProviderIFC
	{
	public:
		~OW_AssociatorTest();

		virtual void associators(
			const OW_ProviderEnvironmentIFCRef& env,
			OW_CIMInstanceResultHandlerIFC& result,
			const OW_String& ns,
			const OW_CIMObjectPath& objectName,
			const OW_String& assocClass,
			const OW_String& resultClass,
			const OW_String& role,
			const OW_String& resultRole,
			OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
			OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
			const OW_StringArray* propertyList)
		{
			env->getLogger()->logDebug(format("OW_AssociatorTest associators called "
				"ns = %1, objectName = %2, assocClass = %3, resultClass = %4, "
				"role = %5, resultRole = %6, includeQualifiers = %7, "
				"includeClassOrigin = %8, propertyList = %9",
				ns, objectName.toString(), assocClass,
				resultClass, role, resultRole, includeQualifiers,
				includeClassOrigin, propertyList));

			OW_CIMOMHandleIFCRef hdl = env->getCIMOMHandle();

			OW_CIMInstance ci = hdl->getInstance(ns,
				objectName);

			OW_String destClass = getDestClass(ci);

			if (destClass.length() == 0)
				return;

			// All other instances of the other class are associated.
			OW_CIMInstanceEnumeration instances = hdl->enumInstancesE(ns,destClass, E_DEEP);
			while (instances.hasMoreElements())
			{
				OW_CIMInstance ci = instances.nextElement();
				ci.setProperty("producedByAssocTest", OW_CIMValue(true));
				env->getLogger()->logDebug(format("OW_AssociatorTest producing: %1", ci));
				result.handle(ci.clone(E_NOT_LOCAL_ONLY,includeQualifiers,includeClassOrigin,propertyList));
			}

		}

		virtual void associatorNames(
			const OW_ProviderEnvironmentIFCRef& env,
			OW_CIMObjectPathResultHandlerIFC& result,
			const OW_String& ns,
			const OW_CIMObjectPath& objectName,
			const OW_String& assocClass,
			const OW_String& resultClass,
			const OW_String& role,
			const OW_String& resultRole )
		{
			env->getLogger()->logDebug(format("OW_AssociatorTest associatorNames called "
				"ns = %1, objectName = %2, assocClass = %3, resultClass = %4, "
				"role = %4, resultRole = %6",
				ns, objectName.toString(), assocClass, resultClass,
				role, resultRole));

			OW_CIMOMHandleIFCRef hdl = env->getCIMOMHandle();
			OW_CIMInstance ci = hdl->getInstance(ns,
				objectName);

			OW_String destClass = getDestClass(ci);

			if (destClass.length() == 0)
			{
				return;
			}

			OW_CIMInstanceEnumeration instances = hdl->enumInstancesE(
				ns, destClass);
			while (instances.hasMoreElements())
			{
				OW_CIMInstance ci = instances.nextElement();
				OW_CIMObjectPath cop(ns, ci);
				env->getLogger()->logDebug(format("OW_AssociatorTest producing: %1", cop));
				result.handle(cop);
			}
		}

		virtual void references(
			const OW_ProviderEnvironmentIFCRef& env,
			OW_CIMInstanceResultHandlerIFC& result,
			const OW_String& ns,
			const OW_CIMObjectPath& objectName,
			const OW_String& resultClass,
			const OW_String& role,
			OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
			OW_WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
			const OW_StringArray* propertyList )
		{
			env->getLogger()->logDebug(format("OW_AssociatorTest references called "
				"ns = %1, objectName = %2, resultClass = %3, role = %4, "
				"includeQualifiers = %5, includeClassOrigin = %6, "
				"propertyList = %7",
				ns, objectName.toString(), resultClass, role,
				includeQualifiers, includeClassOrigin, propertyList));

			OW_CIMOMHandleIFCRef hdl = env->getCIMOMHandle();
			OW_CIMInstance ci = hdl->getInstance(ns,
				objectName);

			OW_String destClass = getDestClass(ci);

			if (destClass.length() == 0)
				return;

			OW_CIMInstanceEnumeration e1 = hdl->enumInstancesE(
				ns, destClass);

			// Just assume that all other instances of the other class are associated!
			while (e1.hasMoreElements())
			{
				OW_CIMClass cc = hdl->getClass(ns,
					resultClass);
				OW_CIMInstance newInstance = cc.newInstance();
				OW_CIMInstance ci = e1.nextElement();
				OW_CIMObjectPath path(ns, ci);

				newInstance.setProperty("GroupComponent", OW_CIMValue(objectName));

				newInstance.setProperty("PartComponent", OW_CIMValue(path));
				newInstance.setProperty("producedByAssocTest", OW_CIMValue(true));

				env->getLogger()->logDebug(format("OW_AssociatorTest producing: %1", newInstance));
				result.handle(newInstance.clone(E_NOT_LOCAL_ONLY,includeQualifiers,includeClassOrigin,propertyList));
			}
		}

		virtual void referenceNames(
			const OW_ProviderEnvironmentIFCRef& env,
			OW_CIMObjectPathResultHandlerIFC& result,
			const OW_String& ns,
			const OW_CIMObjectPath& objectName,
			const OW_String& resultClass,
			const OW_String& role )
		{
			env->getLogger()->logDebug(format("OW_AssociatorTest referenceNames called "
				"ns = %1, objectName = %2, resultClass = %3, role = %4",
				ns, objectName.toString(), resultClass, role));

			OW_CIMOMHandleIFCRef hdl = env->getCIMOMHandle();
			OW_CIMInstance ci = hdl->getInstance(ns,
				objectName);

			OW_String destClass = getDestClass(ci);

			if (destClass.length() == 0)
				return;

			OW_CIMInstanceEnumeration e1 = hdl->enumInstancesE(ns, destClass);

			// Just assume that all other instances of the other class are associated!
			while (e1.hasMoreElements())
			{
				OW_CIMClass cc = hdl->getClass(ns, resultClass);
				OW_CIMInstance newInstance = cc.newInstance();

				OW_CIMInstance ci = e1.nextElement();
				OW_CIMObjectPath path(ns, ci);

				newInstance.setProperty("GroupComponent", OW_CIMValue(objectName));

				newInstance.setProperty("PartComponent", OW_CIMValue(path));

				OW_CIMObjectPath newPath(ns, newInstance);

				env->getLogger()->logDebug(format("OW_AssociatorTest producing: %1", newPath));
				result.handle(newPath);
			}
		}

		virtual void initialize(const OW_ProviderEnvironmentIFCRef& env);

		virtual void doSimpleEnumInstances(
			const OW_ProviderEnvironmentIFCRef &env, 
			const OW_String &ns, 
			const OW_CIMClass &cimClass, 
			OW_CIMInstanceResultHandlerIFC &result,
			EPropertiesFlag propertiesFlag)
		{
			(void)propertiesFlag;
			env->getLogger()->logDebug("OW_AssociatorTest doSimpleEnumInstances called ");

			OW_CIMOMHandleIFCRef hdl = env->getCIMOMHandle();

			try
			{
				OW_CIMInstanceEnumeration insts1e = hdl->enumInstancesE(ns, "EXP_BionicComputerSystem");
				OW_CIMInstanceArray insts1;
				while (insts1e.hasMoreElements())
				{
					insts1.push_back(insts1e.nextElement());
				}
				OW_CIMInstanceEnumeration insts2e = hdl->enumInstancesE(ns, "EXP_BionicComputerSystem2");
				OW_CIMInstanceArray insts2;
				while (insts2e.hasMoreElements())
				{
					insts2.push_back(insts2e.nextElement());
				}

				// Just assume that all instances are associated!
				for (size_t i = 0; i < insts1.size(); ++i)
				{
					for (size_t j = 0; j < insts2.size(); ++j)
					{
						OW_CIMInstance newInstance(cimClass.newInstance());
						newInstance.setProperty("GroupComponent", OW_CIMValue(OW_CIMObjectPath(ns, insts1[i])));

						newInstance.setProperty("PartComponent", OW_CIMValue(OW_CIMObjectPath(ns, insts2[j])));
						env->getLogger()->logDebug(format("OW_AssociatorTest producing: %1", newInstance.toMOF()));
						result.handle(newInstance);
					}
				}
			}
			catch (OW_CIMException&)
			{
				// just ignore errors for the test.
			}
		}

		void deleteInstance(const OW_ProviderEnvironmentIFCRef &, const OW_String &, const OW_CIMObjectPath &)
		{
			// do nothing.
		}

		OW_CIMObjectPath createInstance(const OW_ProviderEnvironmentIFCRef &, const OW_String &ns, const OW_CIMInstance &inst)
		{
			// do nothing.
			return OW_CIMObjectPath(ns, inst);
		}

		void modifyInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMInstance& modifiedInstance,
			const OW_CIMInstance& previousInstance,
			OW_WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
			const OW_StringArray* propertyList,
			const OW_CIMClass& theClass)
		{
			(void)env; (void)ns; (void)modifiedInstance; (void)previousInstance; (void)includeQualifiers; (void)propertyList; (void)theClass;
			// do nothing.
		}
	};

//////////////////////////////////////////////////////////////////////////////
	OW_AssociatorTest::~OW_AssociatorTest()
	{
	}



//////////////////////////////////////////////////////////////////////////////
	void
		OW_AssociatorTest::initialize(const OW_ProviderEnvironmentIFCRef& env)
	{
		env->getLogger()->logDebug("OW_AssociatorTest initialize called");
	}

} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
OW_PROVIDERFACTORY(OW_AssociatorTest, associatortest);

