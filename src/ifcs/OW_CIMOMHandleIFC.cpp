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
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_CIMFeatures.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMNameSpace.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMClassEnumeration.hpp"
#include "OW_CIMObjectPathEnumeration.hpp"
#include "OW_CIMQualifierEnumeration.hpp"
#include "OW_CIMValue.hpp"

namespace OpenWBEM
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
	(void)instance;
	(void)instNS;
}
//////////////////////////////////////////////////////////////////////////////
namespace
{
    // TODO: Put these into a common header.  Make them available for re-use.
	class CIMClassEnumBuilder : public CIMClassResultHandlerIFC
	{
	public:
		CIMClassEnumBuilder(CIMClassEnumeration& e) : m_e(e) {}
	protected:
		virtual void doHandle(const CIMClass &c)
		{
			m_e.addElement(c);
		}
	private:
		CIMClassEnumeration& m_e;
	};
	
	class StringArrayBuilder : public StringResultHandlerIFC
	{
	public:
		StringArrayBuilder(StringArray& a) : m_a(a) {}
	protected:
		virtual void doHandle(const String &s)
		{
			m_a.push_back(s);
		}
	private:
		StringArray& m_a;
	};
	
	class StringEnumBuilder : public StringResultHandlerIFC
	{
	public:
		StringEnumBuilder(StringEnumeration& e) : m_e(e) {}
	protected:
		virtual void doHandle(const String &s)
		{
			m_e.addElement(s);
		}
	private:
		StringEnumeration& m_e;
	};
	
	class CIMObjectPathEnumBuilder : public CIMObjectPathResultHandlerIFC
	{
	public:
		CIMObjectPathEnumBuilder(CIMObjectPathEnumeration& e) : m_e(e) {}
	protected:
		virtual void doHandle(const CIMObjectPath &cop)
		{
			m_e.addElement(cop);
		}
	private:
		CIMObjectPathEnumeration& m_e;
	};
	class CIMInstanceEnumBuilder : public CIMInstanceResultHandlerIFC
	{
	public:
		CIMInstanceEnumBuilder(CIMInstanceEnumeration& e) : m_e(e) {}
	protected:
		virtual void doHandle(const CIMInstance &i)
		{
			m_e.addElement(i);
		}
	private:
		CIMInstanceEnumeration& m_e;
	};
	
	class CIMQualifierTypeEnumBuilder : public CIMQualifierTypeResultHandlerIFC
	{
	public:
		CIMQualifierTypeEnumBuilder(CIMQualifierTypeEnumeration& e) : m_e(e) {}
	protected:
		virtual void doHandle(const CIMQualifierType &qt)
		{
			m_e.addElement(qt);
		}
	private:
		CIMQualifierTypeEnumeration& m_e;
	};
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

} // end namespace OpenWBEM

