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

class OW_AssociatorProviderProxy : public OW_AssociatorProviderIFC
{
public:

	OW_AssociatorProviderProxy(OW_CppAssociatorProviderIFCRef pProv);
	
	virtual OW_CIMInstanceEnumeration associators(
			const OW_ProviderEnvironmentIFCRef& env,
			OW_CIMObjectPath assocName,
			OW_CIMObjectPath objectName,
			OW_String resultClass,
			OW_String role,
			OW_String resultRole,
			OW_Bool includeQualifiers,
			OW_Bool includeClassOrigin,
			const OW_StringArray* propertyList);

	virtual OW_CIMObjectPathEnumeration associatorNames(
			const OW_ProviderEnvironmentIFCRef& env,
			OW_CIMObjectPath assocName,
			OW_CIMObjectPath objectName,
			OW_String resultClass,
			OW_String role,
			OW_String resultRole);

	virtual OW_CIMInstanceEnumeration references(
			const OW_ProviderEnvironmentIFCRef& env,
			OW_CIMObjectPath assocName,
			OW_CIMObjectPath objectName,
			OW_String role,
			OW_Bool includeQualifiers,
			OW_Bool includeClassOrigin,
			const OW_StringArray* propertyList);

	virtual OW_CIMObjectPathEnumeration referenceNames(
			const OW_ProviderEnvironmentIFCRef& env,
			OW_CIMObjectPath assocName,
			OW_CIMObjectPath objectName,
			OW_String role);

private:
	OW_CppAssociatorProviderIFCRef m_pProv;
};

class OW_InstanceProviderProxy : public OW_InstanceProviderIFC
{
public:
	OW_InstanceProviderProxy(OW_CppInstanceProviderIFCRef pProv);

	virtual void deleteInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			OW_CIMObjectPath cop);

	virtual OW_CIMObjectPathEnumeration enumInstances(
			const OW_ProviderEnvironmentIFCRef& env,
			OW_CIMObjectPath cop,
			OW_Bool deep,
			OW_CIMClass cimClass);

	virtual OW_CIMInstanceEnumeration enumInstances(
			const OW_ProviderEnvironmentIFCRef& env,
			OW_CIMObjectPath cop,
			OW_Bool deep,
			OW_CIMClass cimClass,
			OW_Bool localOnly);

	virtual OW_CIMInstance getInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			OW_CIMObjectPath cop,
			OW_CIMClass cimClass,
			OW_Bool localOnly);

	virtual OW_CIMObjectPath createInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& cop,
			OW_CIMInstance cimInstance);

	virtual void setInstance(
			const OW_ProviderEnvironmentIFCRef& env,
			OW_CIMObjectPath cop,
			OW_CIMInstance cimInstance);

private:
	OW_CppInstanceProviderIFCRef m_pProv;
};

class OW_MethodProviderProxy : public OW_MethodProviderIFC
{
public:

	OW_MethodProviderProxy(OW_CppMethodProviderIFCRef pProv);

	virtual OW_CIMValue invokeMethod(
			const OW_ProviderEnvironmentIFCRef& env,
			const OW_CIMObjectPath& cop,
			const OW_String& methodName,
			const OW_CIMValueArray& in,
			OW_CIMValueArray& out);

private:

	OW_CppMethodProviderIFCRef m_pProv;
};

class OW_PropertyProviderProxy : public OW_PropertyProviderIFC
{
public:
	OW_PropertyProviderProxy(OW_CppPropertyProviderIFCRef pProv);
	virtual OW_CIMValue getPropertyValue(
			const OW_ProviderEnvironmentIFCRef& env,
			OW_CIMObjectPath cop,
			OW_String originClass,
			OW_String propertyName);

	virtual void setPropertyValue(
			const OW_ProviderEnvironmentIFCRef& env,
			OW_CIMObjectPath cop,
			OW_String originClass,
			OW_String propertyName, 	OW_CIMValue val);

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

	virtual long poll(const OW_ProviderEnvironmentIFCRef& env
			) { return m_pProv->poll(env); }

	virtual long getInitialPollingInterval(const OW_ProviderEnvironmentIFCRef& env
			)
	{
		return m_pProv->getInitialPollingInterval(env);
	}

private:
	OW_CppPolledProviderIFCRef m_pProv;
};

#endif	// __OW_PROXYPROVIDER_HPP__

