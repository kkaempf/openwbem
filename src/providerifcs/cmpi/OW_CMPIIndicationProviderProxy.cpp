/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc, IBM Corp. All rights reserved.
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
#include "OW_CMPIIndicationProviderProxy.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_CMPIProviderIFCUtils.hpp"
#include "OW_WQLSelectStatement.hpp"
#include "cmpisrv.h"

/////////////////////////////////////////////////////////////////////////////
void
OW_CMPIIndicationProviderProxy::deActivateFilter(
	const OW_ProviderEnvironmentIFCRef &env, 
	const OW_WQLSelectStatement &filter,
	const OW_String &eventType,
	const OW_String& nameSpace,
	const OW_StringArray& classes)
{
	bool lastActivation = (--m_activationCount == 0);

	env->getLogger()->logDebug("deactivateFilter");

	if (m_ftable->miVector.indMI->ft->deActivateFilter != NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};

		::OperationContext context;

		OW_ProviderEnvironmentIFCRef env2(env);
		m_ftable->broker.hdl = static_cast<void *>(&env2);

		CMPI_ResultOnStack eRes;

		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&(m_ftable->broker), &eCtx);

		OW_WQLSelectStatement mutableFilter(filter);

		OW_CIMObjectPath mutablePath;
		mutablePath.setNameSpace(nameSpace);
		if (!classes.empty())
			mutablePath.setObjectName(classes[0]);

		CMPI_ObjectPathOnStack eRef(mutablePath);

		CMPISelectExp exp; // = {&mutableFilter};

		//CMPIFlags flgs = 0;

		::CMPIIndicationMI * mi = m_ftable->miVector.indMI;

		char * _eventType = eventType.allocateCString();
		rc = m_ftable->miVector.indMI->ft->deActivateFilter(
			mi, &eCtx, &eRes,
			&exp, 
		 	_eventType, &eRef, lastActivation);

		if (_eventType) delete _eventType;

		if (rc.rc == CMPI_RC_OK) return;
		else
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				 rc.msg ? CMGetCharPtr(rc.msg) : "");
		}
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
			"Provider does not support deactivateFilter");
	}
}

/////////////////////////////////////////////////////////////////////////////
void
OW_CMPIIndicationProviderProxy::activateFilter(
	const OW_ProviderEnvironmentIFCRef &env, 
	const OW_WQLSelectStatement &filter,
	const OW_String &eventType,
	const OW_String& nameSpace,
	const OW_StringArray& classes)
{
	bool firstActivation = (m_activationCount++ == 0);

	env->getLogger()->logDebug("activateFilter");

	if (m_ftable->miVector.indMI->ft->activateFilter != NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};

		::OperationContext context;

		OW_ProviderEnvironmentIFCRef env2(env);
		m_ftable->broker.hdl = static_cast<void *>(&env2);

		CMPI_ResultOnStack eRes;

		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&(m_ftable->broker), &eCtx);

		OW_WQLSelectStatement mutableFilter(filter);

		OW_CIMObjectPath mutablePath;
		mutablePath.setNameSpace(nameSpace);
		if (!classes.empty())
			mutablePath.setObjectName(classes[0]);

		CMPI_ObjectPathOnStack eRef(mutablePath);

		CMPISelectExp exp; // = {&mutableFilter};

		//CMPIFlags flgs = 0;

		::CMPIIndicationMI * mi = m_ftable->miVector.indMI;

		char * _eventType = eventType.allocateCString();
		rc = m_ftable->miVector.indMI->ft->activateFilter(
			mi, &eCtx, &eRes,
			&exp, 
		 	_eventType, &eRef, firstActivation);

		if (_eventType) delete _eventType;

		if (rc.rc == CMPI_RC_OK) return;
		else
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				 rc.msg ? CMGetCharPtr(rc.msg) : "");
		}
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
			"Provider does not support activateFilter");
	}
}

/////////////////////////////////////////////////////////////////////////////
void
OW_CMPIIndicationProviderProxy::authorizeFilter(
	const OW_ProviderEnvironmentIFCRef &env, 
	const OW_WQLSelectStatement &filter, 
	const OW_String &eventType, 
	const OW_String &nameSpace, 
	const OW_StringArray &classes, 
	const OW_String &owner)
{
	env->getLogger()->logDebug("authorizeFilter");

	if (m_ftable->miVector.indMI->ft->authorizeFilter != NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};

		::OperationContext context;

		OW_ProviderEnvironmentIFCRef env2(env);
		m_ftable->broker.hdl = static_cast<void *>(&env2);

		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&(m_ftable->broker), &eCtx);

		CMPI_ResultOnStack eRes;

		OW_WQLSelectStatement mutableFilter(filter);

		OW_CIMObjectPath mutablePath;
		mutablePath.setNameSpace(nameSpace);
		if (!classes.empty())
			mutablePath.setObjectName(classes[0]);

		CMPI_ObjectPathOnStack eRef(mutablePath);

		CMPISelectExp exp; // = {&mutableFilter};

		//CMPIFlags flgs = 0;

		::CMPIIndicationMI * mi = m_ftable->miVector.indMI;

		char * _eventType = eventType.allocateCString();
		char * _owner = owner.allocateCString();

		rc = m_ftable->miVector.indMI->ft->authorizeFilter(
			mi, &eCtx, &eRes,
			&exp, _eventType, &eRef, _owner);

		if (_eventType) delete _eventType;
		if (_owner) delete _owner;

		if (rc.rc == CMPI_RC_OK) return;
		else
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				 rc.msg ? CMGetCharPtr(rc.msg) : "");
		}
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
			"Provider does not support authorizeFilter");
	}
}


/////////////////////////////////////////////////////////////////////////////
int
OW_CMPIIndicationProviderProxy::mustPoll(
	const OW_ProviderEnvironmentIFCRef &env, 
	const OW_WQLSelectStatement &filter, 
	const OW_String &eventType, 
	const OW_String &nameSpace, 
	const OW_StringArray &classes)
{
	env->getLogger()->logDebug("mustPoll");

	if (m_ftable->miVector.indMI->ft->mustPoll != NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};

		::OperationContext context;

		OW_ProviderEnvironmentIFCRef env2(env);
		m_ftable->broker.hdl = static_cast<void *>(&env2);

		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&(m_ftable->broker), &eCtx);

		CMPI_ResultOnStack eRes;

		OW_WQLSelectStatement mutableFilter(filter);

		OW_CIMObjectPath mutablePath;
		mutablePath.setNameSpace(nameSpace);
		if (!classes.empty())
			mutablePath.setObjectName(classes[0]);

		CMPI_ObjectPathOnStack eRef(mutablePath);

		CMPISelectExp exp; // = {&mutableFilter};

		//CMPIFlags flgs = 0;

		::CMPIIndicationMI * mi = m_ftable->miVector.indMI;

		char * _eventType = eventType.allocateCString();

		rc = m_ftable->miVector.indMI->ft->mustPoll(
			mi, &eCtx, &eRes,
			&exp, _eventType, &eRef);

		if (_eventType) delete _eventType;

		if (rc.rc == CMPI_RC_OK) return 5*60;
		else
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				 rc.msg ? CMGetCharPtr(rc.msg) : "");
		}
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
			"Provider does not support mustPoll");
	}
}

