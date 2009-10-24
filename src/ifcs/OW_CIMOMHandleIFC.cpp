/*******************************************************************************
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMClassEnumeration.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"
#include "OW_CIMQualifierEnumeration.hpp"
#include "OW_CIMValue.hpp"
#include "blocxx/Array.hpp"
#include "OW_ResultHandlers.hpp"

namespace OW_NAMESPACE
{

using namespace WBEMFlags;
//////////////////////////////////////////////////////////////////////////////
CIMOMHandleIFC::~CIMOMHandleIFC()
{
}
//////////////////////////////////////////////////////////////////////////////
CIMFeatures
CIMOMHandleIFC::getServerFeatures()
{
	return CIMFeatures();
}
//////////////////////////////////////////////////////////////////////////////
void
CIMOMHandleIFC::exportIndication(const CIMInstance& instance,
		const String& instNS)
{
	OW_THROWCIM(CIMException::FAILED);
}
//////////////////////////////////////////////////////////////////////////////
CIMClassEnumeration
CIMOMHandleIFC::enumClassE(const String& ns,
	const String& className,
	EDeepFlag deep,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin)
{
	CIMClassEnumeration rval;
	CIMClassEnumBuilder handler(rval);
	enumClass(ns, className, handler, deep, localOnly, includeQualifiers,
		includeClassOrigin);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMClassArray
CIMOMHandleIFC::enumClassA(const String& ns,
	const String& className,
	EDeepFlag deep,
	ELocalOnlyFlag localOnly, EIncludeQualifiersFlag includeQualifiers, EIncludeClassOriginFlag includeClassOrigin)
{
	CIMClassArray rval;
	CIMClassArrayBuilder handler(rval);
	enumClass(ns, className, handler, deep, localOnly, includeQualifiers,
		includeClassOrigin);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
StringEnumeration
CIMOMHandleIFC::enumClassNamesE(
	const String& ns,
	const String& className,
	EDeepFlag deep)
{
	StringEnumeration rval;
	StringEnumBuilder handler(rval);
	enumClassNames(ns, className, handler, deep);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
StringArray
CIMOMHandleIFC::enumClassNamesA(
	const String& ns,
	const String& className,
	EDeepFlag deep)
{
	StringArray rval;
	StringArrayBuilder handler(rval);
	enumClassNames(ns, className, handler, deep);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstanceEnumeration
CIMOMHandleIFC::enumInstancesE(
		const String& ns,
		const String& className,
		EDeepFlag deep,
		ELocalOnlyFlag localOnly,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList)
{
	CIMInstanceEnumeration rval;
	CIMInstanceEnumBuilder handler(rval);
	enumInstances(ns, className, handler,deep,localOnly,includeQualifiers,
		includeClassOrigin,propertyList);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstanceArray
CIMOMHandleIFC::enumInstancesA(
		const String& ns,
		const String& className,
		EDeepFlag deep,
		ELocalOnlyFlag localOnly,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList)
{
	CIMInstanceArray rval;
	CIMInstanceArrayBuilder handler(rval);
	enumInstances(ns, className, handler,deep,localOnly,includeQualifiers,
		includeClassOrigin,propertyList);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPathEnumeration
CIMOMHandleIFC::enumInstanceNamesE(
	const String& ns,
	const String& className)
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
	const String& className)
{
	CIMObjectPathArray rval;
	CIMObjectPathArrayBuilder handler(rval);
	enumInstanceNames(ns, className, handler);
	return rval;
}
#ifndef OW_DISABLE_QUALIFIER_DECLARATION
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
#endif
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
CIMObjectPathEnumeration
CIMOMHandleIFC::associatorNamesE(
		const String& ns,
		const CIMObjectPath& objectName,
		const String& assocClass,
		const String& resultClass,
		const String& role,
		const String& resultRole)
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
		const String& assocClass,
		const String& resultClass,
		const String& role,
		const String& resultRole)
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
		const String& assocClass,
		const String& resultClass,
		const String& role,
		const String& resultRole,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList)
{
	CIMInstanceEnumeration rval;
	CIMInstanceEnumBuilder handler(rval);
	associators(ns, path, handler, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstanceArray
CIMOMHandleIFC::associatorsA(
		const String& ns,
		const CIMObjectPath& path,
		const String& assocClass,
		const String& resultClass,
		const String& role,
		const String& resultRole,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList)
{
	CIMInstanceArray rval;
	CIMInstanceArrayBuilder handler(rval);
	associators(ns, path, handler, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMClassEnumeration
CIMOMHandleIFC::associatorsClassesE(
		const String& ns,
		const CIMObjectPath& path,
		const String& assocClass,
		const String& resultClass,
		const String& role,
		const String& resultRole,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList)
{
	CIMClassEnumeration rval;
	CIMClassEnumBuilder handler(rval);
	associatorsClasses(ns, path, handler, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMClassArray
CIMOMHandleIFC::associatorsClassesA(
		const String& ns,
		const CIMObjectPath& path,
		const String& assocClass,
		const String& resultClass,
		const String& role,
		const String& resultRole,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList)
{
	CIMClassArray rval;
	CIMClassArrayBuilder handler(rval);
	associatorsClasses(ns, path, handler, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMObjectPathEnumeration
CIMOMHandleIFC::referenceNamesE(
	const String& ns,
		const CIMObjectPath& path,
		const String& resultClass,
		const String& role)
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
		const String& resultClass,
		const String& role)
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
		const String& resultClass,
		const String& role,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList)
{
	CIMInstanceEnumeration rval;
	CIMInstanceEnumBuilder handler(rval);
	references(ns, path, handler, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMInstanceArray
CIMOMHandleIFC::referencesA(
		const String& ns,
		const CIMObjectPath& path,
		const String& resultClass,
		const String& role,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList)
{
	CIMInstanceArray rval;
	CIMInstanceArrayBuilder handler(rval);
	references(ns, path, handler, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMClassEnumeration
CIMOMHandleIFC::referencesClassesE(
		const String& ns,
		const CIMObjectPath& path,
		const String& resultClass,
		const String& role,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList)
{
	CIMClassEnumeration rval;
	CIMClassEnumBuilder handler(rval);
	referencesClasses(ns, path, handler, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList);
	return rval;
}
//////////////////////////////////////////////////////////////////////////////
CIMClassArray
CIMOMHandleIFC::referencesClassesA(
		const String& ns,
		const CIMObjectPath& path,
		const String& resultClass,
		const String& role,
		EIncludeQualifiersFlag includeQualifiers,
		EIncludeClassOriginFlag includeClassOrigin,
		const StringArray* propertyList)
{
	CIMClassArray rval;
	CIMClassArrayBuilder handler(rval);
	referencesClasses(ns, path, handler, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList);
	return rval;
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
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

} // end namespace OW_NAMESPACE

