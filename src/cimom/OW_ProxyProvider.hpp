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

#ifndef __OW_PROXYPROVIDER_HPP__
#define __OW_PROXYPROVIDER_HPP__

#include "OW_config.h"
#include "OW_AssociatorProviderIFC.hpp"
#include "OW_InstanceProviderIFC.hpp"
#include "OW_MethodProviderIFC.hpp"
#include "OW_PropertyProviderIFC.hpp"
#include "OW_PolledProviderIFC.hpp"
#include "OW_IndicationExportProviderIFC.hpp"
#include "OW_CppAssociatorProviderIFC.hpp"
#include "OW_CppInstanceProviderIFC.hpp"
#include "OW_CppMethodProviderIFC.hpp"
#include "OW_CppPropertyProviderIFC.hpp"
#include "OW_CppPolledProviderIFC.hpp"
#include "OW_CppIndicationExportProviderIFC.hpp"
#include "OW_CIMValue.hpp"

class OW_AssociatorProviderProxy : public OW_AssociatorProviderIFC
{
public:

	OW_AssociatorProviderProxy(OW_CppAssociatorProviderIFCRef pProv)
	: OW_AssociatorProviderIFC(), m_pProv(pProv)
	{}
	
	virtual void associators(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& assocName,
			const OW_CIMObjectPath& objectName,
			OW_CIMInstanceResultHandlerIFC& result,
			const OW_String& resultClass,
			const OW_String& role,
			const OW_String& resultRole,
			const OW_Bool& includeQualifiers,
			const OW_Bool& includeClassOrigin,
			const OW_StringArray* propertyList)
	{
		m_pProv->associators(env,assocName,objectName,result,resultClass,role,
			resultRole,includeQualifiers,includeClassOrigin,propertyList);
	}

	virtual void associatorNames(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& assocName,
			const OW_CIMObjectPath& objectName,
			OW_CIMObjectPathResultHandlerIFC& result,
			const OW_String& resultClass,
			const OW_String& role,
			const OW_String& resultRole)
	{
		m_pProv->associatorNames(env,assocName,objectName,result,resultClass,
			role,resultRole);
	}

	virtual void references(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& assocName,
			const OW_CIMObjectPath& objectName,
			OW_CIMInstanceResultHandlerIFC& result,
			const OW_String& role,
			const OW_Bool& includeQualifiers,
			const OW_Bool& includeClassOrigin,
			const OW_StringArray* propertyList)
	{
		m_pProv->references(env,assocName,objectName,result,role,
			includeQualifiers,includeClassOrigin,propertyList);
	}

	virtual void referenceNames(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& assocName,
			const OW_CIMObjectPath& objectName,
			OW_CIMObjectPathResultHandlerIFC& result,
			const OW_String& role)
	{
		m_pProv->referenceNames(env,assocName,objectName,result,role);
	}

private:
	OW_CppAssociatorProviderIFCRef m_pProv;
};

class OW_InstanceProviderProxy : public OW_InstanceProviderIFC
{
public:
	OW_InstanceProviderProxy(OW_CppInstanceProviderIFCRef pProv)
		: OW_InstanceProviderIFC()
		, m_pProv(pProv)
	{}


	virtual void deleteInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_CIMObjectPath& cop)
	{
		m_pProv->deleteInstance(env, ns, cop);
	}

	virtual void enumInstanceNames(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& cop,
			OW_CIMObjectPathResultHandlerIFC& result,
			const OW_Bool& deep,
			const OW_CIMClass& cimClass)
	{
		m_pProv->enumInstanceNames(env,cop,result,deep,cimClass);
	}

	virtual void enumInstances(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_String& ns,
			const OW_String& className,
			OW_CIMInstanceResultHandlerIFC& result,
			const OW_Bool& deep,
			const OW_CIMClass& cimClass,
			const OW_Bool& localOnly)
	{
		m_pProv->enumInstances(env,ns,className,result,deep,cimClass,localOnly);
	}

	virtual OW_CIMInstance getInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& cop,
			const OW_CIMClass& cimClass,
			const OW_Bool& localOnly)
	{
		return m_pProv->getInstance(env,cop,cimClass,localOnly);
	}

	virtual OW_CIMObjectPath createInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& cop,
			const OW_CIMInstance& cimInstance)
	{
		return m_pProv->createInstance(env,cop,cimInstance);
	}

	virtual void modifyInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& cop,
			const OW_CIMInstance& cimInstance)
	{
		m_pProv->modifyInstance(env,cop,cimInstance);
	}

private:
	OW_CppInstanceProviderIFCRef m_pProv;
};

class OW_MethodProviderProxy : public OW_MethodProviderIFC
{
public:

	OW_MethodProviderProxy(OW_CppMethodProviderIFCRef pProv)
	: OW_MethodProviderIFC()
	, m_pProv(pProv)
	{}

	virtual OW_CIMValue invokeMethod(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& cop,
			const OW_String& methodName,
			const OW_CIMParamValueArray& in,
			OW_CIMParamValueArray& out)
	{
		return m_pProv->invokeMethod(env,cop,methodName,in,out);
	}

private:

	OW_CppMethodProviderIFCRef m_pProv;
};

class OW_PropertyProviderProxy : public OW_PropertyProviderIFC
{
public:
	OW_PropertyProviderProxy(OW_CppPropertyProviderIFCRef pProv)
	: OW_PropertyProviderIFC()
	, m_pProv(pProv)
	{}

	virtual OW_CIMValue getPropertyValue(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& cop,
			const OW_String& originClass,
			const OW_String& propertyName)
	{
		return m_pProv->getPropertyValue(env,cop,originClass,propertyName);
	}

	virtual void setPropertyValue(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& cop,
			const OW_String& originClass,
			const OW_String& propertyName, 	
			const OW_CIMValue& val)
	{
		m_pProv->setPropertyValue(env,cop,originClass,propertyName,val);
	}

private:
	OW_CppPropertyProviderIFCRef m_pProv;
};

class OW_IndicationExportProviderProxy : public OW_IndicationExportProviderIFC
{
public:
	OW_IndicationExportProviderProxy(OW_CppIndicationExportProviderIFCRef pProv) :
		m_pProv(pProv) {}

	virtual OW_StringArray getHandlerClassNames()
	{
		return m_pProv->getHandlerClassNames();
	}
	
	virtual void exportIndication(const OW_ProviderEnvironmentIFCRef& env,
		OW_CIMInstance& indHandlerInst, OW_CIMInstance& indicationInst)
	{
		m_pProv->exportIndication(env, indHandlerInst, indicationInst);
	}

private:
	OW_CppIndicationExportProviderIFCRef m_pProv;
};

class OW_PolledProviderProxy : public OW_PolledProviderIFC
{
public:
	OW_PolledProviderProxy(OW_CppPolledProviderIFCRef pProv) :
		m_pProv(pProv) {}

	virtual OW_Int32 poll(const OW_ProviderEnvironmentIFCRef& env )
			{ return m_pProv->poll(env); }

	virtual OW_Int32 getInitialPollingInterval(const OW_ProviderEnvironmentIFCRef& env)
			{ return m_pProv->getInitialPollingInterval(env); }

private:
	OW_CppPolledProviderIFCRef m_pProv;
};

#endif	// __OW_PROXYPROVIDER_HPP__

