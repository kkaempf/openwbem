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
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMClassEnumeration.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"
#include "OW_CIMQualifierEnumeration.hpp"

const OW_Bool OW_CIMOMHandleIFC::DEEP(true);
const OW_Bool OW_CIMOMHandleIFC::SHALLOW(false);
const OW_Bool OW_CIMOMHandleIFC::INCLUDE_QUALIFIERS(true);
const OW_Bool OW_CIMOMHandleIFC::EXCLUDE_QUALIFIERS(false);
const OW_Bool OW_CIMOMHandleIFC::INCLUDE_CLASS_ORIGIN(true);
const OW_Bool OW_CIMOMHandleIFC::EXCLUDE_CLASS_ORIGIN(false);
const OW_Bool OW_CIMOMHandleIFC::LOCAL_ONLY(true);
const OW_Bool OW_CIMOMHandleIFC::NOT_LOCAL_ONLY(false);

OW_CIMFeatures
OW_CIMOMHandleIFC::getServerFeatures()
{
	return OW_CIMFeatures();
}

void
OW_CIMOMHandleIFC::exportIndication(const OW_CIMInstance& instance,
		const OW_CIMNameSpace& instNS)
{
	OW_THROWCIM(OW_CIMException::FAILED);
	(void)instance;
	(void)instNS;
}

//////////////////////////////////////////////////////////////////////////////
namespace
{
	class CIMClassEnumBuilder : public OW_CIMClassResultHandlerIFC
	{
	public:
		CIMClassEnumBuilder(OW_CIMClassEnumeration& e) : m_e(e) {}
	protected:
		virtual void doHandleClass(const OW_CIMClass &c)
		{
			m_e.addElement(c);
		}
	private:
		OW_CIMClassEnumeration& m_e;
	};
	
	class StringArrayBuilder : public OW_StringResultHandlerIFC
	{
	public:
		StringArrayBuilder(OW_StringArray& a) : m_a(a) {}
	protected:
		virtual void doHandleString(const OW_String &s)
		{
			m_a.push_back(s);
		}
	private:
		OW_StringArray& m_a;
	};
	
	class CIMObjectPathEnumBuilder : public OW_CIMObjectPathResultHandlerIFC
	{
	public:
		CIMObjectPathEnumBuilder(OW_CIMObjectPathEnumeration& e) : m_e(e) {}
	protected:
		virtual void doHandleObjectPath(const OW_CIMObjectPath &cop)
		{
			m_e.addElement(cop);
		}
	private:
		OW_CIMObjectPathEnumeration& m_e;
	};

	class CIMInstanceEnumBuilder : public OW_CIMInstanceResultHandlerIFC
	{
	public:
		CIMInstanceEnumBuilder(OW_CIMInstanceEnumeration& e) : m_e(e) {}
	protected:
		virtual void doHandleInstance(const OW_CIMInstance &i)
		{
			m_e.addElement(i);
		}
	private:
		OW_CIMInstanceEnumeration& m_e;
	};
	
	class CIMQualifierTypeEnumBuilder : public OW_CIMQualifierTypeResultHandlerIFC
	{
	public:
		CIMQualifierTypeEnumBuilder(OW_CIMQualifierTypeEnumeration& e) : m_e(e) {}
	protected:
		virtual void doHandleQualifierType(const OW_CIMQualifierType &qt)
		{
			m_e.addElement(qt);
		}
	private:
		OW_CIMQualifierTypeEnumeration& m_e;
	};

}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClassEnumeration
OW_CIMOMHandleIFC::enumClassE(const OW_CIMObjectPath& path, OW_Bool deep,
	OW_Bool localOnly, OW_Bool includeQualifiers, OW_Bool includeClassOrigin)
{
	OW_CIMClassEnumeration rval;
	CIMClassEnumBuilder handler(rval);
	enumClass(path, handler, deep, localOnly, includeQualifiers,
		includeClassOrigin);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_StringArray
OW_CIMOMHandleIFC::enumNameSpaceE(const OW_CIMNameSpace& ns,
		OW_Bool deep)
{
	OW_StringArray rval;
	StringArrayBuilder handler(rval);
	enumNameSpace(ns, handler, deep);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_CIMOMHandleIFC::enumClassNamesE(const OW_CIMObjectPath& path, OW_Bool deep)
{
	OW_CIMObjectPathEnumeration rval;
	CIMObjectPathEnumBuilder handler(rval);
	enumClassNames(path, handler, deep);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_CIMOMHandleIFC::enumInstancesE(
		const OW_CIMObjectPath& path,
		OW_Bool deep,
		OW_Bool localOnly,
		OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList)
{
	OW_CIMInstanceEnumeration rval;
	CIMInstanceEnumBuilder handler(rval);
	enumInstances(path,handler,deep,localOnly,includeQualifiers,
		includeClassOrigin,propertyList);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_CIMOMHandleIFC::enumInstanceNamesE(
		const OW_CIMObjectPath& path)
{
	OW_CIMObjectPathEnumeration rval;
	CIMObjectPathEnumBuilder handler(rval);
	enumInstanceNames(path, handler);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMQualifierTypeEnumeration
OW_CIMOMHandleIFC::enumQualifierTypesE(
		const OW_CIMObjectPath& path)
{
	OW_CIMQualifierTypeEnumeration rval;
	CIMQualifierTypeEnumBuilder handler(rval);
	enumQualifierTypes(path, handler);
	return rval;
}


//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_CIMOMHandleIFC::associatorNamesE(
		const OW_CIMObjectPath& objectName,
		const OW_String& assocClass,
		const OW_String& resultClass,
		const OW_String& role,
		const OW_String& resultRole)
{
	OW_CIMObjectPathEnumeration rval;
	CIMObjectPathEnumBuilder handler(rval);
	associatorNames(objectName,handler,assocClass,resultClass,role,resultRole);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_CIMOMHandleIFC::associatorsE(
		const OW_CIMObjectPath& path,
		const OW_String& assocClass,
		const OW_String& resultClass,
		const OW_String& role,
		const OW_String& resultRole,
		OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList)
{
	OW_CIMInstanceEnumeration rval;
	CIMInstanceEnumBuilder handler(rval);
	associators(path, handler, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList);	
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClassEnumeration
OW_CIMOMHandleIFC::associatorsClassesE(
		const OW_CIMObjectPath& path,
		const OW_String& assocClass,
		const OW_String& resultClass,
		const OW_String& role,
		const OW_String& resultRole,
		OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList)
{
	OW_CIMClassEnumeration rval;
	CIMClassEnumBuilder handler(rval);
	associatorsClasses(path, handler, assocClass, resultClass, role, resultRole,
		includeQualifiers, includeClassOrigin, propertyList);	
	return rval;
}


//////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_CIMOMHandleIFC::referenceNamesE(
		const OW_CIMObjectPath& path,
		const OW_String& resultClass,
		const OW_String& role)
{
	OW_CIMObjectPathEnumeration rval;
	CIMObjectPathEnumBuilder handler(rval);
	referenceNames(path,handler,resultClass,role);
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_CIMOMHandleIFC::referencesE(
		const OW_CIMObjectPath& path,
		const OW_String& resultClass,
		const OW_String& role,
		OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList)
{
	OW_CIMInstanceEnumeration rval;
	CIMInstanceEnumBuilder handler(rval);
	references(path, handler, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList);	
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMClassEnumeration
OW_CIMOMHandleIFC::referencesClassesE(
		const OW_CIMObjectPath& path,
		const OW_String& resultClass,
		const OW_String& role,
		OW_Bool includeQualifiers,
		OW_Bool includeClassOrigin,
		const OW_StringArray* propertyList)
{
	OW_CIMClassEnumeration rval;
	CIMClassEnumBuilder handler(rval);
	referencesClasses(path, handler, resultClass, role,
		includeQualifiers, includeClassOrigin, propertyList);	
	return rval;
}

//////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_CIMOMHandleIFC::execQueryE(
	const OW_CIMNameSpace& ns,
	const OW_String& query,
	const OW_String& queryLanguage)
{
	OW_CIMInstanceEnumeration rval;
	CIMInstanceEnumBuilder handler(rval);
	execQuery(ns,handler,query,queryLanguage);
	return rval;
}


