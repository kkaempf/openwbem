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

#ifndef OWBI1_PROXYPROVIDER_HPP_INCLUDE_GUARD_
#define OWBI1_PROXYPROVIDER_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OWBI1_config.h"
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
#include "OW_AssociatorProviderIFC.hpp"
#endif
#include "OW_InstanceProviderIFC.hpp"
#include "OW_SecondaryInstanceProviderIFC.hpp"
#include "OW_MethodProviderIFC.hpp"
#include "OW_PolledProviderIFC.hpp"
#include "OW_IndicationExportProviderIFC.hpp"
#include "OW_IndicationProviderIFC.hpp"

#include "OWBI1_AssociatorProviderIFC.hpp"
#include "OWBI1_InstanceProviderIFC.hpp"
#include "OWBI1_SecondaryInstanceProviderIFC.hpp"
#include "OWBI1_MethodProviderIFC.hpp"
#include "OWBI1_PolledProviderIFC.hpp"
#include "OWBI1_IndicationExportProviderIFC.hpp"
#include "OWBI1_IndicationProviderIFC.hpp"

#include "OWBI1_ProxyProviderDetail.hpp"

#include "OW_SharedLibraryReference.hpp"
/*
#include "OW_TempFileStream.hpp"
#include "OW_BinarySerialization.hpp"
#include "OWBI1_CIMInstance.hpp"
#include "OWBI1_WQLSelectStatement.hpp"
#include "OW_ResultHandlerIFC.hpp"
#include "OWBI1_ResultHandlerIFC.hpp"
*/

// The classes and functions defined in this file are not meant for general
// use, they are internal implementation details.  They may change at any time.

namespace OWBI1
{

typedef OpenWBEM::SharedLibraryReference< IntrusiveReference<BI1AssociatorProviderIFC> > BI1AssociatorProviderIFCRef;
typedef OpenWBEM::SharedLibraryReference< IntrusiveReference<BI1InstanceProviderIFC> > BI1InstanceProviderIFCRef;
typedef OpenWBEM::SharedLibraryReference< IntrusiveReference<BI1SecondaryInstanceProviderIFC> > BI1SecondaryInstanceProviderIFCRef;
typedef OpenWBEM::SharedLibraryReference< IntrusiveReference<BI1MethodProviderIFC> > BI1MethodProviderIFCRef;
typedef OpenWBEM::SharedLibraryReference< IntrusiveReference<BI1PolledProviderIFC> > BI1PolledProviderIFCRef;
typedef OpenWBEM::SharedLibraryReference< IntrusiveReference<BI1IndicationProviderIFC> > BI1IndicationProviderIFCRef;
typedef OpenWBEM::SharedLibraryReference< IntrusiveReference<BI1IndicationExportProviderIFC> > BI1IndicationExportProviderIFCRef;
typedef OpenWBEM::SharedLibraryReference< IntrusiveReference<BI1IndicationProviderIFC> > BI1IndicationProviderIFCRef;

using namespace ProxyProviderDetail;

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

class OWBI1_OWBI1PROVIFC_API BI1AssociatorProviderProxy : public OpenWBEM::AssociatorProviderIFC
{
public:
	BI1AssociatorProviderProxy(const BI1AssociatorProviderIFCRef& pProv);
	
	virtual void associators(
			const OpenWBEM::ProviderEnvironmentIFCRef& env,
			OpenWBEM::CIMInstanceResultHandlerIFC& result,
			const OpenWBEM::String& ns,
			const OpenWBEM::CIMObjectPath& objectName,
			const OpenWBEM::String& assocClass,
			const OpenWBEM::String& resultClass,
			const OpenWBEM::String& role,
			const OpenWBEM::String& resultRole,
			OpenWBEM::WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
			OpenWBEM::WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
			const OpenWBEM::StringArray* propertyList);
	virtual void associatorNames(
			const OpenWBEM::ProviderEnvironmentIFCRef& env,
			OpenWBEM::CIMObjectPathResultHandlerIFC& result,
			const OpenWBEM::String& ns,
			const OpenWBEM::CIMObjectPath& objectName,
			const OpenWBEM::String& assocClass,
			const OpenWBEM::String& resultClass,
			const OpenWBEM::String& role,
			const OpenWBEM::String& resultRole);
	virtual void references(
			const OpenWBEM::ProviderEnvironmentIFCRef& env,
			OpenWBEM::CIMInstanceResultHandlerIFC& result,
			const OpenWBEM::String& ns,
			const OpenWBEM::CIMObjectPath& objectName,
			const OpenWBEM::String& resultClass,
			const OpenWBEM::String& role,
			OpenWBEM::WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
			OpenWBEM::WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
			const OpenWBEM::StringArray* propertyList);
	virtual void referenceNames(
			const OpenWBEM::ProviderEnvironmentIFCRef& env,
			OpenWBEM::CIMObjectPathResultHandlerIFC& result,
			const OpenWBEM::String& ns,
			const OpenWBEM::CIMObjectPath& objectName,
			const OpenWBEM::String& resultClass,
			const OpenWBEM::String& role);
private:
	BI1AssociatorProviderIFCRef m_pProv;
};
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL

class OWBI1_OWBI1PROVIFC_API BI1InstanceProviderProxy : public OpenWBEM::InstanceProviderIFC
{
public:
	BI1InstanceProviderProxy(const BI1InstanceProviderIFCRef& pProv);
	virtual void enumInstanceNames(
			const OpenWBEM::ProviderEnvironmentIFCRef& env,
			const OpenWBEM::String& ns,
			const OpenWBEM::String& className,
			OpenWBEM::CIMObjectPathResultHandlerIFC& result,
			const OpenWBEM::CIMClass& cimClass);
	virtual void enumInstances(
			const OpenWBEM::ProviderEnvironmentIFCRef& env,
			const OpenWBEM::String& ns,
			const OpenWBEM::String& className,
			OpenWBEM::CIMInstanceResultHandlerIFC& result,
			OpenWBEM::WBEMFlags::ELocalOnlyFlag localOnly, 
			OpenWBEM::WBEMFlags::EDeepFlag deep, 
			OpenWBEM::WBEMFlags::EIncludeQualifiersFlag includeQualifiers, 
			OpenWBEM::WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
			const OpenWBEM::StringArray* propertyList,
			const OpenWBEM::CIMClass& requestedClass,
			const OpenWBEM::CIMClass& cimClass);
	virtual OpenWBEM::CIMInstance getInstance(
			const OpenWBEM::ProviderEnvironmentIFCRef& env,
			const OpenWBEM::String& ns,
			const OpenWBEM::CIMObjectPath& instanceName,
			OpenWBEM::WBEMFlags::ELocalOnlyFlag localOnly,
			OpenWBEM::WBEMFlags::EIncludeQualifiersFlag includeQualifiers, 
			OpenWBEM::WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
			const OpenWBEM::StringArray* propertyList, 
			const OpenWBEM::CIMClass& cimClass);
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	virtual OpenWBEM::CIMObjectPath createInstance(
			const OpenWBEM::ProviderEnvironmentIFCRef& env,
			const OpenWBEM::String& ns,
			const OpenWBEM::CIMInstance& cimInstance);
	virtual void modifyInstance(
			const OpenWBEM::ProviderEnvironmentIFCRef& env,
			const OpenWBEM::String& ns,
			const OpenWBEM::CIMInstance& modifiedInstance,
			const OpenWBEM::CIMInstance& previousInstance,
			OpenWBEM::WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
			const OpenWBEM::StringArray* propertyList,
			const OpenWBEM::CIMClass& theClass);
	virtual void deleteInstance(
			const OpenWBEM::ProviderEnvironmentIFCRef& env,
			const OpenWBEM::String& ns,
			const OpenWBEM::CIMObjectPath& cop);
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
private:
	BI1InstanceProviderIFCRef m_pProv;
};

class OWBI1_OWBI1PROVIFC_API BI1SecondaryInstanceProviderProxy : public OpenWBEM::SecondaryInstanceProviderIFC
{
public:
	BI1SecondaryInstanceProviderProxy(const BI1SecondaryInstanceProviderIFCRef& pProv);
	virtual void filterInstances(const OpenWBEM::ProviderEnvironmentIFCRef &env, const OpenWBEM::String &ns, const OpenWBEM::String &className, OpenWBEM::CIMInstanceArray &instances, OpenWBEM::WBEMFlags::ELocalOnlyFlag localOnly, OpenWBEM::WBEMFlags::EDeepFlag deep, OpenWBEM::WBEMFlags::EIncludeQualifiersFlag includeQualifiers, OpenWBEM::WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, const OpenWBEM::StringArray *propertyList, const OpenWBEM::CIMClass &requestedClass, const OpenWBEM::CIMClass &cimClass);
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
	virtual void createInstance(const OpenWBEM::ProviderEnvironmentIFCRef &env, const OpenWBEM::String &ns, const OpenWBEM::CIMInstance &cimInstance);
	virtual void modifyInstance(const OpenWBEM::ProviderEnvironmentIFCRef &env, const OpenWBEM::String &ns, const OpenWBEM::CIMInstance &modifiedInstance, const OpenWBEM::CIMInstance &previousInstance, OpenWBEM::WBEMFlags:: EIncludeQualifiersFlag includeQualifiers, const OpenWBEM::StringArray *propertyList, const OpenWBEM::CIMClass &theClass);
	virtual void deleteInstance(const OpenWBEM::ProviderEnvironmentIFCRef &env, const OpenWBEM::String &ns, const OpenWBEM::CIMObjectPath &cop);
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION
private:
	BI1SecondaryInstanceProviderIFCRef m_pProv;
};

class OWBI1_OWBI1PROVIFC_API BI1MethodProviderProxy : public OpenWBEM::MethodProviderIFC
{
public:
	BI1MethodProviderProxy(const BI1MethodProviderIFCRef& pProv);
	virtual OpenWBEM::CIMValue invokeMethod(
			const OpenWBEM::ProviderEnvironmentIFCRef& env,
			const OpenWBEM::String& ns,
			const OpenWBEM::CIMObjectPath& path,
			const OpenWBEM::String& methodName,
			const OpenWBEM::CIMParamValueArray& in,
			OpenWBEM::CIMParamValueArray& out);
private:
	BI1MethodProviderIFCRef m_pProv;
};
class OWBI1_OWBI1PROVIFC_API BI1IndicationExportProviderProxy : public OpenWBEM::IndicationExportProviderIFC
{
public:
	BI1IndicationExportProviderProxy(const BI1IndicationExportProviderIFCRef& pProv);
	~BI1IndicationExportProviderProxy();
	virtual OpenWBEM::StringArray getHandlerClassNames();
	virtual void exportIndication(const OpenWBEM::ProviderEnvironmentIFCRef& env,
		const OpenWBEM::String& ns,
		const OpenWBEM::CIMInstance& indHandlerInst, const OpenWBEM::CIMInstance& indicationInst);
	virtual void doCooperativeCancel();
	virtual void doDefinitiveCancel();
private:
	BI1IndicationExportProviderIFCRef m_pProv;
};
class OWBI1_OWBI1PROVIFC_API BI1PolledProviderProxy : public OpenWBEM::PolledProviderIFC
{
public:
	BI1PolledProviderProxy(const BI1PolledProviderIFCRef& pProv);
	~BI1PolledProviderProxy();
	virtual OpenWBEM::Int32 poll(const OpenWBEM::ProviderEnvironmentIFCRef& env );
	virtual OpenWBEM::Int32 getInitialPollingInterval(const OpenWBEM::ProviderEnvironmentIFCRef& env );
	virtual void doCooperativeCancel();
	virtual void doDefinitiveCancel();
private:
	BI1PolledProviderIFCRef m_pProv;
};
class OWBI1_OWBI1PROVIFC_API BI1IndicationProviderProxy : public OpenWBEM::IndicationProviderIFC
{
public:
	BI1IndicationProviderProxy(const BI1IndicationProviderIFCRef& pProv);
	~BI1IndicationProviderProxy();
	virtual void deActivateFilter(
		const OpenWBEM::ProviderEnvironmentIFCRef &env, 
		const OpenWBEM::WQLSelectStatement &filter, 
		const OpenWBEM::String& eventType, 
		const OpenWBEM::String& nameSpace,
		const OpenWBEM::StringArray& classes);
	virtual void activateFilter(
		const OpenWBEM::ProviderEnvironmentIFCRef &env, 
		const OpenWBEM::WQLSelectStatement &filter, 
		const OpenWBEM::String& eventType, 
		const OpenWBEM::String& nameSpace,
		const OpenWBEM::StringArray& classes);
	virtual void authorizeFilter(
		const OpenWBEM::ProviderEnvironmentIFCRef &env, 
		const OpenWBEM::WQLSelectStatement &filter, 
		const OpenWBEM::String& eventType, 
		const OpenWBEM::String& nameSpace,
		const OpenWBEM::StringArray& classes, 
		const OpenWBEM::String &owner);
	virtual int mustPoll(
		const OpenWBEM::ProviderEnvironmentIFCRef& env,
		const OpenWBEM::WQLSelectStatement& filter, 
		const OpenWBEM::String& eventType, 
		const OpenWBEM::String& nameSpace,
		const OpenWBEM::StringArray& classes
		);
private:
	BI1IndicationProviderIFCRef m_pProv;
	unsigned int m_activationCount;
};

} // end namespace OWBI1

#endif
