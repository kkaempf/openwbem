/*******************************************************************************
* Copyright (C) 2001 Vintela, Inc. All rights reserved.
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
*******************************************************************************/
#include "OW_config.h"
#include "OW_FTABLERef.hpp"
#include "OW_NPIInstanceProviderProxy.hpp"
#include "NPIExternal.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_NPIProviderIFCUtils.hpp"

namespace OpenWBEM
{

// debugging
#define DDD(X) // X
using namespace WBEMFlags;
/////////////////////////////////////////////////////////////////////////////
NPIInstanceProviderProxy::~NPIInstanceProviderProxy() 
{
}
/////////////////////////////////////////////////////////////////////////////
void
NPIInstanceProviderProxy::enumInstanceNames(
		const ProviderEnvironmentIFCRef& env,
		const String& ns,
		const String& className,
		CIMObjectPathResultHandlerIFC& result,
		const CIMClass& cimClass )
{
		env->getLogger()->
			logDebug("NPIInstanceProviderProxy::enumInstanceNames()");
		if (m_ftable->fp_enumInstanceNames!= NULL)
		{
		::NPIHandle _npiHandle = { 0, 0, 0, 0, m_ftable->npicontext};
			NPIHandleFreer nhf(_npiHandle);
			ProviderEnvironmentIFCRef env2(env);
			_npiHandle.thisObject = static_cast<void *>(&env2);
			//  may the arguments must be copied verbatim
			//  to avoid locking problems
			CIMClass cimClass2(cimClass);
			::CIMClass _cc = { static_cast<void *> (&cimClass2)};
			CIMObjectPath cop(className, ns);
			::CIMObjectPath _cop = { static_cast<void *> (&cop)};
			::Vector v =
				m_ftable->fp_enumInstanceNames(&_npiHandle,_cop,true,_cc);
			if (_npiHandle.errorOccurred)
			{
				OW_THROWCIMMSG(CIMException::FAILED,
					_npiHandle.providerError);
			}
			::CIMObjectPath my_cop;
			for (int i=0,n=VectorSize(&_npiHandle,v); i < n; i++)
			{
				my_cop.ptr = _VectorGet(&_npiHandle,v,i);
				CIMObjectPath ow_cop(*
					static_cast<CIMObjectPath *>(my_cop.ptr) );
				ow_cop.setObjectName(cimClass.getName());
		result.handle(ow_cop);
			}
		}
		else
		{
			OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support enumInstanceNames");
		}
}
/////////////////////////////////////////////////////////////////////////////
void
NPIInstanceProviderProxy::enumInstances(
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
	env->getLogger()->
	logDebug("NPIInstanceProviderProxy::enumInstances()");
	if (m_ftable->fp_enumInstances == NULL)
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support enumInstances");
	}
	::NPIHandle _npiHandle = { 0, 0, 0, 0, m_ftable->npicontext};
	NPIHandleFreer nhf(_npiHandle);
	ProviderEnvironmentIFCRef env2(env);
	_npiHandle.thisObject = static_cast<void *>(&env2);
	//  may the arguments must be copied verbatim
	//  to avoid locking problems
	CIMClass cimClass2(cimClass);
	::CIMClass _cc = { static_cast<void *> (&cimClass2)};
	CIMObjectPath cop(className, ns);
	::CIMObjectPath _cop = { static_cast<void *> (&cop)};
	int de = deep;
	int lo = localOnly;
	::Vector v =
	m_ftable->fp_enumInstances(&_npiHandle, _cop, de, _cc, lo);
	if (_npiHandle.errorOccurred)
	{
		OW_THROWCIMMSG(CIMException::FAILED,
		_npiHandle.providerError);
	}
	::CIMInstance my_inst;
	for (int i=0,n=VectorSize(&_npiHandle,v); i < n; i++)
	{
		my_inst.ptr = _VectorGet(&_npiHandle,v,i);
		CIMInstance ow_inst(*
		static_cast<CIMInstance *>(my_inst.ptr) );
		ow_inst.setClassName(cimClass.getName());
		result.handle(ow_inst.clone(localOnly,deep,includeQualifiers,
			includeClassOrigin,propertyList,requestedClass,cimClass));
	}
}
	
/////////////////////////////////////////////////////////////////////////////
CIMInstance
NPIInstanceProviderProxy::getInstance(
	const ProviderEnvironmentIFCRef &env,
	const String& ns,
	const CIMObjectPath& instanceName,
	ELocalOnlyFlag localOnly,
	EIncludeQualifiersFlag includeQualifiers, 
	EIncludeClassOriginFlag includeClassOrigin,
	const StringArray* propertyList, 
	const CIMClass& cimClass)
{
	CIMInstance rval(CIMNULL);
	env->getLogger()->
		logDebug("NPIInstanceProviderProxy::getInstance()");
	if (m_ftable->fp_getInstance != NULL)
	{
		::NPIHandle _npiHandle = { 0, 0, 0, 0, m_ftable->npicontext};
		NPIHandleFreer nhf(_npiHandle);
		ProviderEnvironmentIFCRef env2(env);
		_npiHandle.thisObject = static_cast<void *>(&env2);
		//  may the arguments must be copied verbatim
		//  to avoid locking problems
		CIMClass cimClass2(cimClass);
		::CIMClass _cc = { static_cast<void *> (&cimClass2)};
		CIMObjectPath cop(instanceName);
		cop.setNameSpace(ns);
		::CIMObjectPath _cop = { static_cast<void *> (&cop)};
		int lo = localOnly;
		::CIMInstance my_inst =
			m_ftable->fp_getInstance(&_npiHandle, _cop, _cc, lo);
		if (_npiHandle.errorOccurred)
		{
			OW_THROWCIMMSG(CIMException::FAILED,
				_npiHandle.providerError);
		}
		CIMInstance ow_inst(*
			static_cast<CIMInstance *>(my_inst.ptr));
		ow_inst.setClassName(cimClass.getName());
		rval = ow_inst;
		rval = rval.clone(localOnly,includeQualifiers,includeClassOrigin,propertyList);
	}
	else
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support getInstance");
	}
	return rval;
}
#ifndef OW_DISABLE_INSTANCE_MANIPULATION
/////////////////////////////////////////////////////////////////////////////
CIMObjectPath
NPIInstanceProviderProxy::createInstance(
	const ProviderEnvironmentIFCRef &env, 
	const String& ns,
	const CIMInstance& cimInstance)
{
		CIMObjectPath rval(CIMNULL);
		env->getLogger()->
			logDebug("NPIInstanceProviderProxy::createInstance()");
		if (m_ftable->fp_createInstance != NULL)
		{
		::NPIHandle _npiHandle = { 0, 0, 0, 0, m_ftable->npicontext};
			NPIHandleFreer nhf(_npiHandle);
			ProviderEnvironmentIFCRef env2(env);
			_npiHandle.thisObject = static_cast<void *>(&env2);
			//  may the arguments must be copied verbatim
			//  to avoid locking problems
			CIMInstance cimInstance2(cimInstance);
			::CIMInstance _ci = { static_cast<void *> (&cimInstance2)};
			CIMObjectPath cop(ns, cimInstance);
			::CIMObjectPath _cop = { static_cast<void *> (const_cast<CIMObjectPath*>(&cop))};
			::CIMObjectPath _rcop =
				m_ftable->fp_createInstance(&_npiHandle, _cop, _ci);
			if (_npiHandle.errorOccurred)
			{
				OW_THROWCIMMSG(CIMException::FAILED,
					_npiHandle.providerError);
			}
			rval = *(static_cast<CIMObjectPath *>(_rcop.ptr) );
		}
		else
		{
			OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support createInstance");
		}
		return rval;
}
/////////////////////////////////////////////////////////////////////////////
void
NPIInstanceProviderProxy::modifyInstance(
	const ProviderEnvironmentIFCRef &env,
	const String& ns,
	const CIMInstance& modifiedInstance,
	const CIMInstance& previousInstance,
	EIncludeQualifiersFlag includeQualifiers,
	const StringArray* propertyList,
	const CIMClass& theClass)
{
	env->getLogger()->
	logDebug("NPIInstanceProviderProxy::modifyInstance()");
	if (m_ftable->fp_setInstance != NULL)
	{
		::NPIHandle _npiHandle = { 0, 0, 0, 0, m_ftable->npicontext};
		NPIHandleFreer nhf(_npiHandle);
		ProviderEnvironmentIFCRef env2(env);
		_npiHandle.thisObject = static_cast<void *>(&env2);
		//  may the arguments must be copied verbatim
		//  to avoid locking problems
		CIMInstance newInst(modifiedInstance.createModifiedInstance(
			previousInstance, includeQualifiers, propertyList, theClass));
		::CIMInstance _ci = { static_cast<void *> (&newInst)};
		CIMObjectPath cop(ns, modifiedInstance);
		::CIMObjectPath _cop = { static_cast<void *> (&cop)};
		m_ftable->fp_setInstance(&_npiHandle, _cop, _ci);
		if (_npiHandle.errorOccurred)
		{
			OW_THROWCIMMSG(CIMException::FAILED,
				_npiHandle.providerError);
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support modifyInstance");
	}
}
/////////////////////////////////////////////////////////////////////////////
void
NPIInstanceProviderProxy::deleteInstance(
	const ProviderEnvironmentIFCRef &env,
	const String& ns, const CIMObjectPath& cop)
{
	env->getLogger()->
		logDebug("NPIInstanceProviderProxy::deleteInstance()");
	if (m_ftable->fp_deleteInstance!= NULL)
	{
		::NPIHandle _npiHandle = { 0, 0, 0, 0, m_ftable->npicontext};
		NPIHandleFreer nhf(_npiHandle);
		ProviderEnvironmentIFCRef env2(env);
		_npiHandle.thisObject = static_cast<void *>(&env2);
		//  may the arguments must be copied verbatim
		//  to avoid locking problems
		CIMObjectPath copWithNS(cop);
		copWithNS.setNameSpace(ns);
		::CIMObjectPath _cop = { static_cast<void *> (&copWithNS)};
		m_ftable->fp_deleteInstance(&_npiHandle, _cop);
		if (_npiHandle.errorOccurred)
		{
			OW_THROWCIMMSG(CIMException::FAILED,
				_npiHandle.providerError);
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support deleteInstance");
	}
}
#endif // #ifndef OW_DISABLE_INSTANCE_MANIPULATION

} // end namespace OpenWBEM

