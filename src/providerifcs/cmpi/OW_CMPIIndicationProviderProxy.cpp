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

#include "OW_config.h"
#include "OW_CMPIIndicationProviderProxy.hpp"
#include "CMPIExternal.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_CMPIProviderIFCUtils.hpp"
#include "OW_WQLSelectStatement.hpp"

/////////////////////////////////////////////////////////////////////////////
void
OW_CMPIIndicationProviderProxy::deActivateFilter(
	const OW_ProviderEnvironmentIFCRef &env, 
	const OW_WQLSelectStatement &filter, 
	const OW_String &eventType, 
	const OW_CIMObjectPath &classPath, 
	bool lastActivation)
{
	env->getLogger()->logDebug("deactivateFilter");
	if (m_ftable->fp_deActivateFilter != NULL)
	{
		::CMPIHandle _npiHandle = { 0, 0, 0, 0, NULL};
		OW_CMPIHandleFreer nhf(_npiHandle);

		env->getLogger()->logDebug("deactivateFilter");

		_npiHandle.thisObject = (void *) static_cast<const void *>(&env);

		OW_WQLSelectStatement mutableFilter(filter);
		OW_CIMObjectPath mutablePath(classPath);
		SelectExp exp = {&mutableFilter};
		CIMObjectPath cop = {&mutablePath};

		m_ftable->fp_deActivateFilter( &_npiHandle, exp, eventType.c_str(), cop, lastActivation);

		if (_npiHandle.errorOccurred)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
			_npiHandle.providerError);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void
OW_CMPIIndicationProviderProxy::activateFilter(
	const OW_ProviderEnvironmentIFCRef &env, 
	const OW_WQLSelectStatement &filter, 
	const OW_String &eventType, 
	const OW_CIMObjectPath &classPath, 
	bool firstActivation)
{
	env->getLogger()->logDebug("activateFilter");
	if (m_ftable->fp_activateFilter != NULL)
	{
		env->getLogger()->logDebug("activateFilter2");
		::CMPIHandle _npiHandle = { 0, 0, 0, 0, NULL};
		OW_CMPIHandleFreer nhf(_npiHandle);

		_npiHandle.thisObject = (void *) static_cast<const void *>(&env);

		OW_WQLSelectStatement mutableFilter(filter);
		OW_CIMObjectPath mutablePath(classPath);
		SelectExp exp = {&mutableFilter};
		CIMObjectPath cop = {&mutablePath};

		m_ftable->fp_activateFilter( &_npiHandle, exp, eventType.c_str(), cop, firstActivation);

		if (_npiHandle.errorOccurred)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
			_npiHandle.providerError);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void
OW_CMPIIndicationProviderProxy::authorizeFilter(
	const OW_ProviderEnvironmentIFCRef &env, 
	const OW_WQLSelectStatement &filter, 
	const OW_String &eventType, 
	const OW_CIMObjectPath &classPath, 
	const OW_String &owner)
{
	env->getLogger()->logDebug("deactivateFilter");
	if (m_ftable->fp_deActivateFilter != NULL)
	{
		::CMPIHandle _npiHandle = { 0, 0, 0, 0, NULL};
		OW_CMPIHandleFreer nhf(_npiHandle);

		env->getLogger()->logDebug("deactivateFilter2");

		_npiHandle.thisObject = (void *) static_cast<const void *>(&env);

		OW_WQLSelectStatement mutableFilter(filter);
		OW_CIMObjectPath mutablePath(classPath);
		SelectExp exp = {&mutableFilter};
		CIMObjectPath cop = {&mutablePath};

		m_ftable->fp_authorizeFilter( &_npiHandle, exp, eventType.c_str(), cop, owner.c_str());

		if (_npiHandle.errorOccurred)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
			_npiHandle.providerError);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
int
OW_CMPIIndicationProviderProxy::mustPoll(
	const OW_ProviderEnvironmentIFCRef &env, 
	const OW_WQLSelectStatement &filter, 
	const OW_String &eventType, 
	const OW_CIMObjectPath &classPath)
{
	env->getLogger()->logDebug("mustPoll");
	if (m_ftable->fp_mustPoll != NULL)
	{
		::CMPIHandle _npiHandle = { 0, 0, 0, 0, NULL};
		OW_CMPIHandleFreer nhf(_npiHandle);

		env->getLogger()->logDebug("mustPoll2");

		_npiHandle.thisObject = (void *) static_cast<const void *>(&env);

		OW_WQLSelectStatement mutableFilter(filter);
		OW_CIMObjectPath mutablePath(classPath);
		SelectExp exp = {&mutableFilter};
		CIMObjectPath cop = {&mutablePath};

		int rval = m_ftable->fp_mustPoll( &_npiHandle, exp, eventType.c_str(), cop);

		if (_npiHandle.errorOccurred)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
			_npiHandle.providerError);
		}
		// CMPI providers only return 0 or 1 for mustPoll.
		// OpenWBEM expects mustPoll to return the polling interval,
		// so we'll return a reasonable polling interval (5 mins.)  
		if (rval > 0)
		{
			return 5 * 60;
		}
	}
	return 0;
}

