/*******************************************************************************
* Copyright (C) 2001 Center 7, Inc All rights reserved.
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
*******************************************************************************/

#include "OW_config.h"
#include "OW_CMPIAssociatorProviderProxy.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_CMPIProviderIFCUtils.hpp"
#include "cmpisrv.h"

#include <alloca.h>

/////////////////////////////////////////////////////////////////////////////
void
OW_CMPIAssociatorProviderProxy::associatorNames(
	const OW_ProviderEnvironmentIFCRef &env,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String& ns,
	const OW_CIMObjectPath& objectName,
	const OW_String& assocClass,
	const OW_String& resultClass,
	const OW_String& role,
	const OW_String& resultRole)
{
	env->getLogger()->
		logDebug("OW_CMPIAssociatorProviderProxy::associatorNames()");

	if (m_ftable->miVector.assocMI->ft->associatorNames != NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};

		::OperationContext context;

		OW_ProviderEnvironmentIFCRef env2(env);
		m_ftable->broker.hdl = static_cast<void *>(&env2);
                                                                                
		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&(m_ftable->broker), &eCtx);
                                                                                
		// initialize path
		OW_CIMObjectPath objectNameWithNS(objectName);
		objectNameWithNS.setNameSpace(ns);
		CMPI_ObjectPathOnStack eRef(objectNameWithNS);

		CMPI_ResultOnStack eRes(result);

		char * aClass = assocClass.allocateCString();
		CMPIFlags flgs = 0;

		char * _resultClass = resultClass.empty() ? 0 :
		resultClass.allocateCString();
		char * _role = role.empty() ? 0 : role.allocateCString();
		char * _resultRole = resultRole.empty() ? 0 :
		resultRole.allocateCString();

		eCtx.ft->addEntry(&eCtx, CMPIInvocationFlags,
			 (CMPIValue *)&flgs, CMPI_uint32);

		::CMPIAssociationMI * mi = m_ftable->miVector.assocMI;

		rc=m_ftable->miVector.assocMI->ft->associatorNames(
			mi,&eCtx,&eRes,&eRef, aClass,
			_resultClass, _role, _resultRole);

		if (aClass) delete aClass;
		if (_role) delete _role;
		if (_resultClass) delete _resultClass;
		if (_resultRole) delete _resultRole;

        	if (rc.rc != CMPI_RC_OK)
        	{
            		OW_THROWCIMMSG(OW_CIMException::FAILED,
					 OW_String(rc.rc).c_str());
        	}

	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
			 "Provider does not support associatorNames");
	}
}

/////////////////////////////////////////////////////////////////////////////
void
OW_CMPIAssociatorProviderProxy::associators(
		const OW_ProviderEnvironmentIFCRef &env,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String& ns,
		const OW_CIMObjectPath& objectName,
		const OW_String& assocClass,
		const OW_String& resultClass,
		const OW_String& role,
		const OW_String& resultRole,
		const OW_Bool& includeQualifiers,
		const OW_Bool& includeClassOrigin,
		const OW_StringArray *propertyList)
{
	env->getLogger()->
		logDebug("OW_CMPIAssociatorProviderProxy::associators()");

	if (m_ftable->miVector.assocMI->ft->associators != NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};
		char **props = NULL;
		int pCount = 0;

		::OperationContext context;

		OW_ProviderEnvironmentIFCRef env2(env);
		m_ftable->broker.hdl = static_cast<void *>(&env2);
                                                                                
		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&(m_ftable->broker), &eCtx);

		// initialize path
		OW_CIMObjectPath objectNameWithNS(objectName);
		objectNameWithNS.setNameSpace(ns);
		CMPI_ObjectPathOnStack eRef(objectNameWithNS);

		CMPI_ResultOnStack eRes(result);

		char * aClass = assocClass.allocateCString();

		if (propertyList)
		{
			if (propertyList->size()>0)
			{
				pCount = propertyList->size();
				props = (char **)
					alloca(1+pCount*sizeof(char *));
				for(int i=0;i<pCount;i++)
					props[i]= (*propertyList)[i].
							allocateCString();
				props[pCount]=NULL;
 			}
		}

		CMPIFlags flgs = 0;
		if (includeQualifiers) flgs|=CMPI_FLAG_IncludeQualifiers;
		if (includeClassOrigin) flgs|=CMPI_FLAG_IncludeClassOrigin;

		char * _resultClass = resultClass.empty() ? 0 :
		resultClass.allocateCString();
		char * _role = role.empty() ? 0 : role.allocateCString();
		char * _resultRole = resultRole.empty() ? 0 :
		resultRole.allocateCString();

		eCtx.ft->addEntry(&eCtx, CMPIInvocationFlags,
				 (CMPIValue *)&flgs, CMPI_uint32);

		::CMPIAssociationMI * mi = m_ftable->miVector.assocMI;

		rc=m_ftable->miVector.assocMI->ft->associators(
			mi,&eCtx,&eRes,&eRef, aClass,
			_resultClass, _role, _resultRole, props);

		if (aClass) delete aClass;
		if (_role) delete _role;
		if (_resultClass) delete _resultClass;
		if (_resultRole) delete _resultRole;

		if (props && pCount)
			for (int i=0;i<pCount;i++) free((char *)props[i]);

        	if (rc.rc == CMPI_RC_OK) return;
		else
        	{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
					 OW_String(rc.rc).c_str());
        	}
	}
	else
	{
	OW_THROWCIMMSG(OW_CIMException::FAILED,
		"Provider does not support associators");
	}
}

/////////////////////////////////////////////////////////////////////////////
void
OW_CMPIAssociatorProviderProxy::references(
		const OW_ProviderEnvironmentIFCRef &env,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String& ns,
		const OW_CIMObjectPath& objectName,
		const OW_String& resultClass,
		const OW_String& role,
		const OW_Bool& includeQualifiers,
		const OW_Bool& includeClassOrigin,
		const OW_StringArray *propertyList)
{
	env->getLogger()->
		logDebug("OW_CMPIAssociatorProviderProxy::references()");

	if (m_ftable->miVector.assocMI->ft->references != NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};
		char **props = NULL;
		int pCount = 0;

		::OperationContext context;

		OW_ProviderEnvironmentIFCRef env2(env);
		m_ftable->broker.hdl = static_cast<void *>(&env2);
                                                                                
		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&(m_ftable->broker), &eCtx);

		// initialize path
		OW_CIMObjectPath objectNameWithNS(objectName);
		objectNameWithNS.setNameSpace(ns);
		CMPI_ObjectPathOnStack eRef(objectNameWithNS);

		CMPI_ResultOnStack eRes(result);

		char * aClass = resultClass.allocateCString();

		if (propertyList)
		{
			if (propertyList->size()>0)
			{
				pCount = propertyList->size();
				props = (char **)
					alloca(1+pCount*sizeof(char *));
				for(int i=0;i<pCount;i++)
					props[i]= (*propertyList)[i].
						allocateCString();
				props[pCount]=NULL;
 			}
		}

		CMPIFlags flgs = 0;
		if (includeQualifiers) flgs|=CMPI_FLAG_IncludeQualifiers;
		if (includeClassOrigin) flgs|=CMPI_FLAG_IncludeClassOrigin;

		char * _role = role.empty() ? 0 : role.allocateCString();

		eCtx.ft->addEntry(&eCtx, CMPIInvocationFlags,
			 (CMPIValue *)&flgs, CMPI_uint32);

		::CMPIAssociationMI * mi = m_ftable->miVector.assocMI;

		rc=m_ftable->miVector.assocMI->ft->references(
			mi,&eCtx,&eRes,&eRef, aClass, _role, props);

		if (aClass) delete aClass;
		if (_role) delete _role;

		if (props && pCount)
			for (int i=0;i<pCount;i++) free((char *)props[i]);

        	if (rc.rc == CMPI_RC_OK) return;
		else
        	{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
					 OW_String(rc.rc).c_str());
        	}
	}
	else
	{
	OW_THROWCIMMSG(OW_CIMException::FAILED,
		"Provider does not support references");
	}
}

/////////////////////////////////////////////////////////////////////////////
void
OW_CMPIAssociatorProviderProxy::referenceNames(
		const OW_ProviderEnvironmentIFCRef &env,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String& ns,
		const OW_CIMObjectPath& objectName,
		const OW_String& resultClass,
		const OW_String& role)
{
	env->getLogger()->
		logDebug("OW_CMPIAssociatorProviderProxy::referenceNames()");

	if (m_ftable->miVector.assocMI->ft->referenceNames != NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};

		::OperationContext context;

		OW_ProviderEnvironmentIFCRef env2(env);
		m_ftable->broker.hdl = static_cast<void *>(&env2);
                                                                                
		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&(m_ftable->broker), &eCtx);

		// initialize path
		OW_CIMObjectPath objectNameWithNS(objectName);
		objectNameWithNS.setNameSpace(ns);
		CMPI_ObjectPathOnStack eRef(objectNameWithNS);

		CMPI_ResultOnStack eRes(result);

		char * aClass = resultClass.allocateCString();
		CMPIFlags flgs = 0;

		char * _role = role.empty() ? 0 : role.allocateCString();

		eCtx.ft->addEntry(&eCtx, CMPIInvocationFlags,
			 (CMPIValue *)&flgs, CMPI_uint32);

		::CMPIAssociationMI * mi = m_ftable->miVector.assocMI;

		rc=m_ftable->miVector.assocMI->ft->referenceNames(
			mi,&eCtx,&eRes,&eRef, aClass, _role);

		if (aClass) delete aClass;
		if (_role) delete _role;

        	if (rc.rc != CMPI_RC_OK) return;
		else
        	{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
					 OW_String(rc.rc).c_str());
        	}
	}
	else
	{
	OW_THROWCIMMSG(OW_CIMException::FAILED,
		"Provider does not support referenceNames");
	}
}

