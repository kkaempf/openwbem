/*******************************************************************************
* Copyright (C) 2001 Caldera International, Inc All rights reserved.
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
*  - Neither the name of Caldera International nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL CALDERA INTERNATIONAL OR THE CONTRIBUTORS
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

#include <fstream>

namespace
{
	// anonymous namespace is to prevent possible linkage problems or identifier
	// conflict whens the library is dynamically loaded

using std::ifstream;

class OW_AssociatorTest : public OW_CppAssociatorProviderIFC
{
public:
	~OW_AssociatorTest();

	virtual OW_CIMInstanceEnumeration associators(
			const OW_ProviderEnvironmentRef& env,
			OW_CIMObjectPath assocName,
			OW_CIMObjectPath objectName,
			OW_String resultClass,
			OW_String role,
			OW_String resultRole,
			OW_Bool includeQualifiers,
			OW_Bool includeClassOrigin,
			const OW_StringArray* propertyList);

	virtual OW_CIMObjectPathEnumeration associatorNames(
			const OW_ProviderEnvironmentRef& env,
			OW_CIMObjectPath assocName,
			OW_CIMObjectPath objectName,
			OW_String resultClass,
			OW_String role,
			OW_String resultRole );

	virtual OW_CIMInstanceEnumeration references(
			const OW_ProviderEnvironmentRef& env,
			OW_CIMObjectPath assocName,
			OW_CIMObjectPath objectName,
			OW_String role,
			OW_Bool includeQualifiers,
			OW_Bool includeClassOrigin,
			const OW_StringArray* propertyList );

	virtual OW_CIMObjectPathEnumeration referenceNames(
			const OW_ProviderEnvironmentRef& env,
			OW_CIMObjectPath assocName,
			OW_CIMObjectPath objectName,
			OW_String role );

	virtual void initialize(const OW_ProviderEnvironmentRef& env);
	virtual void cleanup();

};

//////////////////////////////////////////////////////////////////////////////
OW_AssociatorTest::~OW_AssociatorTest()
{
}

static OW_String getDestClass(const OW_CIMInstance& ci)
{

	OW_String classOfSource = ci.getClassName();
	if (classOfSource.equalsIgnoreCase("EXP_DemoComputerSystem"))
	{
		return "EXP_UnitaryComputerSystem";
	}
	else if (classOfSource.equalsIgnoreCase("EXP_UnitaryComputerSystem"))
	{
		return "EXP_DemoComputerSystem";
	}
	else
	{
		return "";
	}
}

OW_CIMInstanceEnumeration OW_AssociatorTest::associators(
		const OW_ProviderEnvironmentRef& env,
		OW_CIMObjectPath assocName,
		OW_CIMObjectPath objectName,
		OW_String resultClass,
		OW_String role,
		OW_String resultRole,
		OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList)
{
	env->getLogger()->logDebug(format("OW_AssociatorTest associators called "
				"assocName = %1, objectName = %2, resultClass = %3, "
				"role = %4, resultRole = %5, includeQualifiers = %6, "
				"includeClassOrigin = %7, propertyList = %8",
				assocName.toString(), objectName.toString(),
				resultClass, role, resultRole, includeQualifiers,
				includeClassOrigin, propertyList));
	
	OW_CIMOMHandleRef hdl = env->getCIMOMHandle();

	OW_CIMInstance ci = hdl->getInstance(objectName, false);
	if (!ci)
		return OW_CIMInstanceEnumeration();

	OW_String destClass = getDestClass(ci);

	if (destClass.length() == 0)
		return OW_CIMInstanceEnumeration();

	OW_CIMObjectPath classOne(destClass, assocName.getNameSpace());
	// Just assume that all other instances of the other class are associated!
	return hdl->enumInstances(classOne, true);

}

OW_CIMObjectPathEnumeration OW_AssociatorTest::associatorNames(
		const OW_ProviderEnvironmentRef& env,
		OW_CIMObjectPath assocName,
		OW_CIMObjectPath objectName,
		OW_String resultClass,
		OW_String role,
		OW_String resultRole )
{
	env->getLogger()->logDebug(format("OW_AssociatorTest associatorNames called "
				"assocName = %1, objectName = %2, resultClass = %3, "
				"role = %4, resultRole = %5",
				assocName.toString(), objectName.toString(), resultClass,
				role, resultRole));

	OW_CIMObjectPathEnumeration retval;
	OW_CIMOMHandleRef hdl = env->getCIMOMHandle();
	OW_CIMInstance ci = hdl->getInstance(objectName, false);
	if (!ci)
	{
		return retval;
	}

	OW_String destClass = getDestClass(ci);

	if (destClass.length() == 0)
	{
		return retval;
	}

	OW_CIMObjectPath classOne(destClass, assocName.getNameSpace());
	OW_CIMInstanceEnumeration instances = hdl->enumInstances(classOne, true, false);
	while (instances.hasMoreElements())
	{
		OW_CIMInstance ci = instances.nextElement();
		OW_CIMObjectPath cop(ci.getClassName(), ci.getKeyValuePairs());
		retval.addElement(cop);
	}

	return retval;
}

OW_CIMInstanceEnumeration OW_AssociatorTest::references(
		const OW_ProviderEnvironmentRef& env,
		OW_CIMObjectPath assocName,
		OW_CIMObjectPath objectName,
		OW_String role,
		OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList )
{
	env->getLogger()->logDebug(format("OW_AssociatorTest references called "
				"assocName = %1, objectName = %2, role = %3, "
				"includeQualifiers = %4, includeClassOrigin = %5, "
				"propertyList = %6",
				assocName.toString(), objectName.toString(), role,
				includeQualifiers, includeClassOrigin, propertyList));

	OW_CIMInstanceEnumeration retval;

	OW_CIMOMHandleRef hdl = env->getCIMOMHandle();
	OW_CIMInstance ci = hdl->getInstance(objectName, false);
	if (!ci)
		return retval;

	OW_String destClass = getDestClass(ci);

	if (destClass.length() == 0)
		return retval;

	OW_CIMObjectPath classOne(destClass, assocName.getNameSpace());
	OW_CIMInstanceEnumeration e1 = hdl->enumInstances(classOne, true);

	// Just assume that all other instances of the other class are associated!
	while (e1.hasMoreElements())
	{
		OW_CIMClass cc = hdl->getClass(assocName, false);
		OW_CIMInstance newInstance = cc.newInstance();
		OW_CIMInstance ci = e1.nextElement();
		OW_CIMObjectPath path(ci.getClassName(),
				ci.getKeyValuePairs());

		newInstance.setProperty("firstRef", OW_CIMValue(objectName));

		newInstance.setProperty("secondRef", OW_CIMValue(path));

		retval.addElement(newInstance);
	}
	return retval;
}

OW_CIMObjectPathEnumeration OW_AssociatorTest::referenceNames(
		const OW_ProviderEnvironmentRef& env,
		OW_CIMObjectPath assocName,
		OW_CIMObjectPath objectName,
		OW_String role )
{
	env->getLogger()->logDebug(format("OW_AssociatorTest referenceNames called "
				"assocName = %1, objectName = %2, role = %3",
				assocName.toString(), objectName.toString(), role));

	OW_CIMObjectPathEnumeration retval;
	
	OW_CIMOMHandleRef hdl = env->getCIMOMHandle();
	OW_CIMInstance ci = hdl->getInstance(objectName, false);
	if (!ci)
		return retval;

	OW_String destClass = getDestClass(ci);

	if (destClass.length() == 0)
		return retval;

	OW_CIMObjectPath classOne(destClass, assocName.getNameSpace());
	OW_CIMInstanceEnumeration e1 = hdl->enumInstances(classOne, true, false);

	// Just assume that all other instances of the other class are associated!
	while (e1.hasMoreElements())
	{
		OW_CIMClass cc = hdl->getClass(assocName, false);
		OW_CIMInstance newInstance = cc.newInstance();

		OW_CIMInstance ci = e1.nextElement();
		OW_CIMObjectPath path(ci.getClassName(),
				ci.getKeyValuePairs());

		newInstance.setProperty("firstRef", OW_CIMValue(objectName));

		newInstance.setProperty("secondRef", OW_CIMValue(path));

		OW_CIMObjectPath newPath(assocName.getObjectName(),
				newInstance.getKeyValuePairs());
		
		newPath.setNameSpace(assocName.getNameSpace());
		retval.addElement(newPath);
	}
	return retval;
}

//////////////////////////////////////////////////////////////////////////////
void
OW_AssociatorTest::initialize(const OW_ProviderEnvironmentRef& env)
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
OW_PROVIDERFACTORY(OW_AssociatorTest);

