/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All, IBM Corp. rights reserved.
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
#include "cmpisrv.h"

#include <alloca.h>

// debugging
#define DDD(X) // X

/////////////////////////////////////////////////////////////////////////////
OW_CMPIInstanceProviderProxy::~OW_CMPIInstanceProviderProxy() 
{
}

/////////////////////////////////////////////////////////////////////////////
void
	OW_CMPIInstanceProviderProxy::enumInstanceNames(
	const OW_ProviderEnvironmentIFCRef& env,
	const OW_String& ns,
	const OW_String& className,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_CIMClass& cimClass )
{
	(void) cimClass;

	env->getLogger()->
		logDebug("OW_CMPIInstanceProviderProxy::enumInstanceNames()");

	if (m_ftable->miVector.instMI->ft->enumInstanceNames!= NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};

		::OperationContext context;

		//m_ftable->broker.hdl = 
		//	new OW_ProviderEnvironmentIFCRef(env);
		OW_ProviderEnvironmentIFCRef env2(env);
		m_ftable->broker.hdl = static_cast<void *>(&env2);

		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&(m_ftable->broker), &eCtx);

		OW_CIMObjectPath cop(className, ns);
		CMPI_ObjectPathOnStack eRef(cop);
		CMPI_ResultOnStack eRes(result);

		CMPIFlags flgs=0;
		eCtx.ft->addEntry(&eCtx,CMPIInvocationFlags,
			(CMPIValue*)&flgs,CMPI_uint32);

		::CMPIInstanceMI *mi = m_ftable->miVector.instMI;

		rc = m_ftable->miVector.instMI->ft->enumInstanceNames(
			mi, &eCtx, &eRes, &eRef);

		//delete ((OW_ProviderEnvironmentIFCRef *)(m_ftable->broker.hdl));

		if (rc.rc == CMPI_RC_OK) return;
		else
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, rc.msg ? CMGetCharPtr(rc.msg) : "");
		}
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support enumInstanceNames");
	}
}

/////////////////////////////////////////////////////////////////////////////
void
	OW_CMPIInstanceProviderProxy::enumInstances(
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
	const OW_CIMClass& cimClass )
{
	(void) localOnly;
	(void) deep;
	(void) requestedClass;
	(void) cimClass;

	env->getLogger()->
		logDebug("OW_CMPIInstanceProviderProxy::enumInstances()");

	if (m_ftable->miVector.instMI->ft->enumInstances!= NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};
		const char **props = NULL;
		int pCount = 0;

		::OperationContext context;

		OW_ProviderEnvironmentIFCRef env2(env);
		m_ftable->broker.hdl = static_cast<void *>(&env2);

		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&(m_ftable->broker), &eCtx);

		OW_CIMObjectPath cop(className, ns);
		CMPI_ObjectPathOnStack eRef(cop);
		CMPI_ResultOnStack eRes(result);

		if (propertyList)
		{
			if (propertyList->size()>0)
			{
				pCount = propertyList->size();
				props = (const char **)
				alloca(1+pCount*sizeof(char *));
				for (int i=0;i<pCount;i++)
					props[i]=
						(*propertyList)[i].allocateCString();
				props[pCount]=NULL;
			}
		}

		CMPIFlags flgs=0;
		if (includeQualifiers) flgs|=CMPI_FLAG_IncludeQualifiers;
		if (includeClassOrigin)	flgs|=CMPI_FLAG_IncludeClassOrigin;

		eCtx.ft->addEntry(&eCtx,CMPIInvocationFlags,
			(CMPIValue*)&flgs,CMPI_uint32);

		::CMPIInstanceMI *mi = m_ftable->miVector.instMI;

		rc = m_ftable->miVector.instMI->ft->enumInstances(
			mi, &eCtx, &eRes, &eRef, (char **)props);

		if (props && pCount)
			for (int i=0;i<pCount;i++) free((char *)props[i]);

		if (rc.rc == CMPI_RC_OK) return;
		else
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, rc.msg ? CMGetCharPtr(rc.msg) : "");
		}
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support enumInstances");
	}
}

/////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
	OW_CMPIInstanceProviderProxy::getInstance(const OW_ProviderEnvironmentIFCRef &env,
	const OW_String& ns,
	const OW_CIMObjectPath& instanceName,
	OW_Bool localOnly,
	OW_Bool includeQualifiers, 
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, 
	const OW_CIMClass& cimClass)
{
	(void) localOnly;
	(void) cimClass;
	OW_CIMInstance rval;

	env->getLogger()->
		logDebug("OW_CMPIInstanceProviderProxy::getInstance()");

	if (m_ftable->miVector.instMI->ft->getInstance!= NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};
		const char **props = NULL;
		int pCount = 0;

		::OperationContext context;

		OW_ProviderEnvironmentIFCRef env2(env);
		m_ftable->broker.hdl = static_cast<void *>(&env2);

		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&(m_ftable->broker), &eCtx);

		OW_CIMObjectPath copWithNS(instanceName);
		copWithNS.setNameSpace(ns);

		CMPI_ObjectPathOnStack eRef(copWithNS);
		//CMPI_ResultOnStack eRes(result);
		CMPIInstanceValueResultHandler instrh;
		CMPI_ResultOnStack eRes(instrh);

		if (propertyList)
		{
			if (propertyList->size()>0)
			{
				pCount = propertyList->size();
				props = (const char **)
				alloca(1+pCount*sizeof(char *));
				for (int i=0;i<pCount;i++)
					props[i]=
						(*propertyList)[i].allocateCString();
				props[pCount]=NULL;
			}
		}

		CMPIFlags flgs=0;
		if (includeQualifiers) flgs|=CMPI_FLAG_IncludeQualifiers;
		if (includeClassOrigin)	flgs|=CMPI_FLAG_IncludeClassOrigin;

		eCtx.ft->addEntry(&eCtx,CMPIInvocationFlags,
			(CMPIValue*)&flgs,CMPI_uint32);

		::CMPIInstanceMI *mi = m_ftable->miVector.instMI;

		rc = m_ftable->miVector.instMI->ft->getInstance(
			mi, &eCtx, &eRes, &eRef, (char **)props);

		if (rc.rc == CMPI_RC_OK)
		{
			return instrh.getValue();
		}
		else
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, rc.msg ? CMGetCharPtr(rc.msg) : ""); 
		}
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support getInstance");
	}

	return rval;
}

#ifndef OW_DISABLE_INSTANCE_MANIPULATION
/////////////////////////////////////////////////////////////////////////////
void
	OW_CMPIInstanceProviderProxy::deleteInstance(const OW_ProviderEnvironmentIFCRef &env,
	const OW_String& ns, const OW_CIMObjectPath& cop)
{
	env->getLogger()->
		logDebug("OW_CMPIInstanceProviderProxy::deleteInstance()");

	if (m_ftable->miVector.instMI->ft->deleteInstance!= NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};

		::OperationContext context;

		OW_ProviderEnvironmentIFCRef env2(env);
		m_ftable->broker.hdl = static_cast<void *>(&env2);

		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&(m_ftable->broker), &eCtx);

		OW_CIMObjectPath copWithNS(cop);
		copWithNS.setNameSpace(ns);

		CMPI_ObjectPathOnStack eRef(copWithNS);
		CMPI_ResultOnStack eRes;

		CMPIFlags flgs=0;
		eCtx.ft->addEntry(&eCtx,CMPIInvocationFlags,
			(CMPIValue*)&flgs,CMPI_uint32);

		::CMPIInstanceMI *mi = m_ftable->miVector.instMI;

		rc = m_ftable->miVector.instMI->ft->deleteInstance(
			mi, &eCtx, &eRes, &eRef);

		if (rc.rc == CMPI_RC_OK) return;
		else
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, rc.msg ? CMGetCharPtr(rc.msg) : "");
		}
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support deleteInstance");
	}
}

/////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
	OW_CMPIInstanceProviderProxy::createInstance(
	const OW_ProviderEnvironmentIFCRef &env, const OW_String& ns,
	const OW_CIMInstance& cimInstance)
{
	OW_CIMObjectPath rval;

	env->getLogger()->
		logDebug(format("OW_CMPIInstanceProviderProxy::createInstance() %1", cimInstance));

	if (m_ftable->miVector.instMI->ft->createInstance!= NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};

		::OperationContext context;

		OW_ProviderEnvironmentIFCRef env2(env);
		m_ftable->broker.hdl = static_cast<void *>(&env2);

		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&(m_ftable->broker), &eCtx);

		OW_CIMObjectPath cop(ns, cimInstance);

		CMPI_ObjectPathOnStack eRef(cop);
		CMPI_InstanceOnStack eInst(cimInstance);
		CMPIObjectPathValueResultHandler coprh;
		CMPI_ResultOnStack eRes(coprh);

		CMPIFlags flgs=0;
		eCtx.ft->addEntry(&eCtx,CMPIInvocationFlags,
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
			OW_THROWCIMMSG(OW_CIMException::FAILED, rc.msg ? CMGetCharPtr(rc.msg) : "");
		}
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support createInstance");
	}

	return rval;
}

/////////////////////////////////////////////////////////////////////////////
void
	OW_CMPIInstanceProviderProxy::modifyInstance(const OW_ProviderEnvironmentIFCRef &env,
	const OW_String& ns,
	const OW_CIMInstance& modifiedInstance,
	const OW_CIMInstance& previousInstance,
	OW_Bool includeQualifiers,
	const OW_StringArray* propertyList,
	const OW_CIMClass& theClass)
{
	(void) includeQualifiers;
	(void) propertyList;
	(void) theClass;

	env->getLogger()->
		logDebug("OW_CMPIInstanceProviderProxy::modifyInstance()");

	if (m_ftable->miVector.instMI->ft->setInstance!= NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};
		const char ** props = NULL;
		int pCount = 0;

		::OperationContext context;

		OW_ProviderEnvironmentIFCRef env2(env);
		m_ftable->broker.hdl = static_cast<void *>(&env2);

		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&(m_ftable->broker), &eCtx);

		OW_CIMObjectPath instRef(ns, previousInstance);

		CMPI_ObjectPathOnStack eRef(instRef);
		CMPI_InstanceOnStack eInst(modifiedInstance);
		//CMPI_ResultOnStack eRes(result);
		CMPI_ResultOnStack eRes;

		CMPIFlags flgs=0;
		//if (includeQualifier) flgs|=CMPI_FLAG_IncludeQualifiers;
		eCtx.ft->addEntry(&eCtx,CMPIInvocationFlags,
			(CMPIValue*)&flgs,CMPI_uint32);

		::CMPIInstanceMI *mi = m_ftable->miVector.instMI;

		rc = m_ftable->miVector.instMI->ft->setInstance(
			mi, &eCtx, &eRes, &eRef, &eInst, (char **)props);

		if (props && pCount)
			for (int i=0;i<pCount;i++) free((char *)props[i]);

		if (rc.rc == CMPI_RC_OK) return;
		else
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, rc.msg ? CMGetCharPtr(rc.msg) : "");
		}
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support modifyInstance");
	}
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION


