/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc. All, IBM Corp. rights reserved.
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
*
* Author:        Markus Mueller <sedgewick_de@yahoo.de>
*
*******************************************************************************/
#include "OW_config.h"
#include "OW_CMPIInstanceProviderProxy.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_CMPIProviderIFCUtils.hpp"
#include "OW_Logger.hpp"
#include "cmpisrv.h"
#ifdef OW_HAVE_ALLOCA_H
#include <alloca.h>
#endif
#include <stdlib.h> // for alloca() on FreeBSD

namespace OW_NAMESPACE
{

// debugging
#define DDD(X) // X
using namespace WBEMFlags;

namespace
{
	const String COMPONENT_NAME("ow.provider.cmpi.ifc");
}

/////////////////////////////////////////////////////////////////////////////
CMPIInstanceProviderProxy::~CMPIInstanceProviderProxy()
{
}

/////////////////////////////////////////////////////////////////////////////
void
CMPIInstanceProviderProxy::enumInstanceNames(
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const String& className,
	CIMObjectPathResultHandlerIFC& result,
	const CIMClass& cimClass )
{
	OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), "CMPIInstanceProviderProxy::enumInstanceNames()");

	m_ftable->lastAccessTime.setToCurrent();

	if (m_ftable->miVector.instMI->ft->enumInstanceNames!= NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};
		::CMPIOperationContext context;
		ProviderEnvironmentIFCRef env2(env);
		::CMPI_Broker localBroker(m_ftable->broker);
		localBroker.hdl = static_cast<void *>(&env2);
		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&localBroker, &eCtx);
		CIMObjectPath cop(className, ns);
		CMPI_ObjectPathOnStack eRef(cop);
		CMPI_ResultOnStack eRes(result);
		CMPIFlags flgs=0;
		eCtx.ft->addEntry(&eCtx, const_cast<char*>(CMPIInvocationFlags),
			(CMPIValue*)&flgs,CMPI_uint32);
		::CMPIInstanceMI *mi = m_ftable->miVector.instMI;
		rc = m_ftable->miVector.instMI->ft->enumInstanceNames(
			mi, &eCtx, &eRes, &eRef);
		if (rc.rc == CMPI_RC_OK)
		{
			return;
		}
		else
		{
			OW_THROWCIMMSG(CIMException::ErrNoType(rc.rc), rc.msg ? CMGetCharPtr(rc.msg) : "");
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support enumInstanceNames");
	}
}
/////////////////////////////////////////////////////////////////////////////
void
CMPIInstanceProviderProxy::enumInstances(
	const ProviderEnvironmentIFCRef& env,
	const String& ns,
	const String& className,
	CIMInstanceResultHandlerIFC& result,
	ELocalOnlyFlag localOnly,
	EDeepFlag deep,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList,
	const CIMClass& requestedClass,
	const CIMClass& cimClass )
{
	OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), "CMPIInstanceProviderProxy::enumInstances()");

	m_ftable->lastAccessTime.setToCurrent();

	if (m_ftable->miVector.instMI->ft->enumInstances!= NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};
		const char **props = NULL;
		int pCount = 0;
		::CMPIOperationContext context;
		ProviderEnvironmentIFCRef env2(env);
		::CMPI_Broker localBroker(m_ftable->broker);
		localBroker.hdl = static_cast<void *>(&env2);
		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&localBroker, &eCtx);
		CIMObjectPath cop(className, ns);
		CMPI_ObjectPathOnStack eRef(cop);
		CMPI_ResultOnStack eRes(result);

		if (propertyList)
		{
			if (propertyList->size()>0)
			{
				pCount = propertyList->size();
				props = (const char **) alloca(1+pCount*sizeof(char *));

				for (int i = 0; i < pCount; i++)
					props[i]= (*propertyList)[i].c_str();

				props[pCount]=NULL;
			}
		}
		CMPIFlags flgs = 0;

		if (includeQualifiers)
			flgs |= CMPI_FLAG_IncludeQualifiers;

		if (includeClassOrigin)
			flgs |= CMPI_FLAG_IncludeClassOrigin;

		eCtx.ft->addEntry(&eCtx,const_cast<char*>(CMPIInvocationFlags), (CMPIValue*)&flgs,
			CMPI_uint32);

		::CMPIInstanceMI *mi = m_ftable->miVector.instMI;
		rc = m_ftable->miVector.instMI->ft->enumInstances(
			mi, &eCtx, &eRes, &eRef, (char **)props);

		if (rc.rc != CMPI_RC_OK)
		{
			OW_THROWCIMMSG(CIMException::ErrNoType(rc.rc), rc.msg ? CMGetCharPtr(rc.msg) : "");
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support enumInstances");
	}
}
/////////////////////////////////////////////////////////////////////////////
CIMInstance
CMPIInstanceProviderProxy::getInstance(const ProviderEnvironmentIFCRef &env,
	const String& ns,
	const CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList,
	const CIMClass& cimClass)
{
	CIMInstance rval;
	OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), "CMPIInstanceProviderProxy::getInstance()");

	m_ftable->lastAccessTime.setToCurrent();

	if (m_ftable->miVector.instMI->ft->getInstance != NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};
		const char **props = NULL;
		int pCount = 0;
		::CMPIOperationContext context;
		ProviderEnvironmentIFCRef env2(env);
		::CMPI_Broker localBroker(m_ftable->broker);
		localBroker.hdl = static_cast<void *>(&env2);
		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&localBroker, &eCtx);
		CIMObjectPath copWithNS(instanceName);
		copWithNS.setNameSpace(ns);
		CMPI_ObjectPathOnStack eRef(copWithNS);
		//CMPI_ResultOnStack eRes(result);
		CMPIInstanceValueResultHandler instrh;
		CMPI_ResultOnStack eRes(instrh);
		if (propertyList)
		{
			if (propertyList->size() > 0)
			{
				pCount = propertyList->size();
				props = (const char **) alloca(1+pCount*sizeof(char *));

				for (int i = 0; i < pCount; i++)
					props[i] = (*propertyList)[i].c_str();

				props[pCount] = NULL;
			}
		}
		CMPIFlags flgs=0;

		if (includeQualifiers)
			flgs |= CMPI_FLAG_IncludeQualifiers;

		if (includeClassOrigin)
			flgs |= CMPI_FLAG_IncludeClassOrigin;

		eCtx.ft->addEntry(&eCtx,const_cast<char*>(CMPIInvocationFlags), (CMPIValue*)&flgs,
			CMPI_uint32);

		::CMPIInstanceMI *mi = m_ftable->miVector.instMI;

		rc = m_ftable->miVector.instMI->ft->getInstance(
			mi, &eCtx, &eRes, &eRef, (char **)props);

		if (rc.rc == CMPI_RC_OK)
		{
			return instrh.getValue();
		}
		else
		{
			OW_THROWCIMMSG(CIMException::ErrNoType(rc.rc), rc.msg ? CMGetCharPtr(rc.msg) : "");
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support getInstance");
	}
	return rval;
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
/////////////////////////////////////////////////////////////////////////////
void
CMPIInstanceProviderProxy::deleteInstance(const ProviderEnvironmentIFCRef &env,
	const String& ns, const CIMObjectPath& cop)
{
	OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), "CMPIInstanceProviderProxy::deleteInstance()");

	m_ftable->lastAccessTime.setToCurrent();

	if (m_ftable->miVector.instMI->ft->deleteInstance!= NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};
		::CMPIOperationContext context;
		ProviderEnvironmentIFCRef env2(env);
		::CMPI_Broker localBroker(m_ftable->broker);
		localBroker.hdl = static_cast<void *>(&env2);
		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&localBroker, &eCtx);
		CIMObjectPath copWithNS(cop);
		copWithNS.setNameSpace(ns);
		CMPI_ObjectPathOnStack eRef(copWithNS);
		CMPI_ResultOnStack eRes;
		CMPIFlags flgs=0;
		eCtx.ft->addEntry(&eCtx,const_cast<char*>(CMPIInvocationFlags),
			(CMPIValue*)&flgs,CMPI_uint32);
		::CMPIInstanceMI *mi = m_ftable->miVector.instMI;

		rc = m_ftable->miVector.instMI->ft->deleteInstance(
			mi, &eCtx, &eRes, &eRef);

		if (rc.rc != CMPI_RC_OK)
		{
			OW_THROWCIMMSG(CIMException::ErrNoType(rc.rc), rc.msg ?
				CMGetCharPtr(rc.msg) : "");
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support deleteInstance");
	}
}
/////////////////////////////////////////////////////////////////////////////
CIMObjectPath
	CMPIInstanceProviderProxy::createInstance(
	const ProviderEnvironmentIFCRef &env, const String& ns,
	const CIMInstance& cimInstance)
{
	CIMObjectPath rval;
	OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), Format("CMPIInstanceProviderProxy::createInstance() %1", cimInstance));

	m_ftable->lastAccessTime.setToCurrent();

	if (m_ftable->miVector.instMI->ft->createInstance!= NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};
		::CMPIOperationContext context;
		ProviderEnvironmentIFCRef env2(env);
		::CMPI_Broker localBroker(m_ftable->broker);
		localBroker.hdl = static_cast<void *>(&env2);
		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&localBroker, &eCtx);
		CIMObjectPath cop(ns, cimInstance);
		CMPI_ObjectPathOnStack eRef(cop);
		CMPI_InstanceOnStack eInst(cimInstance);
		CMPIObjectPathValueResultHandler coprh;
		CMPI_ResultOnStack eRes(coprh);
		CMPIFlags flgs=0;
		eCtx.ft->addEntry(&eCtx,const_cast<char*>(CMPIInvocationFlags),
			(CMPIValue*)&flgs,CMPI_uint32);
		::CMPIInstanceMI *mi = m_ftable->miVector.instMI;
// Cheating
		rc = m_ftable->miVector.instMI->ft->createInstance(
			mi, &eCtx, &eRes, &eRef, &eInst);
		if (rc.rc == CMPI_RC_OK)
		{
			return coprh.getValue();
		}
		else
		{
			OW_THROWCIMMSG(CIMException::ErrNoType(rc.rc), rc.msg ? CMGetCharPtr(rc.msg) : "");
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support createInstance");
	}
	return rval;
}
/////////////////////////////////////////////////////////////////////////////
void
	CMPIInstanceProviderProxy::modifyInstance(const ProviderEnvironmentIFCRef &env,
	const String& ns,
	const CIMInstance& modifiedInstance,
	const CIMInstance& previousInstance,
	EIncludeQualifiersFlag includeQualifiers,
	const StringArray* propertyList,
	const CIMClass& theClass)
{
	OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), "CMPIInstanceProviderProxy::modifyInstance()");

	m_ftable->lastAccessTime.setToCurrent();

	if (m_ftable->miVector.instMI->ft->setInstance!= NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};
		const char ** props = NULL;
		int pCount = 0;
		::CMPIOperationContext context;
		ProviderEnvironmentIFCRef env2(env);
		::CMPI_Broker localBroker(m_ftable->broker);
		localBroker.hdl = static_cast<void *>(&env2);
		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&localBroker, &eCtx);
		CIMObjectPath instRef(ns, previousInstance);
		CMPI_ObjectPathOnStack eRef(instRef);
		CMPI_InstanceOnStack eInst(modifiedInstance);
		//CMPI_ResultOnStack eRes(result);
		CMPI_ResultOnStack eRes;
		CMPIFlags flgs=0;
		//if (includeQualifier) flgs|=CMPI_FLAG_IncludeQualifiers;
		eCtx.ft->addEntry(&eCtx,const_cast<char*>(CMPIInvocationFlags),
			(CMPIValue*)&flgs,CMPI_uint32);
		::CMPIInstanceMI *mi = m_ftable->miVector.instMI;
		rc = m_ftable->miVector.instMI->ft->setInstance(
			mi, &eCtx, &eRes, &eRef, &eInst, (char **)props);
		if (props && pCount)
			for (int i=0;i<pCount;i++) free((char *)props[i]);
		if (rc.rc == CMPI_RC_OK) return;
		else
		{
			OW_THROWCIMMSG(CIMException::ErrNoType(rc.rc), rc.msg ? CMGetCharPtr(rc.msg) : "");
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support modifyInstance");
	}
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

} // end namespace OW_NAMESPACE

