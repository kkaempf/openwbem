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


#include "OW_config.h"
#include "OW_CppAssociatorProviderIFC.hpp"
#include "OW_CppReadOnlyInstanceProviderIFC.hpp"
#include "OW_CppSimpleInstanceProviderIFC.hpp"
#include "OW_Format.hpp"
#include "OW_String.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMException.hpp"

#include <fstream>

using namespace OpenWBEM;
using namespace WBEMFlags;

namespace
{
	// anonymous namespace is to prevent possible linkage problems or identifier
	// conflict whens the library is dynamically loaded

	using std::ifstream;

	const String COMPONENT_NAME("ow.test.AssociatorTest");
	

	static String getDestClass(const CIMInstance& ci)
	{

		String classOfSource = ci.getClassName();
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

	class AssociatorTest : public CppAssociatorProviderIFC, public CppSimpleInstanceProviderIFC
	{
	public:
		~AssociatorTest();

		virtual void associators(
			const ProviderEnvironmentIFCRef& env,
			CIMInstanceResultHandlerIFC& result,
			const String& ns,
			const CIMObjectPath& objectName,
			const String& assocClass,
			const String& resultClass,
			const String& role,
			const String& resultRole,
			EIncludeQualifiersFlag includeQualifiers,
			EIncludeClassOriginFlag includeClassOrigin,
			const StringArray* propertyList)
		{
			OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("AssociatorTest associators called "
				"ns = %1, objectName = %2, assocClass = %3, resultClass = %4, "
				"role = %5, resultRole = %6, includeQualifiers = %7, "
				"includeClassOrigin = %8, propertyList = %9",
				ns, objectName.toString(), assocClass,
				resultClass, role, resultRole, includeQualifiers,
				includeClassOrigin, propertyList));

			CIMOMHandleIFCRef hdl = env->getCIMOMHandle();

			CIMInstance ci = hdl->getInstance(ns,
				objectName);

			String destClass = getDestClass(ci);

			if (destClass.length() == 0)
				return;

			// All other instances of the other class are associated.
			CIMInstanceEnumeration instances = hdl->enumInstancesE(ns,destClass, E_DEEP);
			while (instances.hasMoreElements())
			{
				CIMInstance ci = instances.nextElement();
				ci.setProperty("producedByAssocTest", CIMValue(true));
				OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("AssociatorTest producing: %1", ci));
				result.handle(ci.clone(E_NOT_LOCAL_ONLY,includeQualifiers,includeClassOrigin,propertyList));
			}

		}

		virtual void associatorNames(
			const ProviderEnvironmentIFCRef& env,
			CIMObjectPathResultHandlerIFC& result,
			const String& ns,
			const CIMObjectPath& objectName,
			const String& assocClass,
			const String& resultClass,
			const String& role,
			const String& resultRole )
		{
			OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("AssociatorTest associatorNames called "
				"ns = %1, objectName = %2, assocClass = %3, resultClass = %4, "
				"role = %4, resultRole = %6",
				ns, objectName.toString(), assocClass, resultClass,
				role, resultRole));

			CIMOMHandleIFCRef hdl = env->getCIMOMHandle();
			CIMInstance ci = hdl->getInstance(ns,
				objectName);

			String destClass = getDestClass(ci);

			if (destClass.length() == 0)
			{
				return;
			}

			CIMInstanceEnumeration instances = hdl->enumInstancesE(
				ns, destClass);
			while (instances.hasMoreElements())
			{
				CIMInstance ci = instances.nextElement();
				CIMObjectPath cop(ns, ci);
				OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("AssociatorTest producing: %1", cop));
				result.handle(cop);
			}
		}

		virtual void references(
			const ProviderEnvironmentIFCRef& env,
			CIMInstanceResultHandlerIFC& result,
			const String& ns,
			const CIMObjectPath& objectName,
			const String& resultClass,
			const String& role,
			EIncludeQualifiersFlag includeQualifiers,
			EIncludeClassOriginFlag includeClassOrigin,
			const StringArray* propertyList )
		{
			OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("AssociatorTest references called "
				"ns = %1, objectName = %2, resultClass = %3, role = %4, "
				"includeQualifiers = %5, includeClassOrigin = %6, "
				"propertyList = %7",
				ns, objectName.toString(), resultClass, role,
				includeQualifiers, includeClassOrigin, propertyList));

			CIMOMHandleIFCRef hdl = env->getCIMOMHandle();
			CIMInstance ci = hdl->getInstance(ns,
				objectName);

			String destClass = getDestClass(ci);

			if (destClass.length() == 0)
				return;

			CIMInstanceEnumeration e1 = hdl->enumInstancesE(
				ns, destClass);

			// Just assume that all other instances of the other class are associated!
			while (e1.hasMoreElements())
			{
				CIMClass cc = hdl->getClass(ns,
					resultClass);
				CIMInstance newInstance = cc.newInstance();
				CIMInstance ci = e1.nextElement();
				CIMObjectPath path(ns, ci);

				newInstance.setProperty("GroupComponent", CIMValue(objectName));

				newInstance.setProperty("PartComponent", CIMValue(path));
				newInstance.setProperty("producedByAssocTest", CIMValue(true));

				OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("AssociatorTest producing: %1", newInstance));
				result.handle(newInstance.clone(E_NOT_LOCAL_ONLY,includeQualifiers,includeClassOrigin,propertyList));
			}
		}

		virtual void referenceNames(
			const ProviderEnvironmentIFCRef& env,
			CIMObjectPathResultHandlerIFC& result,
			const String& ns,
			const CIMObjectPath& objectName,
			const String& resultClass,
			const String& role )
		{
			OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("AssociatorTest referenceNames called "
				"ns = %1, objectName = %2, resultClass = %3, role = %4",
				ns, objectName.toString(), resultClass, role));

			CIMOMHandleIFCRef hdl = env->getCIMOMHandle();
			CIMInstance ci = hdl->getInstance(ns,
				objectName);

			String destClass = getDestClass(ci);

			if (destClass.length() == 0)
				return;

			CIMInstanceEnumeration e1 = hdl->enumInstancesE(ns, destClass);

			// Just assume that all other instances of the other class are associated!
			while (e1.hasMoreElements())
			{
				CIMClass cc = hdl->getClass(ns, resultClass);
				CIMInstance newInstance = cc.newInstance();

				CIMInstance ci = e1.nextElement();
				CIMObjectPath path(ns, ci);

				newInstance.setProperty("GroupComponent", CIMValue(objectName));

				newInstance.setProperty("PartComponent", CIMValue(path));

				CIMObjectPath newPath(ns, newInstance);

				OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("AssociatorTest producing: %1", newPath));
				result.handle(newPath);
			}
		}

		virtual void initialize(const ProviderEnvironmentIFCRef& env);

		virtual void doSimpleEnumInstances(
			const ProviderEnvironmentIFCRef &env,
			const String &ns,
			const CIMClass &cimClass,
			CIMInstanceResultHandlerIFC &result,
			EPropertiesFlag propertiesFlag)
		{
			OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), "AssociatorTest doSimpleEnumInstances called ");

			CIMOMHandleIFCRef hdl = env->getCIMOMHandle();

			try
			{
				CIMInstanceEnumeration insts1e = hdl->enumInstancesE(ns, "EXP_BionicComputerSystem");
				CIMInstanceArray insts1;
				while (insts1e.hasMoreElements())
				{
					insts1.push_back(insts1e.nextElement());
				}
				CIMInstanceEnumeration insts2e = hdl->enumInstancesE(ns, "EXP_BionicComputerSystem2");
				CIMInstanceArray insts2;
				while (insts2e.hasMoreElements())
				{
					insts2.push_back(insts2e.nextElement());
				}

				// Just assume that all instances are associated!
				for (size_t i = 0; i < insts1.size(); ++i)
				{
					for (size_t j = 0; j < insts2.size(); ++j)
					{
						CIMInstance newInstance(cimClass.newInstance());
						newInstance.setProperty("GroupComponent", CIMValue(CIMObjectPath(ns, insts1[i])));

						newInstance.setProperty("PartComponent", CIMValue(CIMObjectPath(ns, insts2[j])));
						OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("AssociatorTest producing: %1", newInstance.toMOF()));
						result.handle(newInstance);
					}
				}
			}
			catch (CIMException&)
			{
				// just ignore errors for the test.
			}
		}

		void deleteInstance(const ProviderEnvironmentIFCRef &, const String &, const CIMObjectPath &)
		{
			// do nothing.
		}

		CIMObjectPath createInstance(const ProviderEnvironmentIFCRef &, const String &ns, const CIMInstance &inst)
		{
			// do nothing.
			return CIMObjectPath(ns, inst);
		}

		void modifyInstance(
			const ProviderEnvironmentIFCRef& env,
			const String& ns,
			const CIMInstance& modifiedInstance,
			const CIMInstance& previousInstance,
			EIncludeQualifiersFlag includeQualifiers,
			const StringArray* propertyList,
			const CIMClass& theClass)
		{
			// do nothing.
		}
	};

//////////////////////////////////////////////////////////////////////////////
	AssociatorTest::~AssociatorTest()
	{
	}



//////////////////////////////////////////////////////////////////////////////
	void
		AssociatorTest::initialize(const ProviderEnvironmentIFCRef& env)
	{
		OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), "AssociatorTest initialize called");
	}

} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
OW_PROVIDERFACTORY(AssociatorTest, associatortest);

