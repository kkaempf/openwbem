/*******************************************************************************
* Copyright (C) 2003 Vintela, Inc., IBM Corp.  All rights reserved.
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
#include "OW_CMPIAssociatorProviderProxy.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_CMPIProviderIFCUtils.hpp"
#include "cmpisrv.h"
#include <alloca.h>

namespace OpenWBEM
{

using namespace WBEMFlags;
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
	env->getLogger()->
		logDebug("CMPIAssociatorProviderProxy::associatorNames()");

	if(m_ftable->miVector.assocMI->ft->associatorNames != NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};
		::CMPIOperationContext context;
		ProviderEnvironmentIFCRef env2(env);
		m_ftable->broker.hdl = static_cast<void *>(&env2);
		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&(m_ftable->broker), &eCtx);

		// initialize path
		CIMObjectPath objectNameWithNS(objectName);
		objectNameWithNS.setNameSpace(ns);
		CMPI_ObjectPathOnStack eRef(objectNameWithNS);
		CMPI_ResultOnStack eRes(result);

		char *aClass = const_cast<char*>(assocClass.c_str());

		CMPIFlags flgs = 0;
		char *_resultClass = const_cast<char*>(resultClass.empty() ? 0 :
			resultClass.c_str());

		char *_role = const_cast<char*>(role.empty() ? 0 : role.c_str());

		char *_resultRole = const_cast<char*>(resultRole.empty() ? 0 :
			resultRole.c_str());

		eCtx.ft->addEntry(&eCtx, CMPIInvocationFlags,
			(CMPIValue *)&flgs, CMPI_uint32);

		::CMPIAssociationMI *mi = m_ftable->miVector.assocMI;

		rc = m_ftable->miVector.assocMI->ft->associatorNames(mi, &eCtx, &eRes,
			&eRef, aClass, _resultClass, _role, _resultRole);

		if(rc.rc != CMPI_RC_OK)
		{
			OW_THROWCIMMSG(CIMException::FAILED,
				String(rc.rc).c_str());
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::FAILED,
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
	env->getLogger()->
		logDebug("CMPIAssociatorProviderProxy::associators()");

	if(m_ftable->miVector.assocMI->ft->associators != NULL)
	{
		char **props = NULL;
		int pCount = 0;

		CMPIStatus rc = {CMPI_RC_OK, NULL};
		::CMPIOperationContext context;
		ProviderEnvironmentIFCRef env2(env);
		m_ftable->broker.hdl = static_cast<void *>(&env2);
		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&(m_ftable->broker), &eCtx);
		// initialize path
		CIMObjectPath objectNameWithNS(objectName);
		objectNameWithNS.setNameSpace(ns);
		CMPI_ObjectPathOnStack eRef(objectNameWithNS);
		CMPI_ResultOnStack eRes(result);
	
		char* aClass = const_cast<char*>(assocClass.c_str());
	
		if(propertyList && propertyList->size() > 0)
		{
			pCount = propertyList->size();
			props = reinterpret_cast<char **>
				(alloca(1+pCount*sizeof(char *)));
	
			for(int i = 0; i < pCount; i++)
			{
				props[i]= const_cast<char*>((*propertyList)[i].c_str());
			}
	
			props[pCount]=NULL;
		}
	
		CMPIFlags flgs = 0;
	
		if(includeQualifiers)
		{
			flgs |= CMPI_FLAG_IncludeQualifiers;
		}
	
		if(includeClassOrigin)
		{
			flgs |= CMPI_FLAG_IncludeClassOrigin;
		}
	
		char *_resultClass = const_cast<char*>(resultClass.empty() ? 0 :
			resultClass.c_str());
	
		char *_role = const_cast<char*>(role.empty() ? 0 : role.c_str());
	
		char * _resultRole = const_cast<char*>(resultRole.empty() ? 0 :
			resultRole.c_str());
	
		eCtx.ft->addEntry(&eCtx, CMPIInvocationFlags,
			(CMPIValue *)&flgs, CMPI_uint32);
	
		::CMPIAssociationMI * mi = m_ftable->miVector.assocMI;
	
		rc=m_ftable->miVector.assocMI->ft->associators(
			mi,&eCtx,&eRes,&eRef, aClass,
			_resultClass, _role, _resultRole, props);
	
		if(rc.rc != CMPI_RC_OK)
		{
			OW_THROWCIMMSG(CIMException::FAILED,
				String(rc.rc).c_str());
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::FAILED,
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
	env->getLogger()->
		logDebug("CMPIAssociatorProviderProxy::references()");

	if(m_ftable->miVector.assocMI->ft->references != NULL)
	{
		char **props = NULL;
		int pCount = 0;

		CMPIStatus rc = {CMPI_RC_OK, NULL};
		::CMPIOperationContext context;
		ProviderEnvironmentIFCRef env2(env);

		m_ftable->broker.hdl = static_cast<void *>(&env2);

		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&(m_ftable->broker), &eCtx);
		// initialize path
		CIMObjectPath objectNameWithNS(objectName);
		objectNameWithNS.setNameSpace(ns);
		CMPI_ObjectPathOnStack eRef(objectNameWithNS);
		CMPI_ResultOnStack eRes(result);
		char *aClass = const_cast<char*>(resultClass.c_str());

		if(propertyList)
		{
			if(propertyList->size() > 0)
			{
				pCount = propertyList->size();
				props = reinterpret_cast<char **>
					(alloca(1+pCount*sizeof(char *)));

				for(int i = 0; i < pCount; i++)
				{
					props[i] = const_cast<char*>((*propertyList)[i].c_str());
				}

				props[pCount] = NULL;
			}
		}

		CMPIFlags flgs = 0;

		if(includeQualifiers)
		{
			flgs |= CMPI_FLAG_IncludeQualifiers;
		}

		if(includeClassOrigin)
		{
			flgs |= CMPI_FLAG_IncludeClassOrigin;
		}

		char *_role = const_cast<char*>(role.empty() ? 0 : role.c_str());
		eCtx.ft->addEntry(&eCtx, CMPIInvocationFlags,
			(CMPIValue *)&flgs, CMPI_uint32);

		::CMPIAssociationMI *mi = m_ftable->miVector.assocMI;
		rc=m_ftable->miVector.assocMI->ft->references(mi, &eCtx, &eRes, &eRef,
			aClass, _role, props);

		if(rc.rc != CMPI_RC_OK)
		{
			OW_THROWCIMMSG(CIMException::FAILED,
				String(rc.rc).c_str());
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::FAILED,
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
	env->getLogger()->
		logDebug("CMPIAssociatorProviderProxy::referenceNames()");

	if(m_ftable->miVector.assocMI->ft->referenceNames != NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};
		::CMPIOperationContext context;

		ProviderEnvironmentIFCRef env2(env);
		m_ftable->broker.hdl = static_cast<void *>(&env2);

		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&(m_ftable->broker), &eCtx);

		// initialize path
		CIMObjectPath objectNameWithNS(objectName);
		objectNameWithNS.setNameSpace(ns);

		CMPI_ObjectPathOnStack eRef(objectNameWithNS);
		CMPI_ResultOnStack eRes(result);
		char* aClass = const_cast<char*>(resultClass.c_str());
		CMPIFlags flgs = 0;
		char* _role = const_cast<char*>(role.empty() ? 0 : role.c_str());
		eCtx.ft->addEntry(&eCtx, CMPIInvocationFlags,
			(CMPIValue *)&flgs, CMPI_uint32);

		::CMPIAssociationMI * mi = m_ftable->miVector.assocMI;
		rc = m_ftable->miVector.assocMI->ft->referenceNames(
			mi, &eCtx, &eRes, &eRef, aClass, _role);

		if(rc.rc != CMPI_RC_OK)
		{
			OW_THROWCIMMSG(CIMException::FAILED, String(rc.rc).c_str());
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::FAILED,
			"Provider does not support referenceNames");
	}
}

} // end namespace OpenWBEM

