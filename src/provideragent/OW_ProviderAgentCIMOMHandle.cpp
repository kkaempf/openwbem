/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
* Copyright (C) 2004 Novell, Inc. All rights reserved.
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
#include "OW_ProviderAgentCIMOMHandle.hpp"
#include "OW_CIMInstance.hpp"
#include "OW_CIMException.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMQualifierType.hpp"
#include "OW_CIMProperty.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CppProviderBaseIFC.hpp"
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CppSecondaryInstanceProviderIFC.hpp"
#include "OW_CppMethodProviderIFC.hpp"
#include "OW_CppAssociatorProviderIFC.hpp"
#include "OW_ProviderAgent.hpp"
#include "OW_Assertion.hpp"
#include "OW_ConfigException.hpp"

namespace OpenWBEM
{

//using namespace WBEMFlags;

ProviderAgentCIMOMHandle::ProviderAgentCIMOMHandle(
	const Map<String, CppProviderBaseIFCRef>& assocProvs, 
	const Map<String, CppProviderBaseIFCRef>& instProvs, 
	const Map<String, CppProviderBaseIFCRef>& secondaryInstProvs, 
	const Map<String, CppProviderBaseIFCRef>& methodProvs, 
	Cache<CIMClass>& cimClasses, 
	const ProviderEnvironmentIFCRef& env, 
	ProviderAgentEnvironment::ClassRetrievalFlag classRetrieval, 
	const ProviderAgentLockerIFCRef& locker)
	: m_assocProvs(assocProvs)
	, m_instProvs(instProvs)
	, m_secondaryInstProvs(secondaryInstProvs)
	, m_methodProvs(methodProvs)
	, m_cimClasses(cimClasses)
	, m_PAEnv(env)
	, m_locker(locker)
	, m_classRetrieval(classRetrieval)
{
}

//////////////////////////////////////////////////////////////////////////////
CIMInstance 
ProviderAgentCIMOMHandle::getInstance(const String &ns, 
			const CIMObjectPath &instanceName, 
			WBEMFlags:: ELocalOnlyFlag localOnly,
			WBEMFlags:: EIncludeQualifiersFlag includeQualifiers,
			WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin,
			const StringArray *propertyList)
{
	CppInstanceProviderIFC* pInstProv = getInstanceProvider(ns, instanceName.getClassName()); 
	CppSecondaryInstanceProviderIFC* pSInstProv = getSecondaryInstanceProvider(ns, instanceName.getClassName()); 
	if (!pInstProv && !pSInstProv)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	}
	CIMInstance rval(CIMNULL); 
	{
		PAReadLock rl(m_locker); 
		if (pInstProv)
		{
			rval = pInstProv->getInstance(m_PAEnv,ns ,instanceName ,localOnly ,
					includeQualifiers ,includeClassOrigin ,
					propertyList , helperGetClass(ns, instanceName.getClassName())); 
		}
		if (pSInstProv)
		{
			CIMInstanceArray ia; 
			if (pInstProv)
			{
				ia.push_back(rval); 
			}
			else
			{
				CIMInstance newInst(instanceName.getClassName()); 
				newInst.setProperties(instanceName.getKeys()); 
				newInst.setKeys(instanceName.getKeys()); 
				ia.push_back(newInst); 
			}
			CIMClass cc = helperGetClass(ns, instanceName.getClassName()); 
			pSInstProv->filterInstances(m_PAEnv,ns , instanceName.getClassName(), 
                                        ia,localOnly , OpenWBEM::WBEMFlags::E_SHALLOW, 
										includeQualifiers, includeClassOrigin, 
										propertyList, cc, cc); // TODO should the classes be different? 
			OW_ASSERT(ia.size() == 1); // did the secondary instance provider do something horribly wrong? 
			rval = ia[0]; 
		}
	}
	return rval; 
}

///////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentCIMOMHandle::enumQualifierTypes(const String &ns, 
			CIMQualifierTypeResultHandlerIFC &result)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}

///////////////////////////////////////////////////////////////////////////////
CIMValue 
ProviderAgentCIMOMHandle::invokeMethod(const String &ns, 
			const CIMObjectPath &path, 
			const String &methodName, 
			const CIMParamValueArray &inParams, 
			CIMParamValueArray &outParams)
{
	CppMethodProviderIFC* pMethodProv = getMethodProvider(ns, path.getClassName(), methodName); 
	if (!pMethodProv)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	}
	{
		PAWriteLock wl(m_locker); 
		return pMethodProv->invokeMethod(m_PAEnv,ns ,path ,methodName , 
				inParams, outParams); 
	}
}

///////////////////////////////////////////////////////////////////////////////
CIMQualifierType 
ProviderAgentCIMOMHandle::getQualifierType(const String &ns, const String &qualifierName)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}

///////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentCIMOMHandle::setQualifierType(const String &ns, const CIMQualifierType &qualifierType)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}

///////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentCIMOMHandle::deleteQualifierType(const String &ns, const String &qualName)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}

///////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentCIMOMHandle::createClass(const String &ns, const CIMClass &cimClass)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}

///////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentCIMOMHandle::deleteClass(const String &ns, const String &className)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}

///////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentCIMOMHandle::modifyClass(const String &ns, const CIMClass &cimClass)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION

///////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentCIMOMHandle::deleteInstance(const String &ns, const CIMObjectPath &path)
{
	CppInstanceProviderIFC* pInstProv = getInstanceProvider(ns, path.getClassName()); 
	CppSecondaryInstanceProviderIFC* pSInstProv = getSecondaryInstanceProvider(ns, path.getClassName()); 
	if (!pInstProv && !pSInstProv)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	}
	{
		PAWriteLock wl(m_locker); 
		if (pInstProv)
		{
			pInstProv->deleteInstance(m_PAEnv,ns , path); 
		}
		if (pSInstProv)
		{
			pSInstProv->deleteInstance(m_PAEnv,ns , path); 
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentCIMOMHandle::setProperty(const String &ns, 
									  const CIMObjectPath &instanceName, 
									  const String &propertyName, 
									  const CIMValue &newValue)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
		// CIMOM will never demand this of us.  It always translates 
		// to modifyInstance. 
	}

///////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentCIMOMHandle::modifyInstance(const String &ns, 
			const CIMInstance &modifiedInstance, 
			WBEMFlags:: EIncludeQualifiersFlag includeQualifiers,
			const StringArray *propertyList)
	{
		CppInstanceProviderIFC* pInstProv = getInstanceProvider(ns, modifiedInstance.getClassName()); 
		CppSecondaryInstanceProviderIFC* pSInstProv = getSecondaryInstanceProvider(ns, modifiedInstance.getClassName()); 
		if (!pInstProv && !pSInstProv)
		{
			OW_THROWCIM(CIMException::NOT_SUPPORTED); 
		}
		{
			PAWriteLock wl(m_locker); 
			CIMClass cc = helperGetClass(ns, modifiedInstance.getClassName()); 
			if (pInstProv)
			{
				pInstProv->modifyInstance(m_PAEnv,ns ,modifiedInstance , 
						CIMInstance(CIMNULL),	// previousInstance unavailable
						includeQualifiers ,
						propertyList , cc); 
			}
			if (pSInstProv)
			{
				pSInstProv->modifyInstance(m_PAEnv,ns ,modifiedInstance , 
										   CIMInstance(CIMNULL),
										   includeQualifiers ,propertyList , 
										   cc); 
			}
		}
	}

//////////////////////////////////////////////////////////////////////////////
CIMObjectPath 
ProviderAgentCIMOMHandle::createInstance(const String &ns, const CIMInstance &instance)
{
	CppInstanceProviderIFC* pInstProv = getInstanceProvider(ns, instance.getClassName()); 
	CppSecondaryInstanceProviderIFC* pSInstProv = getSecondaryInstanceProvider(ns, instance.getClassName()); 
	if (!pInstProv && !pSInstProv)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	}
	CIMObjectPath rval(ns, instance); // Not CIMNULL since we have to return something for the seconday instance provider.
	{
		PAWriteLock wl(m_locker); 
		if (pInstProv)
		{
			rval = pInstProv->createInstance(m_PAEnv,ns , instance); 
		}
		if (pSInstProv)
		{
			pSInstProv->createInstance(m_PAEnv,ns , instance); 
		}
	}
	return rval; 
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
///////////////////////////////////////////////////////////////////////////////
CIMValue 
ProviderAgentCIMOMHandle::getProperty(const String &ns, 
			const CIMObjectPath &instanceName, 
			const String &propertyName)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	// CIMOM will never demand this of us.  
}

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
///////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentCIMOMHandle::associatorNames(const String &ns, 
			const CIMObjectPath &objectName, 
			CIMObjectPathResultHandlerIFC &result, 
			const String &assocClass,
			const String &resultClass,
			const String &role,
			const String &resultRole)
{
	CppAssociatorProviderIFC* pAssocProv = getAssociatorProvider(ns, assocClass); 
	if (!pAssocProv)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	}
	{
		PAReadLock rl(m_locker); 
		pAssocProv->associatorNames(m_PAEnv,result ,ns ,objectName ,
				assocClass ,resultClass ,role ,resultRole); 
	}
}

///////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentCIMOMHandle::associatorsClasses(const String &ns, 
				const CIMObjectPath &path, 
				CIMClassResultHandlerIFC &result, 
				const String &assocClass, 
				const String &resultClass, 
				const String &role, 
				const String &resultRole, 
				WBEMFlags:: EIncludeQualifiersFlag includeQualifiers, 
				WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin, 
				const StringArray *propertyList)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}

///////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentCIMOMHandle::associators(const String &ns, const CIMObjectPath &path, 
		CIMInstanceResultHandlerIFC &result, 
		const String &assocClass, 
		const String &resultClass, 
		const String &role, 
		const String &resultRole, 
		WBEMFlags:: EIncludeQualifiersFlag 
		includeQualifiers,
		WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin, 
		const StringArray *propertyList)
{
	CppAssociatorProviderIFC* pAssocProv = getAssociatorProvider(ns, assocClass); 
	if (!pAssocProv)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	}
	{
		PAReadLock rl(m_locker); 
		pAssocProv->associators(m_PAEnv,result ,ns , path,assocClass ,
				resultClass ,role ,resultRole ,
				includeQualifiers ,includeClassOrigin ,
				propertyList); 
	}
}

//////////////////////////////////////////////////////////////////////////////
void
ProviderAgentCIMOMHandle::referenceNames(const String &ns, 
		const CIMObjectPath &path, 
		CIMObjectPathResultHandlerIFC &result, 
		const String &resultClass,
		const String &role)
{
	CppAssociatorProviderIFC* pAssocProv = getAssociatorProvider(ns, resultClass); 
	if (!pAssocProv)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	}
	{
		PAReadLock rl(m_locker); 
		pAssocProv->referenceNames(m_PAEnv,result ,ns , path,resultClass ,role); 
	}
}

//////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentCIMOMHandle::references(const String &ns, 
		const CIMObjectPath &path, 
		CIMInstanceResultHandlerIFC &result, 
		const String &resultClass,
		const String &role,
		WBEMFlags:: EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin,
		const StringArray *propertyList)
{
	CppAssociatorProviderIFC* pAssocProv = getAssociatorProvider(ns, resultClass); 
	if (!pAssocProv)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	}
	{
		PAReadLock rl(m_locker); 
		pAssocProv->references(m_PAEnv,result ,ns , path,resultClass ,role ,
				includeQualifiers ,
				includeClassOrigin ,
				propertyList); 
	}
}

//////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentCIMOMHandle::referencesClasses(const String &ns, 
		const CIMObjectPath &path, 
		CIMClassResultHandlerIFC &result, 
		const String &resultClass,
		const String &role,
		WBEMFlags:: EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin,
		const StringArray *propertyList)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentCIMOMHandle::close()
{
}

//////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentCIMOMHandle::enumClassNames(const String &ns, 
		const String &className, 
		StringResultHandlerIFC &result, 
		WBEMFlags:: EDeepFlag deep)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}

//////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentCIMOMHandle::enumClass(const String &ns, 
		const String &className, 
		CIMClassResultHandlerIFC &result, 
		WBEMFlags:: EDeepFlag deep,
		WBEMFlags:: ELocalOnlyFlag localOnly,
		WBEMFlags:: EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}

//////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentCIMOMHandle::enumInstances(const String &ns, 
		const String &className, 
		CIMInstanceResultHandlerIFC &result, 
		WBEMFlags:: EDeepFlag deep,
		WBEMFlags:: ELocalOnlyFlag localOnly,
		WBEMFlags:: EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin,
		const StringArray *propertyList)
{
	CppInstanceProviderIFC* pInstProv = getInstanceProvider(ns, className); 
	if (!pInstProv)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	}
	{
		PAReadLock rl(m_locker); 
		CIMClass cc = helperGetClass(ns, className); 
		pInstProv->enumInstances(m_PAEnv,ns ,className ,result ,localOnly ,
				deep ,includeQualifiers ,includeClassOrigin,
				propertyList, cc,cc); 
	}
}

//////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentCIMOMHandle::enumInstanceNames(const String &ns, 
		const String &className, 
		CIMObjectPathResultHandlerIFC &result)
{
	CppInstanceProviderIFC* pInstProv = getInstanceProvider(ns, className); 
	if (!pInstProv)
	{
		OW_THROWCIM(CIMException::NOT_SUPPORTED); 
	}
	{
		PAReadLock rl(m_locker); 
		pInstProv->enumInstanceNames(m_PAEnv,ns ,className ,result , 
									 helperGetClass(ns, className)); 
	}
}

///////////////////////////////////////////////////////////////////////////////
CIMClass 
ProviderAgentCIMOMHandle::getClass(const String &ns, 
		const String &className, 
		WBEMFlags:: ELocalOnlyFlag localOnly,
		WBEMFlags:: EIncludeQualifiersFlag includeQualifiers,
		WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin,
		const StringArray *propertyList)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}

///////////////////////////////////////////////////////////////////////////////
void 
ProviderAgentCIMOMHandle::execQuery(const String &ns, 
		CIMInstanceResultHandlerIFC &result, 
		const String &query, 
		const String &queryLanguage)
{
	OW_THROWCIM(CIMException::NOT_SUPPORTED); 
}

//////////////////////////////////////////////////////////////////////////////
ProviderAgentCIMOMHandle::PAReadLock::PAReadLock(const ProviderAgentLockerIFCRef& locker)
	: m_locker(locker)
{
	m_locker->getReadLock(); 
}
//////////////////////////////////////////////////////////////////////////////
ProviderAgentCIMOMHandle::PAReadLock::~PAReadLock()
{
	m_locker->releaseReadLock(); 
}
//////////////////////////////////////////////////////////////////////////////
ProviderAgentCIMOMHandle::PAWriteLock::PAWriteLock(const ProviderAgentLockerIFCRef& locker)
	: m_locker(locker)
{
	m_locker->getWriteLock(); 
}
//////////////////////////////////////////////////////////////////////////////
ProviderAgentCIMOMHandle::PAWriteLock::~PAWriteLock()
{
	m_locker->releaseWriteLock(); 
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
CppInstanceProviderIFC* 
ProviderAgentCIMOMHandle::getInstanceProvider(const String& ns, 
												const String& className) const
{
	CppInstanceProviderIFC* rval = 0; 
	String key = ns + ":" + className; 
	key.toLowerCase(); 
	Map<String, CppProviderBaseIFCRef>::const_iterator iter = 
			m_instProvs.find(key); 
	if (iter != m_instProvs.end())
	{
		rval = iter->second->getInstanceProvider(); 
		OW_ASSERT(rval != 0); 
		return rval; 
	}
	key = String("") + ":" + className; 
	key.toLowerCase(); 
	iter = m_instProvs.find(key); 
	if (iter != m_instProvs.end())
	{
		rval = iter->second->getInstanceProvider(); 
		OW_ASSERT(rval != 0); 
		return rval; 
	}
	iter = m_instProvs.find("*"); 
	if (iter != m_instProvs.end())
	{
		rval = iter->second->getInstanceProvider(); 
		OW_ASSERT(rval != 0); 
		return rval; 
	}
	return rval; 
}
//////////////////////////////////////////////////////////////////////////////
CppSecondaryInstanceProviderIFC* 
ProviderAgentCIMOMHandle::getSecondaryInstanceProvider(const String& ns, 
												const String& className) const
{
	CppSecondaryInstanceProviderIFC* rval = 0; 
	String key = ns + ":" + className; 
	key.toLowerCase(); 
	Map<String, CppProviderBaseIFCRef>::const_iterator iter = 
			m_secondaryInstProvs.find(key); 
	if (iter != m_secondaryInstProvs.end())
	{
		rval = iter->second->getSecondaryInstanceProvider(); 
		OW_ASSERT(rval != 0); 
		return rval; 
	}
	key = String("") + ":" + className; 
	key.toLowerCase(); 
	iter = m_secondaryInstProvs.find(key); 
	if (iter != m_secondaryInstProvs.end())
	{
		rval = iter->second->getSecondaryInstanceProvider(); 
		OW_ASSERT(rval != 0); 
		return rval; 
	}
	iter = m_secondaryInstProvs.find("*"); 
	if (iter != m_secondaryInstProvs.end())
	{
		rval = iter->second->getSecondaryInstanceProvider(); 
		OW_ASSERT(rval != 0); 
		return rval; 
	}
	return rval; 
}
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////
CppAssociatorProviderIFC* 
ProviderAgentCIMOMHandle::getAssociatorProvider(const String& ns, 
												const String& className) const
{
	CppAssociatorProviderIFC* rval = 0; 
	String key = ns + ":" + className; 
	key.toLowerCase(); 
	Map<String, CppProviderBaseIFCRef>::const_iterator iter = 
			m_assocProvs.find(key); 
	if (iter != m_assocProvs.end())
	{
		rval = iter->second->getAssociatorProvider(); 
		OW_ASSERT(rval != 0); 
		return rval; 
	}
	key = String("") + ":" + className; 
	key.toLowerCase(); 
	iter = m_assocProvs.find(key); 
	if (iter != m_assocProvs.end())
	{
		rval = iter->second->getAssociatorProvider(); 
		OW_ASSERT(rval != 0); 
		return rval; 
	}
	iter = m_assocProvs.find("*"); 
	if (iter != m_assocProvs.end())
	{
		rval = iter->second->getAssociatorProvider(); 
		OW_ASSERT(rval != 0); 
		return rval; 
	}
	return rval; 
}
#endif
//////////////////////////////////////////////////////////////////////////////
CppMethodProviderIFC* 
ProviderAgentCIMOMHandle::getMethodProvider(const String& ns, 
											const String& className, 
											const String& methodName) const
{
	CppMethodProviderIFC* rval = 0; 
	String key = ns + ":" + className + ":" + methodName; 
	key.toLowerCase(); 
	Map<String, CppProviderBaseIFCRef>::const_iterator iter = 
			m_methodProvs.find(key); 
	if (iter != m_methodProvs.end())
	{
		rval = iter->second->getMethodProvider(); 
		OW_ASSERT(rval != 0); 
		return rval; 
	}
	key = String("") + ":" + className + ":" + methodName; 
	key.toLowerCase(); 
	iter = m_methodProvs.find(key); 
	if (iter != m_methodProvs.end())
	{
		rval = iter->second->getMethodProvider(); 
		OW_ASSERT(rval != 0); 
		return rval; 
	}
	iter = m_methodProvs.find("*"); 
	if (iter != m_methodProvs.end())
	{
		rval = iter->second->getMethodProvider(); 
		OW_ASSERT(rval != 0); 
		return rval; 
	}
	return rval; 
}

//////////////////////////////////////////////////////////////////////////////
CIMClass
ProviderAgentCIMOMHandle::helperGetClass(const String& ns, 
										 const String& className) 
{
	CIMClass rval(CIMNULL); 
	String lcn = className; 
	lcn.toLowerCase(); 
	String lns = ns; 
	lns.toLowerCase(); 
	String key = lns + ":" + lcn; 
	rval = m_cimClasses.getFromCache(key); 
	if (rval)
	{
		return rval; 
	}
	rval = m_cimClasses.getFromCache(lcn); 
	if (rval)
	{
		return rval; 
	}
	if (m_classRetrieval == ProviderAgentEnvironment::RETRIEVE_CLASSES)
	{
		CIMOMHandleIFCRef ch = m_PAEnv->getCIMOMHandle(); 
		if (ch)
		{
			rval = ch->getClass(ns, className); 
			if (rval)
			{
				m_cimClasses.addToCache(rval,key); 
			}
		}
	}
	return rval; 
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
} // end namespace OpenWBEM

