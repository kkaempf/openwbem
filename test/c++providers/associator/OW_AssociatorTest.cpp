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
#include "OW_Exception.hpp"
#include "OW_Format.hpp"
#include "OW_String.hpp"
#include "OW_Bool.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMInstanceEnumeration.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMObjectPath.hpp"

#include <fstream>

namespace
{
	// anonymous namespace is to prevent possible linkage problems or identifier
	// conflict whens the library is dynamically loaded

	using std::ifstream;

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

	class OW_AssociatorTest : public OW_CppAssociatorProviderIFC
	{
	public:
		~OW_AssociatorTest();

		virtual void associators(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMObjectPath& assocName,
			const OW_CIMObjectPath& objectName,
			OW_CIMInstanceResultHandlerIFC& result,
			const OW_String& resultClass,
			const OW_String& role,
			const OW_String& resultRole,
			const OW_Bool& includeQualifiers,
			const OW_Bool& includeClassOrigin,
			const OW_StringArray* propertyList)
		{
			env->getLogger()->logDebug(format("OW_AssociatorTest associators called "
				"ns = %1, assocName = %2, objectName = %3, resultClass = %4, "
				"role = %5, resultRole = %6, includeQualifiers = %7, "
				"includeClassOrigin = %8, propertyList = %9",
				ns, assocName.toString(), objectName.toString(),
				resultClass, role, resultRole, includeQualifiers,
				includeClassOrigin, propertyList));

			OW_CIMOMHandleIFCRef hdl = env->getCIMOMHandle();

			OW_CIMInstance ci = hdl->getInstance(ns,
				objectName, false);

			OW_String destClass = getDestClass(ci);

			if (destClass.length() == 0)
				return;

			// All other instances of the other class are associated.
			OW_CIMInstanceEnumeration instances = hdl->enumInstancesE(ns,destClass, true);
			while (instances.hasMoreElements())
			{
				OW_CIMInstance ci = instances.nextElement();
				ci.setProperty("producedByAssocTest", OW_CIMValue(true));
				env->getLogger()->logDebug(format("OW_AssociatorTest producing: %1", ci));
				result.handle(ci.clone(false,includeQualifiers,includeClassOrigin,propertyList));
			}

		}

		virtual void associatorNames(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMObjectPath& assocName,
			const OW_CIMObjectPath& objectName,
			OW_CIMObjectPathResultHandlerIFC& result,
			const OW_String& resultClass,
			const OW_String& role,
			const OW_String& resultRole )
		{
			env->getLogger()->logDebug(format("OW_AssociatorTest associatorNames called "
				"ns = %1, assocName = %2, objectName = %3, resultClass = %4, "
				"role = %4, resultRole = %6",
				ns, assocName.toString(), objectName.toString(), resultClass,
				role, resultRole));

			OW_CIMOMHandleIFCRef hdl = env->getCIMOMHandle();
			OW_CIMInstance ci = hdl->getInstance(ns,
				objectName, false);

			OW_String destClass = getDestClass(ci);

			if (destClass.length() == 0)
			{
				return;
			}

			OW_CIMInstanceEnumeration instances = hdl->enumInstancesE(
				ns, destClass, true, false);
			while (instances.hasMoreElements())
			{
				OW_CIMInstance ci = instances.nextElement();
				OW_CIMObjectPath cop(ci);
				cop.setNameSpace(ns);
				env->getLogger()->logDebug(format("OW_AssociatorTest producing: %1", cop));
				result.handle(cop);
			}
		}

		virtual void references(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMObjectPath& assocName,
			const OW_CIMObjectPath& objectName,
			OW_CIMInstanceResultHandlerIFC& result,
			const OW_String& role,
			const OW_Bool& includeQualifiers,
			const OW_Bool& includeClassOrigin,
			const OW_StringArray* propertyList )
		{
			env->getLogger()->logDebug(format("OW_AssociatorTest references called "
				"ns = %1, assocName = %2, objectName = %3, role = %4, "
				"includeQualifiers = %5, includeClassOrigin = %6, "
				"propertyList = %7",
				ns, assocName.toString(), objectName.toString(), role,
				includeQualifiers, includeClassOrigin, propertyList));

			OW_CIMOMHandleIFCRef hdl = env->getCIMOMHandle();
			OW_CIMInstance ci = hdl->getInstance(ns,
				objectName, false);

			OW_String destClass = getDestClass(ci);

			if (destClass.length() == 0)
				return;

			OW_CIMInstanceEnumeration e1 = hdl->enumInstancesE(
				ns, destClass, true);

			// Just assume that all other instances of the other class are associated!
			while (e1.hasMoreElements())
			{
				OW_CIMClass cc = hdl->getClass(ns,
					assocName.getObjectName(), false);
				OW_CIMInstance newInstance = cc.newInstance();
				OW_CIMInstance ci = e1.nextElement();
				OW_CIMObjectPath path(ci);
				path.setNameSpace(ns);

				newInstance.setProperty("GroupComponent", OW_CIMValue(objectName));

				newInstance.setProperty("PartComponent", OW_CIMValue(path));
				newInstance.setProperty("producedByAssocTest", OW_CIMValue(true));

				env->getLogger()->logDebug(format("OW_AssociatorTest producing: %1", newInstance));
				result.handle(newInstance.clone(false,includeQualifiers,includeClassOrigin,propertyList));
			}
		}

		virtual void referenceNames(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMObjectPath& assocName,
			const OW_CIMObjectPath& objectName,
			OW_CIMObjectPathResultHandlerIFC& result,
			const OW_String& role )
		{
			env->getLogger()->logDebug(format("OW_AssociatorTest referenceNames called "
				"ns = %1, assocName = %2, objectName = %3, role = %4",
				ns, assocName.toString(), objectName.toString(), role));

			OW_CIMOMHandleIFCRef hdl = env->getCIMOMHandle();
			OW_CIMInstance ci = hdl->getInstance(ns,
				objectName, false);

			OW_String destClass = getDestClass(ci);

			if (destClass.length() == 0)
				return;

			OW_CIMInstanceEnumeration e1 = hdl->enumInstancesE(ns, destClass, true, false);

			// Just assume that all other instances of the other class are associated!
			while (e1.hasMoreElements())
			{
				OW_CIMClass cc = hdl->getClass(ns,
					assocName.getObjectName(), false);
				OW_CIMInstance newInstance = cc.newInstance();

				OW_CIMInstance ci = e1.nextElement();
				OW_CIMObjectPath path(ci);
				path.setNameSpace(ns);

				newInstance.setProperty("GroupComponent", OW_CIMValue(objectName));

				newInstance.setProperty("PartComponent", OW_CIMValue(path));

				OW_CIMObjectPath newPath(newInstance);

				newPath.setNameSpace(ns);
				env->getLogger()->logDebug(format("OW_AssociatorTest producing: %1", newPath));
				result.handle(newPath);
			}
		}

		virtual void initialize(const OW_ProviderEnvironmentIFCRef& env);
		virtual void cleanup();

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

//////////////////////////////////////////////////////////////////////////////
	void
		OW_AssociatorTest::cleanup()
	{
	}



} // end anonymous namespace
//////////////////////////////////////////////////////////////////////////////
OW_PROVIDERFACTORY(OW_AssociatorTest, associatortest);

