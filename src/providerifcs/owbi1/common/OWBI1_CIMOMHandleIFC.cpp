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

#include "OWBI1_config.h"
#include "OWBI1_CIMOMHandleIFC.hpp"
#include "OWBI1_CIMInstance.hpp"
#include "OWBI1_CIMNameSpace.hpp"
#include "OWBI1_CIMException.hpp"
#include "OWBI1_CIMValue.hpp"
#include "OWBI1_CIMClass.hpp"
#include "OWBI1_CIMObjectPath.hpp"
#include "OWBI1_CIMQualifierType.hpp"
#include "OWBI1_Array.hpp"
#include "OWBI1_ResultHandlers.hpp"
#include "OWBI1_Enumeration.hpp"

namespace OWBI1
{

using namespace WBEMFlags;
//////////////////////////////////////////////////////////////////////////////
CIMOMHandleIFC::~CIMOMHandleIFC() 
{
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMHandleIFC::exportIndication(const CIMInstance& instance,
		const String& instNS)
{
	OWBI1_THROWCIM(CIMException::FAILED);
}
//////////////////////////////////////////////////////////////////////////////
CIMClassEnumeration
CIMOMHandleIFC::enumClassE(const String& ns,
	const CIMName& className, 
	EDeepFlag deep,
	ELocalOnlyFlag localOnly)
{
	CIMClassEnumeration rval;
	CIMClassEnumBuilder handler(rval);
	enumClass(ns, className, handler, deep, localOnly);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMClassArray
CIMOMHandleIFC::enumClassA(const String& ns,
	const CIMName& className, 
	EDeepFlag deep,
	ELocalOnlyFlag localOnly)
{
	CIMClassArray rval;
	CIMClassArrayBuilder handler(rval);
	enumClass(ns, className, handler, deep, localOnly);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMNameEnumeration
CIMOMHandleIFC::enumClassNamesE(
	const String& ns,
	const CIMName& className,
	EDeepFlag deep)
{
	CIMNameEnumeration rval;
	CIMNameEnumBuilder handler(rval);
	enumClassNames(ns, className, handler, deep);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMNameArray
CIMOMHandleIFC::enumClassNamesA(
	const String& ns,
	const CIMName& className,
	EDeepFlag deep)
{
	CIMNameArray rval;
	CIMNameArrayBuilder handler(rval);
	enumClassNames(ns, className, handler, deep);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstanceEnumeration
CIMOMHandleIFC::enumInstancesE(
		const String& ns,
		const CIMName& className,
		const CIMPropertyList& propertyList)
{
	CIMInstanceEnumeration rval;
	CIMInstanceEnumBuilder handler(rval);
	enumInstances(ns, className, handler,propertyList);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstanceArray
CIMOMHandleIFC::enumInstancesA(
		const String& ns,
		const CIMName& className,
		const CIMPropertyList& propertyList)
{
	CIMInstanceArray rval;
	CIMInstanceArrayBuilder handler(rval);
	enumInstances(ns, className, handler,propertyList);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPathEnumeration
CIMOMHandleIFC::enumInstanceNamesE(
	const String& ns,
	const CIMName& className)
{
	CIMObjectPathEnumeration rval;
	CIMObjectPathEnumBuilder handler(rval);
	enumInstanceNames(ns, className, handler);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPathArray
CIMOMHandleIFC::enumInstanceNamesA(
	const String& ns,
	const CIMName& className)
{
	CIMObjectPathArray rval;
	CIMObjectPathArrayBuilder handler(rval);
	enumInstanceNames(ns, className, handler);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierTypeEnumeration
CIMOMHandleIFC::enumQualifierTypesE(
	const String& ns)
{
	CIMQualifierTypeEnumeration rval;
	CIMQualifierTypeEnumBuilder handler(rval);
	enumQualifierTypes(ns, handler);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMQualifierTypeArray
CIMOMHandleIFC::enumQualifierTypesA(
	const String& ns)
{
	CIMQualifierTypeArray rval;
	CIMQualifierTypeArrayBuilder handler(rval);
	enumQualifierTypes(ns, handler);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPathEnumeration
CIMOMHandleIFC::associatorNamesE(
		const String& ns,
		const CIMObjectPath& objectName,
		const CIMName& assocClass,
		const CIMName& resultClass,
		const CIMName& role,
		const CIMName& resultRole)
{
	CIMObjectPathEnumeration rval;
	CIMObjectPathEnumBuilder handler(rval);
	associatorNames(ns,objectName,handler,assocClass,resultClass,role,resultRole);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPathArray
CIMOMHandleIFC::associatorNamesA(
		const String& ns,
		const CIMObjectPath& objectName,
		const CIMName& assocClass,
		const CIMName& resultClass,
		const CIMName& role,
		const CIMName& resultRole)
{
	CIMObjectPathArray rval;
	CIMObjectPathArrayBuilder handler(rval);
	associatorNames(ns,objectName,handler,assocClass,resultClass,role,resultRole);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstanceEnumeration
CIMOMHandleIFC::associatorsE(
		const String& ns,
		const CIMObjectPath& path,
		const CIMName& assocClass,
		const CIMName& resultClass,
		const CIMName& role,
		const CIMName& resultRole,
		const CIMPropertyList& propertyList)
{
	CIMInstanceEnumeration rval;
	CIMInstanceEnumBuilder handler(rval);
	associators(ns, path, handler, assocClass, resultClass, role, resultRole,
		propertyList);	
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstanceArray
CIMOMHandleIFC::associatorsA(
		const String& ns,
		const CIMObjectPath& path,
		const CIMName& assocClass,
		const CIMName& resultClass,
		const CIMName& role,
		const CIMName& resultRole,
		const CIMPropertyList& propertyList)
{
	CIMInstanceArray rval;
	CIMInstanceArrayBuilder handler(rval);
	associators(ns, path, handler, assocClass, resultClass, role, resultRole,
		propertyList);	
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMClassEnumeration
CIMOMHandleIFC::associatorsClassesE(
		const String& ns,
		const CIMObjectPath& path,
		const CIMName& assocClass,
		const CIMName& resultClass,
		const CIMName& role,
		const CIMName& resultRole,
		const CIMPropertyList& propertyList)
{
	CIMClassEnumeration rval;
	CIMClassEnumBuilder handler(rval);
	associatorsClasses(ns, path, handler, assocClass, resultClass, role, resultRole,
		propertyList);	
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMClassArray
CIMOMHandleIFC::associatorsClassesA(
		const String& ns,
		const CIMObjectPath& path,
		const CIMName& assocClass,
		const CIMName& resultClass,
		const CIMName& role,
		const CIMName& resultRole,
		const CIMPropertyList& propertyList)
{
	CIMClassArray rval;
	CIMClassArrayBuilder handler(rval);
	associatorsClasses(ns, path, handler, assocClass, resultClass, role, resultRole,
		propertyList);	
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPathEnumeration
CIMOMHandleIFC::referenceNamesE(
	const String& ns,
		const CIMObjectPath& path,
		const CIMName& resultClass,
		const CIMName& role)
{
	CIMObjectPathEnumeration rval;
	CIMObjectPathEnumBuilder handler(rval);
	referenceNames(ns,path,handler,resultClass,role);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPathArray
CIMOMHandleIFC::referenceNamesA(
	const String& ns,
		const CIMObjectPath& path,
		const CIMName& resultClass,
		const CIMName& role)
{
	CIMObjectPathArray rval;
	CIMObjectPathArrayBuilder handler(rval);
	referenceNames(ns,path,handler,resultClass,role);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstanceEnumeration
CIMOMHandleIFC::referencesE(
		const String& ns,
		const CIMObjectPath& path,
		const CIMName& resultClass,
		const CIMName& role,
		const CIMPropertyList& propertyList)
{
	CIMInstanceEnumeration rval;
	CIMInstanceEnumBuilder handler(rval);
	references(ns, path, handler, resultClass, role,
		propertyList);	
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstanceArray
CIMOMHandleIFC::referencesA(
		const String& ns,
		const CIMObjectPath& path,
		const CIMName& resultClass,
		const CIMName& role,
		const CIMPropertyList& propertyList)
{
	CIMInstanceArray rval;
	CIMInstanceArrayBuilder handler(rval);
	references(ns, path, handler, resultClass, role,
		propertyList);	
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMClassEnumeration
CIMOMHandleIFC::referencesClassesE(
		const String& ns,
		const CIMObjectPath& path,
		const CIMName& resultClass,
		const CIMName& role,
		const CIMPropertyList& propertyList)
{
	CIMClassEnumeration rval;
	CIMClassEnumBuilder handler(rval);
	referencesClasses(ns, path, handler, resultClass, role,
		propertyList);	
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMClassArray
CIMOMHandleIFC::referencesClassesA(
		const String& ns,
		const CIMObjectPath& path,
		const CIMName& resultClass,
		const CIMName& role,
		const CIMPropertyList& propertyList)
{
	CIMClassArray rval;
	CIMClassArrayBuilder handler(rval);
	referencesClasses(ns, path, handler, resultClass, role,
		propertyList);	
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstanceEnumeration
CIMOMHandleIFC::execQueryE(
	const String& ns,
	const String& query,
	const String& queryLanguage)
{
	CIMInstanceEnumeration rval;
	CIMInstanceEnumBuilder handler(rval);
	execQuery(ns,handler,query,queryLanguage);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstanceArray
CIMOMHandleIFC::execQueryA(
	const String& ns,
	const String& query,
	const String& queryLanguage)
{
	CIMInstanceArray rval;
	CIMInstanceArrayBuilder handler(rval);
	execQuery(ns,handler,query,queryLanguage);
	return rval;
}

} // end namespace OWBI1

