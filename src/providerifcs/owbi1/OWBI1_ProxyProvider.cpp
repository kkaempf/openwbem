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

#include "OW_config.h"
#include "OWBI1_ProxyProvider.hpp"
#include "OW_WBEMFlags.hpp"
#include "OW_CIMValue.hpp"
#include "OWBI1_CIMValue.hpp"
#include "OW_CIMClass.hpp"
#include "OWBI1_CIMClass.hpp"
#include "OW_CIMInstance.hpp"
#include "OWBI1_CIMInstance.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OWBI1_CIMObjectPath.hpp"
#include "OW_CIMParamValue.hpp"
#include "OWBI1_CIMParamValue.hpp"
#include "OW_ProviderEnvironmentIFC.hpp"
#include "OWBI1_ProviderEnvironmentIFC.hpp"

namespace OWBI1
{

// TODO:
// remove includeQualifiers
// remove includeClassOrigin
// create class PropertyList and use that.
//   remove localOnly & deep and have them modify the PropertyList
// convert all OWBI1::CIMException to OW::CIMException
// convert all OWBI1::Exception to OW::OWBI1Exception

using namespace WBEMFlags;
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////		
BI1AssociatorProviderProxy::BI1AssociatorProviderProxy(
	const BI1AssociatorProviderIFCRef& pProv) :
	AssociatorProviderIFC(), m_pProv(pProv)
{
}

//////////////////////////////////////////////////////////////////////////////		
void
BI1AssociatorProviderProxy::associators(
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
		const OpenWBEM::StringArray* propertyList)
{
	m_pProv->updateAccessTime();
	CIMInstanceResultHandlerWrapper cirh(result);
	// TODO: handle includeQualifiers & includeClassOrigin
	m_pProv->associators(createProvEnvWrapper(env), cirh, ns.c_str(), convertCIMObjectPath(objectName), assocClass.c_str(), resultClass.c_str(), role.c_str(),
		resultRole.c_str(), convertPropertyList(propertyList));
}
//////////////////////////////////////////////////////////////////////////////		
void
BI1AssociatorProviderProxy::associatorNames(
		const OpenWBEM::ProviderEnvironmentIFCRef& env,
		OpenWBEM::CIMObjectPathResultHandlerIFC& result,
		const OpenWBEM::String& ns,
		const OpenWBEM::CIMObjectPath& objectName,
		const OpenWBEM::String& assocClass,
		const OpenWBEM::String& resultClass,
		const OpenWBEM::String& role,
		const OpenWBEM::String& resultRole)
{
	m_pProv->updateAccessTime();
	CIMObjectPathResultHandlerWrapper coprh(result);
	m_pProv->associatorNames(createProvEnvWrapper(env), coprh, ns.c_str(), convertCIMObjectPath(objectName), assocClass.c_str(), resultClass.c_str(),
			role.c_str(), resultRole.c_str());
}
//////////////////////////////////////////////////////////////////////////////		
void
BI1AssociatorProviderProxy::references(
		const OpenWBEM::ProviderEnvironmentIFCRef& env,
		OpenWBEM::CIMInstanceResultHandlerIFC& result,
		const OpenWBEM::String& ns,
		const OpenWBEM::CIMObjectPath& objectName,
		const OpenWBEM::String& resultClass,
		const OpenWBEM::String& role,
		OpenWBEM::WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		OpenWBEM::WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const OpenWBEM::StringArray* propertyList)
{
	m_pProv->updateAccessTime();
	CIMInstanceResultHandlerWrapper cirh(result);
	// TODO: handle includeQualifiers & includeClassOrigin
	m_pProv->references(createProvEnvWrapper(env), cirh, ns.c_str(), convertCIMObjectPath(objectName), resultClass.c_str(), role.c_str(),
			convertPropertyList(propertyList));
}
//////////////////////////////////////////////////////////////////////////////		
void
BI1AssociatorProviderProxy::referenceNames(
		const OpenWBEM::ProviderEnvironmentIFCRef& env,
		OpenWBEM::CIMObjectPathResultHandlerIFC& result,
		const OpenWBEM::String& ns,
		const OpenWBEM::CIMObjectPath& objectName,
		const OpenWBEM::String& resultClass,
		const OpenWBEM::String& role)
{
	m_pProv->updateAccessTime();
	CIMObjectPathResultHandlerWrapper coprh(result);
	m_pProv->referenceNames(createProvEnvWrapper(env), coprh, ns.c_str(), convertCIMObjectPath(objectName), resultClass.c_str(), role.c_str());
}
#endif // #ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
//////////////////////////////////////////////////////////////////////////////		
BI1InstanceProviderProxy::BI1InstanceProviderProxy(
		const BI1InstanceProviderIFCRef& pProv) :
	InstanceProviderIFC(), m_pProv(pProv)
{
}
//////////////////////////////////////////////////////////////////////////////		
void
BI1InstanceProviderProxy::enumInstanceNames(
		const OpenWBEM::ProviderEnvironmentIFCRef& env,
		const OpenWBEM::String& ns,
		const OpenWBEM::String& className,
		OpenWBEM::CIMObjectPathResultHandlerIFC& result,
		const OpenWBEM::CIMClass& cimClass)
{
	m_pProv->updateAccessTime();
	CIMObjectPathResultHandlerWrapper coprh(result);
	return m_pProv->enumInstanceNames(createProvEnvWrapper(env), ns.c_str(), className.c_str(), coprh, convertCIMClass(cimClass));
}
//////////////////////////////////////////////////////////////////////////////		
void
BI1InstanceProviderProxy::enumInstances(
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
		const OpenWBEM::CIMClass& cimClass)
{
	m_pProv->updateAccessTime();
	CIMInstanceResultHandlerWrapper cirh(result);
	// TODO: handle includeQualifiers & includeClassOrigin & localOnly & deep
	m_pProv->enumInstances(createProvEnvWrapper(env), ns.c_str(), className.c_str(), cirh,
		convertPropertyList(propertyList), convertCIMClass(requestedClass), 
		convertCIMClass(cimClass));
}
//////////////////////////////////////////////////////////////////////////////		
OpenWBEM::CIMInstance
BI1InstanceProviderProxy::getInstance(
		const OpenWBEM::ProviderEnvironmentIFCRef& env,
		const OpenWBEM::String& ns,
		const OpenWBEM::CIMObjectPath& instanceName,
		OpenWBEM::WBEMFlags::ELocalOnlyFlag localOnly,
		OpenWBEM::WBEMFlags::EIncludeQualifiersFlag includeQualifiers, 
		OpenWBEM::WBEMFlags::EIncludeClassOriginFlag includeClassOrigin,
		const OpenWBEM::StringArray* propertyList, 
		const OpenWBEM::CIMClass& cimClass)
{
	m_pProv->updateAccessTime();
	// TODO: handle localOnly, includeQualifiers & includeClassOrigin
	return convertCIMInstance(m_pProv->getInstance(createProvEnvWrapper(env), ns.c_str(), convertCIMObjectPath(instanceName),
		convertPropertyList(propertyList), convertCIMClass(cimClass)));
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////		
OpenWBEM::CIMObjectPath
BI1InstanceProviderProxy::createInstance(
		const OpenWBEM::ProviderEnvironmentIFCRef& env,
		const OpenWBEM::String& ns,
		const OpenWBEM::CIMInstance& cimInstance)
{
	m_pProv->updateAccessTime();
	return convertCIMObjectPath(m_pProv->createInstance(createProvEnvWrapper(env), ns.c_str(), convertCIMInstance(cimInstance)));
}
//////////////////////////////////////////////////////////////////////////////		
void
BI1InstanceProviderProxy::modifyInstance(
		const OpenWBEM::ProviderEnvironmentIFCRef& env,
		const OpenWBEM::String& ns,
		const OpenWBEM::CIMInstance& modifiedInstance,
		const OpenWBEM::CIMInstance& previousInstance,
		OpenWBEM::WBEMFlags::EIncludeQualifiersFlag includeQualifiers,
		const OpenWBEM::StringArray* propertyList,
		const OpenWBEM::CIMClass& theClass)
{
	m_pProv->updateAccessTime();
	m_pProv->modifyInstance(createProvEnvWrapper(env), ns.c_str(), convertCIMInstance(modifiedInstance), convertCIMInstance(previousInstance), 
		convertPropertyList(propertyList), convertCIMClass(theClass));
}
//////////////////////////////////////////////////////////////////////////////		
void
BI1InstanceProviderProxy::deleteInstance(
		const OpenWBEM::ProviderEnvironmentIFCRef& env,
		const OpenWBEM::String& ns,
		const OpenWBEM::CIMObjectPath& cop)
{
	m_pProv->updateAccessTime();
	m_pProv->deleteInstance(createProvEnvWrapper(env), ns.c_str(), convertCIMObjectPath(cop));
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

//////////////////////////////////////////////////////////////////////////////		
BI1SecondaryInstanceProviderProxy::BI1SecondaryInstanceProviderProxy(const BI1SecondaryInstanceProviderIFCRef& pProv)
	: SecondaryInstanceProviderIFC()
	, m_pProv(pProv)
{
}
//////////////////////////////////////////////////////////////////////////////		
void
BI1SecondaryInstanceProviderProxy::filterInstances(
	const OpenWBEM::ProviderEnvironmentIFCRef &env, 
	const OpenWBEM::String &ns, 
	const OpenWBEM::String &className, 
	OpenWBEM::CIMInstanceArray &instances, 
	OpenWBEM::WBEMFlags::ELocalOnlyFlag localOnly, 
	OpenWBEM::WBEMFlags::EDeepFlag deep, 
	OpenWBEM::WBEMFlags::EIncludeQualifiersFlag includeQualifiers, 
	OpenWBEM::WBEMFlags::EIncludeClassOriginFlag includeClassOrigin, 
	const OpenWBEM::StringArray *propertyList, 
	const OpenWBEM::CIMClass &requestedClass, 
	const OpenWBEM::CIMClass &cimClass)
{
	m_pProv->updateAccessTime();
	OWBI1::CIMInstanceArray owbi1Instances(convertCIMInstances(instances));
	// TODO: handle localOnly & deep by modifying the propertyList. Handle includeQualifiers & includeClassOrigin
	m_pProv->filterInstances(createProvEnvWrapper(env), ns.c_str(), className.c_str(), owbi1Instances, 
		convertPropertyList(propertyList), convertCIMClass(requestedClass), convertCIMClass(cimClass));
	instances = convertCIMInstances(owbi1Instances);
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
//////////////////////////////////////////////////////////////////////////////		
void
BI1SecondaryInstanceProviderProxy::createInstance(const OpenWBEM::ProviderEnvironmentIFCRef &env, const OpenWBEM::String &ns, const OpenWBEM::CIMInstance &cimInstance)
{
	m_pProv->updateAccessTime();
	m_pProv->createInstance(createProvEnvWrapper(env), ns.c_str(), convertCIMInstance(cimInstance));
}
//////////////////////////////////////////////////////////////////////////////		
void
BI1SecondaryInstanceProviderProxy::modifyInstance(
	const OpenWBEM::ProviderEnvironmentIFCRef &env, 
	const OpenWBEM::String &ns, 
	const OpenWBEM::CIMInstance &modifiedInstance, 
	const OpenWBEM::CIMInstance &previousInstance, 
	OpenWBEM::WBEMFlags::EIncludeQualifiersFlag includeQualifiers, 
	const OpenWBEM::StringArray *propertyList, 
	const OpenWBEM::CIMClass &theClass)
{
	m_pProv->updateAccessTime();
	m_pProv->modifyInstance(createProvEnvWrapper(env), ns.c_str(), convertCIMInstance(modifiedInstance), convertCIMInstance(previousInstance), 
		convertPropertyList(propertyList), convertCIMClass(theClass));
}
//////////////////////////////////////////////////////////////////////////////		
void
BI1SecondaryInstanceProviderProxy::deleteInstance(const OpenWBEM::ProviderEnvironmentIFCRef &env, const OpenWBEM::String &ns, const OpenWBEM::CIMObjectPath &cop)
{
	m_pProv->updateAccessTime();
	m_pProv->deleteInstance(createProvEnvWrapper(env), ns.c_str(), convertCIMObjectPath(cop));
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

//////////////////////////////////////////////////////////////////////////////		
BI1MethodProviderProxy::BI1MethodProviderProxy(const BI1MethodProviderIFCRef& pProv) :
	MethodProviderIFC(), m_pProv(pProv)
{
}
//////////////////////////////////////////////////////////////////////////////		
OpenWBEM::CIMValue
BI1MethodProviderProxy::invokeMethod(
		const OpenWBEM::ProviderEnvironmentIFCRef& env,
		const OpenWBEM::String& ns,
		const OpenWBEM::CIMObjectPath& path,
		const OpenWBEM::String& methodName,
		const OpenWBEM::CIMParamValueArray& in,
		OpenWBEM::CIMParamValueArray& out)
{
	m_pProv->updateAccessTime();
	OWBI1::CIMParamValueArray owbi1Out(convertCIMParamValues(out));
	CIMValue rval = m_pProv->invokeMethod(createProvEnvWrapper(env), ns.c_str(), convertCIMObjectPath(path), methodName.c_str(), convertCIMParamValues(in), owbi1Out);
	out = convertCIMParamValues(owbi1Out);
	return convertCIMValue(rval);
}

BI1IndicationExportProviderProxy::BI1IndicationExportProviderProxy(const BI1IndicationExportProviderIFCRef& pProv) :
	m_pProv(pProv) {}
BI1IndicationExportProviderProxy::~BI1IndicationExportProviderProxy()
{
}
OpenWBEM::StringArray BI1IndicationExportProviderProxy::getHandlerClassNames()
{
	return convertStringArray(m_pProv->getHandlerClassNames());
}

void BI1IndicationExportProviderProxy::exportIndication(const OpenWBEM::ProviderEnvironmentIFCRef& env,
	const OpenWBEM::String& ns,
	const OpenWBEM::CIMInstance& indHandlerInst, const OpenWBEM::CIMInstance& indicationInst)
{
	m_pProv->exportIndication(createProvEnvWrapper(env), ns.c_str(), convertCIMInstance(indHandlerInst), convertCIMInstance(indicationInst));
}

void BI1IndicationExportProviderProxy::doCooperativeCancel()
{
	m_pProv->doCooperativeCancel();
}
void BI1IndicationExportProviderProxy::doDefinitiveCancel()
{
	m_pProv->doDefinitiveCancel();
}

BI1PolledProviderProxy::BI1PolledProviderProxy(const BI1PolledProviderIFCRef& pProv) :
	m_pProv(pProv) {}
BI1PolledProviderProxy::~BI1PolledProviderProxy()
{
}
OpenWBEM::Int32 BI1PolledProviderProxy::poll(const OpenWBEM::ProviderEnvironmentIFCRef& env )
		{ return m_pProv->poll(createProvEnvWrapper(env)); }
OpenWBEM::Int32 BI1PolledProviderProxy::getInitialPollingInterval(const OpenWBEM::ProviderEnvironmentIFCRef& env )
		{ return m_pProv->getInitialPollingInterval(createProvEnvWrapper(env)); }
void BI1PolledProviderProxy::doCooperativeCancel()
{
	m_pProv->doCooperativeCancel();
}
void BI1PolledProviderProxy::doDefinitiveCancel()
{
	m_pProv->doDefinitiveCancel();
}

BI1IndicationProviderProxy::BI1IndicationProviderProxy(const BI1IndicationProviderIFCRef& pProv)
	: m_pProv(pProv)
	, m_activationCount(0) {}
BI1IndicationProviderProxy::~BI1IndicationProviderProxy()
{
}
void BI1IndicationProviderProxy::deActivateFilter(
	const OpenWBEM::ProviderEnvironmentIFCRef &env, 
	const OpenWBEM::WQLSelectStatement &filter, 
	const OpenWBEM::String& eventType, 
	const OpenWBEM::String& nameSpace,
	const OpenWBEM::StringArray& classes) 
{
	bool lastActivation = (--m_activationCount == 0);
	m_pProv->deActivateFilter(createProvEnvWrapper(env),convertWQLSelectStatement(filter),eventType.c_str(),nameSpace.c_str(), convertStringArray(classes),lastActivation);
}
void BI1IndicationProviderProxy::activateFilter(
	const OpenWBEM::ProviderEnvironmentIFCRef &env, 
	const OpenWBEM::WQLSelectStatement &filter, 
	const OpenWBEM::String& eventType, 
	const OpenWBEM::String& nameSpace,
	const OpenWBEM::StringArray& classes) 
{
	bool firstActivation = (m_activationCount++ == 0);
	m_pProv->activateFilter(createProvEnvWrapper(env),convertWQLSelectStatement(filter),eventType.c_str(),nameSpace.c_str(),convertStringArray(classes),firstActivation);
}
void BI1IndicationProviderProxy::authorizeFilter(
	const OpenWBEM::ProviderEnvironmentIFCRef &env, 
	const OpenWBEM::WQLSelectStatement &filter, 
	const OpenWBEM::String& eventType, 
	const OpenWBEM::String& nameSpace,
	const OpenWBEM::StringArray& classes, 
	const OpenWBEM::String &owner) 
{
	m_pProv->authorizeFilter(createProvEnvWrapper(env),convertWQLSelectStatement(filter),eventType.c_str(),nameSpace.c_str(),convertStringArray(classes),owner.c_str());
}
int BI1IndicationProviderProxy::mustPoll(
	const OpenWBEM::ProviderEnvironmentIFCRef& env,
	const OpenWBEM::WQLSelectStatement& filter, 
	const OpenWBEM::String& eventType, 
	const OpenWBEM::String& nameSpace,
	const OpenWBEM::StringArray& classes
	)
{
	return m_pProv->mustPoll(createProvEnvWrapper(env),convertWQLSelectStatement(filter),eventType.c_str(),nameSpace.c_str(),convertStringArray(classes));
}

} // end namespace OWBI1

