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

#ifndef OW_CPPPROXYPROVIDER_HPP_
#define OW_CPPPROXYPROVIDER_HPP_

#include "OW_config.h"
#include "OW_AssociatorProviderIFC.hpp"
#include "OW_InstanceProviderIFC.hpp"
#include "OW_MethodProviderIFC.hpp"
#include "OW_PropertyProviderIFC.hpp"
#include "OW_PolledProviderIFC.hpp"
#include "OW_IndicationExportProviderIFC.hpp"
#include "OW_IndicationProviderIFC.hpp"
#include "OW_CppAssociatorProviderIFC.hpp"
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CppMethodProviderIFC.hpp"
#include "OW_CppPropertyProviderIFC.hpp"
#include "OW_CppPolledProviderIFC.hpp"
#include "OW_CppIndicationExportProviderIFC.hpp"
#include "OW_CppIndicationProviderIFC.hpp"

class OW_CppAssociatorProviderProxy : public OW_AssociatorProviderIFC
{
public:

	OW_CppAssociatorProviderProxy(OW_CppAssociatorProviderIFCRef pProv);
	
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
			const OW_StringArray* propertyList);

	virtual void associatorNames(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMObjectPath& assocName,
			const OW_CIMObjectPath& objectName,
			OW_CIMObjectPathResultHandlerIFC& result,
			const OW_String& resultClass,
			const OW_String& role,
			const OW_String& resultRole);

	virtual void references(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMObjectPath& assocName,
			const OW_CIMObjectPath& objectName,
			OW_CIMInstanceResultHandlerIFC& result,
			const OW_String& role,
			const OW_Bool& includeQualifiers,
			const OW_Bool& includeClassOrigin,
			const OW_StringArray* propertyList);

	virtual void referenceNames(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMObjectPath& assocName,
			const OW_CIMObjectPath& objectName,
			OW_CIMObjectPathResultHandlerIFC& result,
			const OW_String& role);

private:
	OW_CppAssociatorProviderIFCRef m_pProv;
};

class OW_CppInstanceProviderProxy : public OW_InstanceProviderIFC
{
public:
	OW_CppInstanceProviderProxy(OW_CppInstanceProviderIFCRef pProv);

	virtual void deleteInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMObjectPath& cop);

	virtual void enumInstanceNames(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_String& className,
			OW_CIMObjectPathResultHandlerIFC& result,
			const OW_CIMClass& cimClass);

	virtual void enumInstances(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_String& className,
			OW_CIMInstanceResultHandlerIFC& result,
			OW_Bool localOnly, 
			OW_Bool deep, 
			OW_Bool includeQualifiers, 
			OW_Bool includeClassOrigin,
			const OW_StringArray* propertyList,
			const OW_CIMClass& requestedClass,
			const OW_CIMClass& cimClass);

	virtual OW_CIMInstance getInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMObjectPath& instanceName,
			OW_Bool localOnly,
			OW_Bool includeQualifiers, 
			OW_Bool includeClassOrigin,
			const OW_StringArray* propertyList, 
			const OW_CIMClass& cimClass);

	virtual OW_CIMObjectPath createInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMInstance& cimInstance);

	virtual void modifyInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMInstance& modifiedInstance,
			const OW_CIMInstance& previousInstance,
			OW_Bool includeQualifiers,
			const OW_StringArray* propertyList,
			const OW_CIMClass& theClass);

private:
	OW_CppInstanceProviderIFCRef m_pProv;
};

class OW_CppMethodProviderProxy : public OW_MethodProviderIFC
{
public:

	OW_CppMethodProviderProxy(OW_CppMethodProviderIFCRef pProv);

	virtual OW_CIMValue invokeMethod(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMObjectPath& path,
			const OW_String& methodName,
			const OW_CIMParamValueArray& in,
			OW_CIMParamValueArray& out);

private:

	OW_CppMethodProviderIFCRef m_pProv;
};

class OW_CppPropertyProviderProxy : public OW_PropertyProviderIFC
{
public:
	OW_CppPropertyProviderProxy(OW_CppPropertyProviderIFCRef pProv);
	virtual OW_CIMValue getPropertyValue(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMObjectPath& cop,
			const OW_String& originClass,
			const OW_String& propertyName);

	virtual void setPropertyValue(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMObjectPath& cop,
			const OW_String& originClass,
			const OW_String& propertyName, 	
			const OW_CIMValue& val);

private:
	OW_CppPropertyProviderIFCRef m_pProv;
};

class OW_CppIndicationExportProviderProxy : public OW_IndicationExportProviderIFC
{
public:
	OW_CppIndicationExportProviderProxy(OW_CppIndicationExportProviderIFCRef pProv) :
		m_pProv(pProv) {}

	virtual OW_StringArray getHandlerClassNames()
	{
		return m_pProv->getHandlerClassNames();
	}
	
	virtual void exportIndication(const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_CIMInstance& indHandlerInst, const OW_CIMInstance& indicationInst)
	{
		m_pProv->exportIndication(env, ns, indHandlerInst, indicationInst);
	}

private:
	OW_CppIndicationExportProviderIFCRef m_pProv;
};

class OW_CppPolledProviderProxy : public OW_PolledProviderIFC
{
public:
	OW_CppPolledProviderProxy(OW_CppPolledProviderIFCRef pProv) :
		m_pProv(pProv) {}

	virtual OW_Int32 poll(const OW_ProviderEnvironmentIFCRef& env )
			{ return m_pProv->poll(env); }

	virtual OW_Int32 getInitialPollingInterval(const OW_ProviderEnvironmentIFCRef& env )
			{ return m_pProv->getInitialPollingInterval(env); }

private:
	OW_CppPolledProviderIFCRef m_pProv;
};

class OW_CppIndicationProviderProxy : public OW_IndicationProviderIFC
{
public:
	OW_CppIndicationProviderProxy(OW_CppIndicationProviderIFCRef pProv)
		: m_pProv(pProv) {}

	virtual void deActivateFilter(
		const OW_ProviderEnvironmentIFCRef &env, 
		const OW_WQLSelectStatement &filter, 
		const OW_String &eventType, 
		const OW_CIMObjectPath &classPath, 
		bool lastActivation) 
	{
		m_pProv->deActivateFilter(env,filter,eventType,classPath,lastActivation);
	}

	virtual void activateFilter(
		const OW_ProviderEnvironmentIFCRef &env, 
		const OW_WQLSelectStatement &filter, 
		const OW_String &eventType, 
		const OW_CIMObjectPath &classPath, 
		bool firstActivation) 
	{
		m_pProv->activateFilter(env,filter,eventType,classPath,firstActivation);
	}

	virtual void authorizeFilter(
		const OW_ProviderEnvironmentIFCRef &env, 
		const OW_WQLSelectStatement &filter, 
		const OW_String &eventType, 
		const OW_CIMObjectPath &classPath, 
		const OW_String &owner) 
	{
		m_pProv->authorizeFilter(env,filter,eventType,classPath,owner);
	}

	virtual int mustPoll(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_WQLSelectStatement& filter, 
		const OW_String& eventType, 
		const OW_CIMObjectPath& classPath
		)
	{
		return m_pProv->mustPoll(env,filter,eventType,classPath);
	}

private:
	OW_CppIndicationProviderIFCRef m_pProv;
};

#endif

