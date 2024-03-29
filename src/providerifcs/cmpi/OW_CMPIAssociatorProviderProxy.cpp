/*******************************************************************************
* Copyright (C) 2003-2004 Quest Software, Inc., IBM Corp.  All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
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
#include "OW_CMPIAssociatorProviderProxy.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMException.hpp"
#include "blocxx/Format.hpp"
#include "OW_CMPIProviderIFCUtils.hpp"
#include "blocxx/Logger.hpp"
#include "cmpisrv.h"

#ifdef OW_HAVE_ALLOCA_H
#include <alloca.h>
#endif
#include <stdlib.h> // for alloca() on FreeBSD

namespace OW_NAMESPACE
{

using namespace WBEMFlags;
using namespace blocxx;

namespace
{
	const String COMPONENT_NAME("ow.provider.cmpi.ifc");
}
/////////////////////////////////////////////////////////////////////////////
void CMPIAssociatorProviderProxy::associatorNames(
	const ProviderEnvironmentIFCRef &env,
	CIMObjectPathResultHandlerIFC& result,
	const String& ns,
	const CIMObjectPath& objectName,
	const String& assocClass,
	const String& resultClass,
	const String& role,
	const String& resultRole)
{
	Logger lgr(COMPONENT_NAME);
	BLOCXX_LOG_DEBUG3(lgr, "CMPIAssociatorProviderProxy::associatorNames()");

	m_ftable->lastAccessTime.setToCurrent();

	if (m_ftable->miVector.assocMI->ft->associatorNames != NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};
		::CMPIOperationContext context;
		ProviderEnvironmentIFCRef env2(env);
		::CMPI_Broker localBroker(m_ftable->broker);
		localBroker.hdl = static_cast<void *>(&env2);
		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&localBroker, &eCtx);

		// initialize path
		CIMObjectPath objectNameWithNS(objectName);
		objectNameWithNS.setNameSpace(ns);
		CMPI_ObjectPathOnStack eRef(objectNameWithNS);
		CMPI_ResultOnStack eRes(result);

		char *aClass = const_cast<char*>(assocClass.c_str());

		char *_resultClass = const_cast<char*>(resultClass.empty() ? 0 :
			resultClass.c_str());

		char *_role = const_cast<char*>(role.empty() ? 0 : role.c_str());

		char *_resultRole = const_cast<char*>(resultRole.empty() ? 0 :
			resultRole.c_str());

		CMPIPrepareContext(env, eCtx);

		::CMPIAssociationMI *mi = m_ftable->miVector.assocMI;

		rc = m_ftable->miVector.assocMI->ft->associatorNames(mi, &eCtx, &eRes,
			&eRef, aClass, _resultClass, _role, _resultRole);

		if (rc.rc != CMPI_RC_OK)
		{
			OW_THROWCIMMSG(CIMException::ErrNoType(rc.rc), rc.msg ? CMGetCharPtr(rc.msg) : "");
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::NOT_SUPPORTED,
			"Provider does not support associatorNames");
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMPIAssociatorProviderProxy::associators(
	const ProviderEnvironmentIFCRef &env,
	CIMInstanceResultHandlerIFC& result,
	const String& ns,
	const CIMObjectPath& objectName,
	const String& assocClass,
	const String& resultClass,
	const String& role,
	const String& resultRole,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray *propertyList)
{
	Logger lgr(COMPONENT_NAME);
	BLOCXX_LOG_DEBUG3(lgr, "CMPIAssociatorProviderProxy::associators()");

	m_ftable->lastAccessTime.setToCurrent();

	if (m_ftable->miVector.assocMI->ft->associators != NULL)
	{
		const char **props = NULL;
		int pCount = 0;

		CMPIStatus rc = {CMPI_RC_OK, NULL};
		::CMPIOperationContext context;
		ProviderEnvironmentIFCRef env2(env);
		::CMPI_Broker localBroker(m_ftable->broker);
		localBroker.hdl = static_cast<void *>(&env2);
		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&localBroker, &eCtx);
		// initialize path
		CIMObjectPath objectNameWithNS(objectName);
		objectNameWithNS.setNameSpace(ns);
		CMPI_ObjectPathOnStack eRef(objectNameWithNS);
		CMPI_ResultOnStack eRes(result);

		char* aClass = const_cast<char*>(assocClass.c_str());

		if (propertyList && propertyList->size() > 0)
		{
			pCount = propertyList->size();
			props = reinterpret_cast<const char **>
				(alloca(1+pCount*sizeof(char *)));

			for (int i = 0; i < pCount; i++)
			{
				props[i]= const_cast<char*>((*propertyList)[i].c_str());
			}

			props[pCount]=NULL;
		}

		char *_resultClass = const_cast<char*>(resultClass.empty() ? 0 :
			resultClass.c_str());

		char *_role = const_cast<char*>(role.empty() ? 0 : role.c_str());

		char * _resultRole = const_cast<char*>(resultRole.empty() ? 0 :
			resultRole.c_str());

		CMPIPrepareContext(env, eCtx, E_NOT_LOCAL_ONLY, E_SHALLOW,
			includeQualifiers, includeClassOrigin);

		::CMPIAssociationMI * mi = m_ftable->miVector.assocMI;

		rc=m_ftable->miVector.assocMI->ft->associators(
			mi,&eCtx,&eRes,&eRef, aClass,
			_resultClass, _role, _resultRole, props);

		if (rc.rc != CMPI_RC_OK)
		{
			OW_THROWCIMMSG(CIMException::ErrNoType(rc.rc), rc.msg ? CMGetCharPtr(rc.msg) : "");
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::NOT_SUPPORTED,
			"Provider does not support associators");
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMPIAssociatorProviderProxy::references(
	const ProviderEnvironmentIFCRef &env,
	CIMInstanceResultHandlerIFC& result,
	const String& ns,
	const CIMObjectPath& objectName,
	const String& resultClass,
	const String& role,
	EIncludeQualifiersFlag includeQualifiers,
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray *propertyList)
{
	Logger lgr(COMPONENT_NAME);
	BLOCXX_LOG_DEBUG3(lgr, "CMPIAssociatorProviderProxy::references()");

	m_ftable->lastAccessTime.setToCurrent();

	if (m_ftable->miVector.assocMI->ft->references != NULL)
	{
		const char **props = NULL;
		int pCount = 0;

		CMPIStatus rc = {CMPI_RC_OK, NULL};
		::CMPIOperationContext context;
		ProviderEnvironmentIFCRef env2(env);

		::CMPI_Broker localBroker(m_ftable->broker);
		localBroker.hdl = static_cast<void *>(&env2);

		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&localBroker, &eCtx);
		// initialize path
		CIMObjectPath objectNameWithNS(objectName);
		objectNameWithNS.setNameSpace(ns);
		CMPI_ObjectPathOnStack eRef(objectNameWithNS);
		CMPI_ResultOnStack eRes(result);
		char *aClass = const_cast<char*>(resultClass.c_str());

		if (propertyList)
		{
			if (propertyList->size() > 0)
			{
				pCount = propertyList->size();
				props = reinterpret_cast<const char **>
					(alloca(1+pCount*sizeof(char *)));

				for (int i = 0; i < pCount; i++)
				{
					props[i] = ((*propertyList)[i].c_str());
				}

				props[pCount] = NULL;
			}
		}

		char *_role = const_cast<char*>(role.empty() ? 0 : role.c_str());

		CMPIPrepareContext(env, eCtx, E_NOT_LOCAL_ONLY, E_SHALLOW,
			includeQualifiers, includeClassOrigin);

		::CMPIAssociationMI *mi = m_ftable->miVector.assocMI;
		rc=m_ftable->miVector.assocMI->ft->references(mi, &eCtx, &eRes, &eRef,
			aClass, _role, props);

		if (rc.rc != CMPI_RC_OK)
		{
			OW_THROWCIMMSG(CIMException::ErrNoType(rc.rc), rc.msg ? CMGetCharPtr(rc.msg) : "");
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::NOT_SUPPORTED,
			"Provider does not support references");
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMPIAssociatorProviderProxy::referenceNames(
	const ProviderEnvironmentIFCRef &env,
	CIMObjectPathResultHandlerIFC& result,
	const String& ns,
	const CIMObjectPath& objectName,
	const String& resultClass,
	const String& role)
{
	Logger lgr(COMPONENT_NAME);
	BLOCXX_LOG_DEBUG3(lgr, "CMPIAssociatorProviderProxy::referenceNames()");

	m_ftable->lastAccessTime.setToCurrent();

	if (m_ftable->miVector.assocMI->ft->referenceNames != NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};
		::CMPIOperationContext context;

		ProviderEnvironmentIFCRef env2(env);
		::CMPI_Broker localBroker(m_ftable->broker);
		localBroker.hdl = static_cast<void *>(&env2);

		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&localBroker, &eCtx);

		// initialize path
		CIMObjectPath objectNameWithNS(objectName);
		objectNameWithNS.setNameSpace(ns);

		CMPI_ObjectPathOnStack eRef(objectNameWithNS);
		CMPI_ResultOnStack eRes(result);
		char* aClass = const_cast<char*>(resultClass.c_str());
		char* _role = const_cast<char*>(role.empty() ? 0 : role.c_str());
		CMPIPrepareContext(env, eCtx);

		::CMPIAssociationMI * mi = m_ftable->miVector.assocMI;
		rc = m_ftable->miVector.assocMI->ft->referenceNames(
			mi, &eCtx, &eRes, &eRef, aClass, _role);

		if (rc.rc != CMPI_RC_OK)
		{
			OW_THROWCIMMSG(CIMException::ErrNoType(rc.rc), rc.msg ? CMGetCharPtr(rc.msg) : "");
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::NOT_SUPPORTED,
			"Provider does not support referenceNames");
	}
}

void
CMPIAssociatorProviderProxy::shuttingDown(const ProviderEnvironmentIFCRef& env)
{
	if (m_ftable->miVector.assocMI)
	{
		::CMPIOperationContext context;
		CMPI_ContextOnStack eCtx(context);
		ProviderEnvironmentIFCRef env2(env);
		::CMPI_Broker localBroker(m_ftable->broker);
		localBroker.hdl = static_cast<void *>(&env2);
		CMPI_ThreadContext thr(&localBroker, &eCtx);
		m_ftable->miVector.assocMI->ft->cleanup(m_ftable->miVector.assocMI,
			&eCtx, true);
	}
}

} // end namespace OW_NAMESPACE

