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
 * @author Dan Nuffer
 */

#ifndef OW_CPPPROXYPROVIDER_HPP_
#define OW_CPPPROXYPROVIDER_HPP_
#include "OW_config.h"
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
#include "OW_AssociatorProviderIFC.hpp"
#endif
#include "OW_InstanceProviderIFC.hpp"
#include "OW_SecondaryInstanceProviderIFC.hpp"
#include "OW_MethodProviderIFC.hpp"
#include "OW_PolledProviderIFC.hpp"
#include "OW_IndicationExportProviderIFC.hpp"
#include "OW_IndicationProviderIFC.hpp"
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
#include "OW_CppAssociatorProviderIFC.hpp"
#endif
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CppSecondaryInstanceProviderIFC.hpp"
#include "OW_CppMethodProviderIFC.hpp"
#include "OW_CppPolledProviderIFC.hpp"
#include "OW_CppIndicationExportProviderIFC.hpp"
#include "OW_CppIndicationProviderIFC.hpp"

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OW_NAMESPACE
{

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

class OW_CPPPROVIFC_API CppAssociatorProviderProxy : public AssociatorProviderIFC
{
public:
	CppAssociatorProviderProxy(CppAssociatorProviderIFCRef pProv);
	
	virtual void associators(
			const ProviderEnvironmentIFCRef& env,
			CIMInstanceResultHandlerIFC& result,
			const String& ns,
			const CIMObjectPath& objectName,
			const String& assocClass,
			const String& resultClass,
			const String& role,
			const String& resultRole,
			WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
			WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
			const StringArray* propertyList);
	virtual void associatorNames(
			const ProviderEnvironmentIFCRef& env,
			CIMObjectPathResultHandlerIFC& result,
			const String& ns,
			const CIMObjectPath& objectName,
			const String& assocClass,
			const String& resultClass,
			const String& role,
			const String& resultRole);
	virtual void references(
			const ProviderEnvironmentIFCRef& env,
			CIMInstanceResultHandlerIFC& result,
			const String& ns,
			const CIMObjectPath& objectName,
			const String& resultClass,
			const String& role,
			WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
			WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
			const StringArray* propertyList);
	virtual void referenceNames(
			const ProviderEnvironmentIFCRef& env,
			CIMObjectPathResultHandlerIFC& result,
			const String& ns,
			const CIMObjectPath& objectName,
			const String& resultClass,
			const String& role);
	virtual void shuttingDown(const ProviderEnvironmentIFCRef& env);
private:
	CppAssociatorProviderIFCRef m_pProv;
};
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

class OW_CPPPROVIFC_API CppInstanceProviderProxy : public InstanceProviderIFC
{
public:
	CppInstanceProviderProxy(CppInstanceProviderIFCRef pProv);
	virtual void enumInstanceNames(
			const ProviderEnvironmentIFCRef& env,
			const String& ns,
			const String& className,
			CIMObjectPathResultHandlerIFC& result,
			const CIMClass& cimClass);
	virtual void enumInstances(
			const ProviderEnvironmentIFCRef& env,
			const String& ns,
			const String& className,
			CIMInstanceResultHandlerIFC& result,
			WBEMFlags::ELocalOnlyFlag localOnly, 
			WBEMFlags::EDeepFlag deep, 
			WBEMFlags::EIncludeQualifiersFlag includeQualifiers, 
			WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
			const StringArray* propertyList,
			const CIMClass& requestedClass,
			const CIMClass& cimClass);
	virtual CIMInstance getInstance(
			const ProviderEnvironmentIFCRef& env,
			const String& ns,
			const CIMObjectPath& instanceName,
			WBEMFlags::ELocalOnlyFlag localOnly,
			WBEMFlags::EIncludeQualifiersFlag includeQualifiers, 
			WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
			const StringArray* propertyList, 
			const CIMClass& cimClass);
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	virtual CIMObjectPath createInstance(
			const ProviderEnvironmentIFCRef& env,
			const String& ns,
			const CIMInstance& cimInstance);
	virtual void modifyInstance(
			const ProviderEnvironmentIFCRef& env,
			const String& ns,
			const CIMInstance& modifiedInstance,
			const CIMInstance& previousInstance,
			WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
			const StringArray* propertyList,
			const CIMClass& theClass);
	virtual void deleteInstance(
			const ProviderEnvironmentIFCRef& env,
			const String& ns,
			const CIMObjectPath& cop);
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
	virtual void shuttingDown(const ProviderEnvironmentIFCRef& env);
private:
	CppInstanceProviderIFCRef m_pProv;
};

class OW_CPPPROVIFC_API CppSecondaryInstanceProviderProxy : public SecondaryInstanceProviderIFC
{
public:
	CppSecondaryInstanceProviderProxy(CppSecondaryInstanceProviderIFCRef pProv);
	virtual void filterInstances(const ProviderEnvironmentIFCRef &env, const String &ns, const String &className, CIMInstanceArray &instances, WBEMFlags:: ELocalOnlyFlag localOnly, WBEMFlags:: EDeepFlag deep, WBEMFlags:: EIncludeQualifiersFlag includeQualifiers, WBEMFlags:: EIncludeClassOriginFlag includeClassOrigin, const StringArray *propertyList, const CIMClass &requestedClass, const CIMClass &cimClass);
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	virtual void createInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMInstance &cimInstance);
	virtual void modifyInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMInstance &modifiedInstance, const CIMInstance &previousInstance, WBEMFlags:: EIncludeQualifiersFlag includeQualifiers, const StringArray *propertyList, const CIMClass &theClass);
	virtual void deleteInstance(const ProviderEnvironmentIFCRef &env, const String &ns, const CIMObjectPath &cop);
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
	virtual void shuttingDown(const ProviderEnvironmentIFCRef& env);
private:
	CppSecondaryInstanceProviderIFCRef m_pProv;
};

class OW_CPPPROVIFC_API CppMethodProviderProxy : public MethodProviderIFC
{
public:
	CppMethodProviderProxy(CppMethodProviderIFCRef pProv);
	virtual CIMValue invokeMethod(
			const ProviderEnvironmentIFCRef& env,
			const String& ns,
			const CIMObjectPath& path,
			const String& methodName,
			const CIMParamValueArray& in,
			CIMParamValueArray& out);
	virtual void shuttingDown(const ProviderEnvironmentIFCRef& env);
private:
	CppMethodProviderIFCRef m_pProv;
};
class OW_CPPPROVIFC_API CppIndicationExportProviderProxy : public IndicationExportProviderIFC
{
public:
	CppIndicationExportProviderProxy(CppIndicationExportProviderIFCRef pProv) :
		m_pProv(pProv) {}
	virtual StringArray getHandlerClassNames()
	{
		return m_pProv->getHandlerClassNames();
	}
	
	virtual void exportIndication(const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const CIMInstance& indHandlerInst, const CIMInstance& indicationInst)
	{
		m_pProv->exportIndication(env, ns, indHandlerInst, indicationInst);
	}

	virtual void doShutdown()
	{
		m_pProv->doShutdown();
	}
	virtual void doCooperativeCancel()
	{
		m_pProv->doCooperativeCancel();
	}
	virtual void doDefinitiveCancel()
	{
		m_pProv->doDefinitiveCancel();
	}
	virtual void shuttingDown(const ProviderEnvironmentIFCRef& env)
	{
		m_pProv->shuttingDown(env);
	}
private:
	CppIndicationExportProviderIFCRef m_pProv;
};
class OW_CPPPROVIFC_API CppPolledProviderProxy : public PolledProviderIFC
{
public:
	CppPolledProviderProxy(CppPolledProviderIFCRef pProv) :
		m_pProv(pProv) {}
	virtual Int32 poll(const ProviderEnvironmentIFCRef& env )
			{ return m_pProv->poll(env); }
	virtual Int32 getInitialPollingInterval(const ProviderEnvironmentIFCRef& env )
			{ return m_pProv->getInitialPollingInterval(env); }
	virtual void doShutdown()
	{
		m_pProv->doShutdown();
	}
	virtual void doCooperativeCancel()
	{
		m_pProv->doCooperativeCancel();
	}
	virtual void doDefinitiveCancel()
	{
		m_pProv->doDefinitiveCancel();
	}
	virtual void shuttingDown(const ProviderEnvironmentIFCRef& env)
	{
		m_pProv->shuttingDown(env);
	}
private:
	CppPolledProviderIFCRef m_pProv;
};
class OW_CPPPROVIFC_API CppIndicationProviderProxy : public IndicationProviderIFC
{
public:
	CppIndicationProviderProxy(CppIndicationProviderIFCRef pProv)
		: m_pProv(pProv) {}
	virtual void deActivateFilter(
		const ProviderEnvironmentIFCRef &env, 
		const WQLSelectStatement &filter, 
		const String &eventType, 
		const String& nameSpace,
		const StringArray& classes,
		bool lastActivation)
	{
		m_pProv->deActivateFilter(env,filter,eventType,nameSpace, classes,lastActivation);
	}
	virtual void activateFilter(
		const ProviderEnvironmentIFCRef &env, 
		const WQLSelectStatement &filter, 
		const String &eventType, 
		const String& nameSpace,
		const StringArray& classes,
		bool firstActivation)
	{
		m_pProv->activateFilter(env,filter,eventType,nameSpace,classes,firstActivation);
	}
	virtual void authorizeFilter(
		const ProviderEnvironmentIFCRef &env, 
		const WQLSelectStatement &filter, 
		const String &eventType, 
		const String& nameSpace,
		const StringArray& classes, 
		const String &owner) 
	{
		m_pProv->authorizeFilter(env,filter,eventType,nameSpace,classes,owner);
	}
	virtual int mustPoll(
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter, 
		const String& eventType, 
		const String& nameSpace,
		const StringArray& classes
		)
	{
		return m_pProv->mustPoll(env,filter,eventType,nameSpace,classes);
	}
	virtual void shuttingDown(const ProviderEnvironmentIFCRef& env)
	{
		m_pProv->shuttingDown(env);
	}
private:
	CppIndicationProviderIFCRef m_pProv;
	
	// note this doesn't need to be mutex protected because the indication server always [de]activates subscriptions serially.
};

} // end namespace OW_NAMESPACE

#endif
