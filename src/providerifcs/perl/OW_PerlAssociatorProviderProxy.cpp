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
#include "OW_PerlAssociatorProviderProxy.hpp"
#include "NPIExternal.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_NPIProviderIFCUtils.hpp"

namespace OpenWBEM
{

using namespace WBEMFlags;
/////////////////////////////////////////////////////////////////////////////
void
PerlAssociatorProviderProxy::associatorNames(
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
		logDebug("PerlAssociatorProviderProxy::associatorNames()");
	if (m_ftable->fp_associatorNames != NULL)
	{
		::NPIHandle _npiHandle = { 0, 0, 0, 0, m_ftable->npicontext};
	NPIHandleFreer nhf(_npiHandle);
	ProviderEnvironmentIFCRef env2(env);
		_npiHandle.thisObject = static_cast<void *>(&env2);
		//  may the arguments must be copied verbatim
		//  to avoid locking problems
		// initialize association class
		CIMObjectPath assocName2(assocClass);
		::CIMObjectPath _assoc = { static_cast<void *> (&assocName2)};
		// initialize path
		CIMObjectPath objectNameWithNS(objectName);
		objectNameWithNS.setNameSpace(ns);
		::CIMObjectPath _path = { static_cast<void *> (&objectNameWithNS)};
		::Vector v =
			m_ftable->fp_associatorNames(&_npiHandle, _assoc, _path,
				resultClass.empty() ? 0 : resultClass.c_str(),
				role.empty() ? 0 : role.c_str(),
				resultRole.empty() ? 0 : resultRole.c_str());
		if (_npiHandle.errorOccurred)
		{
			OW_THROWCIMMSG(CIMException::FAILED, _npiHandle.providerError);
		}
		::CIMObjectPath cop;
		for (int i=::VectorSize(&_npiHandle, v) - 1; i >= 0; i--)
		{
			cop.ptr = ::_VectorGet(&_npiHandle,v,i);
			CIMObjectPath ow_cop(*
				static_cast<CIMObjectPath *>(cop.ptr) );
		result.handle(ow_cop);
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support associatorNames");
	}
}
/////////////////////////////////////////////////////////////////////////////
void
PerlAssociatorProviderProxy::associators(
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
		logDebug("PerlAssociatorProviderProxy::associators()");
	if (m_ftable->fp_associators != NULL)
	{
		::NPIHandle _npiHandle = { 0, 0, 0, 0, m_ftable->npicontext};
	NPIHandleFreer nhf(_npiHandle);
	ProviderEnvironmentIFCRef env2(env);
		_npiHandle.thisObject = static_cast<void *>(&env2);
		//  may the arguments must be copied verbatim
		//  to avoid locking problems
		// initialize association class
	CIMObjectPath assocName2(assocClass);
		::CIMObjectPath _assoc = { static_cast<void *> (&assocName2)};
		// initialize path
	CIMObjectPath objectNameWithNS(objectName);
	objectNameWithNS.setNameSpace(ns);
		::CIMObjectPath _path = { static_cast<void *> (&objectNameWithNS)};
		int _plLen = 0;
		std::vector<const char *> _propertyList;
		if (propertyList)
		{
			_plLen = propertyList->size();
			for (int i = 0; i < _plLen; i++)
				_propertyList.push_back((*propertyList)[i].allocateCString());
		}
		::Vector v =
			m_ftable->fp_associators(&_npiHandle, _assoc, _path,
				resultClass.empty() ? 0 : resultClass.c_str(),
				role.empty() ? 0 : role.c_str(),
				resultRole.empty() ? 0 : resultRole.c_str(),
				includeQualifiers, includeClassOrigin,
				_plLen > 0 ? &_propertyList[0] : 0, _plLen);
		// free the strings in _propertyList
		for (std::vector<const char*>::iterator i = _propertyList.begin();
			 i != _propertyList.end(); ++i)
		{
			free(const_cast<void*>(static_cast<const void*>(*i)));
		}
		if (_npiHandle.errorOccurred)
		{
			OW_THROWCIMMSG(CIMException::FAILED, _npiHandle.providerError);
		}
		int n = ::VectorSize(&_npiHandle,v);
		env->getLogger()->logDebug(Format("PerlAssociatorProviderProxy::"
			"associators() got %1 associator instances", n - 1));
		::CIMInstance my_inst;
		for (int i=0; i < n; i++)
		{
			my_inst.ptr = _VectorGet(&_npiHandle,v,i);
			CIMInstance ow_inst(*
				static_cast<CIMInstance *>(my_inst.ptr) );
		result.handle(ow_inst);
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support associators");
	}
}
/////////////////////////////////////////////////////////////////////////////
void
PerlAssociatorProviderProxy::references(
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
	env->getLogger()->logDebug("PerlAssociatorProviderProxy::references()");
	if (m_ftable->fp_references != NULL)
	{
		::NPIHandle _npiHandle = { 0, 0, 0, 0, m_ftable->npicontext};
	NPIHandleFreer nhf(_npiHandle);
	ProviderEnvironmentIFCRef env2(env);
		_npiHandle.thisObject = static_cast<void *>(&env2);
		//  may the arguments must be copied verbatim
		//  to avoid locking problems
		// initialize association class
	CIMObjectPath assocName2(resultClass);
		::CIMObjectPath _assoc = { static_cast<void *> (&assocName2)};
		// initialize path
	CIMObjectPath objectNameWithNS(objectName);
	objectNameWithNS.setNameSpace(ns);
		::CIMObjectPath _path = { static_cast<void *> (&objectNameWithNS)};
		int _plLen = 0;
		std::vector<const char *> _propertyList;
		if (propertyList)
		{
			_plLen = propertyList->size();
			for (int i = 0; i < _plLen; i++)
				_propertyList.push_back((*propertyList)[i].allocateCString());
		}
		::Vector v =
			m_ftable->fp_references(&_npiHandle, _assoc, _path,
				role.empty() ? 0 : role.c_str(),
				includeQualifiers, includeClassOrigin,
				_plLen > 0 ? &_propertyList[0] : 0, _plLen);
		// free the strings in _propertyList
		for (std::vector<const char*>::iterator i = _propertyList.begin();
			 i != _propertyList.end(); ++i)
		{
			free(const_cast<void*>(static_cast<const void*>(*i)));
		}
		if (_npiHandle.errorOccurred)
		{
			OW_THROWCIMMSG(CIMException::FAILED, _npiHandle.providerError);
		}
		int n = ::VectorSize(&_npiHandle,v);
		env->getLogger()->logDebug(Format("PerlAssociatorProviderProxy::"
			"references() got %1 associator instances", n - 1));
		::CIMInstance my_inst;
		for (int i=0; i < n; i++)
		{
			my_inst.ptr = _VectorGet(&_npiHandle,v,i);
			CIMInstance ow_inst(*
				static_cast<CIMInstance *>(my_inst.ptr) );
		result.handle(ow_inst);
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support references");
	}
}
/////////////////////////////////////////////////////////////////////////////
void
PerlAssociatorProviderProxy::referenceNames(
		const ProviderEnvironmentIFCRef &env,
		CIMObjectPathResultHandlerIFC& result,
		const String& ns,
		const CIMObjectPath& objectName,
		const String& resultClass,
		const String& role)
{
	env->getLogger()->
		logDebug("PerlAssociatorProviderProxy::referenceNames()");
	if (m_ftable->fp_referenceNames != NULL)
	{
		::NPIHandle _npiHandle = { 0, 0, 0, 0, m_ftable->npicontext};
	NPIHandleFreer nhf(_npiHandle);
	ProviderEnvironmentIFCRef env2(env);
		_npiHandle.thisObject = static_cast<void *>(&env2);
		//  may the arguments must be copied verbatim
		//  to avoid locking problems
		// initialize association class
	CIMObjectPath assocName2(resultClass);
		::CIMObjectPath _assoc = { static_cast<void *> (&assocName2)};
		// initialize path
	CIMObjectPath objectNameWithNS(objectName);
	objectNameWithNS.setNameSpace(ns);
		::CIMObjectPath _path = { static_cast<void *> (&objectNameWithNS)};
		::Vector v =
			m_ftable->fp_referenceNames(&_npiHandle, _assoc, _path,
					role.empty() ? 0 : role.c_str());
		if (_npiHandle.errorOccurred)
		{
			OW_THROWCIMMSG(CIMException::FAILED, _npiHandle.providerError);
		}
		::CIMObjectPath my_cop;
		for (int i=::VectorSize(&_npiHandle, v) - 1; i >= 0; i--)
		{
			my_cop.ptr = _VectorGet(&_npiHandle,v,i);
			CIMObjectPath ow_cop(*
				static_cast<CIMObjectPath*>(my_cop.ptr) );
		result.handle(ow_cop);
		}
	}
	else
	{
		OW_THROWCIMMSG(CIMException::FAILED, "Provider does not support referenceNames");
	}
}

} // end namespace OpenWBEM

