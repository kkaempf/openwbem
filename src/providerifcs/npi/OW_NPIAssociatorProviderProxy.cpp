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
#include "OW_NPIAssociatorProviderProxy.hpp"
#include "NPIExternal.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_NPIProviderIFCUtils.hpp"

/////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_NPIAssociatorProviderProxy::associatorNames(
    const OW_ProviderEnvironmentIFCRef &env, OW_CIMObjectPath assocName,
    OW_CIMObjectPath objectName, OW_String resultClass, OW_String role,
    OW_String resultRole)
{
	OW_CIMObjectPathEnumeration rval;

    env->getLogger()->logDebug("OW_NPIAssociatorProviderProxy::associators()");

    if (m_ftable->fp_associatorNames != NULL)
	{
		::NPIHandle _npiHandle = { 0, 0, 0, 0 };
		_npiHandle.thisObject = 0; // fix this (void *)createEnv(_repository, nameSpace);

        // initialize association class
        ::CIMObjectPath _assoc = { static_cast<void *>(assocName.toBlob()) };
		OW_BlobFreer bf1(static_cast<OW_Blob*>(_assoc.ptr));

		// initialize path
        ::CIMObjectPath _path = { static_cast<void *>(objectName.toBlob()) };
		OW_BlobFreer bf2(static_cast<OW_Blob*>(_path.ptr));

        ::Vector v =
            m_ftable->fp_associatorNames(&_npiHandle, _assoc, _path,
				resultClass.c_str(), role.c_str(), resultRole.c_str());

		OW_NPIVectorFreer vf1(v);

		if (_npiHandle.errorOccurred)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, _npiHandle.providerError);
		}

        ::CIMObjectPath cop;
        for (int i=::VectorSize(&_npiHandle, v) - 1; i >= 0; i--)
		{
            cop.ptr = ::_VectorGet(&_npiHandle,v,i);
            OW_CIMObjectPath mycop;
			mycop.fromBlob(static_cast<OW_Blob*>(cop.ptr));
			rval.addElement(mycop);
        }
    }
    else
	{
        OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
    }
	return rval;
}

/////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_NPIAssociatorProviderProxy::associators(
    const OW_ProviderEnvironmentIFCRef &env, OW_CIMObjectPath assocName,
    OW_CIMObjectPath objectName, OW_String resultClass, OW_String role,
    OW_String resultRole, OW_Bool includeQualifiers,
    OW_Bool includeClassOrigin, const OW_StringArray *propertyList)
{
	OW_CIMInstanceEnumeration rval;

    env->getLogger()->logDebug("OW_NPIAssociatorProviderProxy::associators()");

    if (m_ftable->fp_associators != NULL)
	{
		::NPIHandle _npiHandle = { 0, 0, 0, 0 };
		_npiHandle.thisObject = 0; // fix this (void *)createEnv(_repository, nameSpace);

        // initialize association class
        ::CIMObjectPath _assoc = { static_cast<void *>(assocName.toBlob()) };
		OW_BlobFreer bf1(static_cast<OW_Blob*>(_assoc.ptr));

		// initialize path
        ::CIMObjectPath _path = { static_cast<void *>(objectName.toBlob()) };
		OW_BlobFreer bf2(static_cast<OW_Blob*>(_path.ptr));

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
                resultClass.c_str(), role.c_str(), resultRole.c_str(),
                includeQualifiers, includeClassOrigin,
				_plLen > 0 ? &_propertyList[0] : 0, _plLen);

		// free the strings in _propertyList
		for (std::vector<const char*>::iterator i = _propertyList.begin();
			 i != _propertyList.end(); ++i)
		{
			free((void*)(*i));
		}

		OW_NPIVectorFreer vf1(v);

		if (_npiHandle.errorOccurred)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, _npiHandle.providerError);
		}

        int n = ::VectorSize(&_npiHandle,v);
        env->getLogger()->logDebug(format("OW_NPIAssociatorProviderProxy::"
			"associators() got %1 associator instances", n - 1));

        ::CIMInstance npici;
        for (int i=0; i < n; i++)
		{
            npici.ptr = _VectorGet(&_npiHandle,v,i);
			OW_CIMInstance ci;
			ci.fromBlob(static_cast<OW_Blob*>(npici.ptr));
			rval.addElement(ci);
        }
		
    }
    else
	{
		OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
    }


	return rval;
}

/////////////////////////////////////////////////////////////////////////////
OW_CIMInstanceEnumeration
OW_NPIAssociatorProviderProxy::references(
    const OW_ProviderEnvironmentIFCRef &env, OW_CIMObjectPath assocName,
    OW_CIMObjectPath objectName, OW_String role, OW_Bool includeQualifiers,
    OW_Bool includeClassOrigin, const OW_StringArray *propertyList)
{
	OW_CIMInstanceEnumeration rval;

    env->getLogger()->logDebug("OW_NPIAssociatorProviderProxy::associators()");

    if (m_ftable->fp_references != NULL)
	{
		::NPIHandle _npiHandle = { 0, 0, 0, 0 };
		_npiHandle.thisObject = 0; // fix this (void *)createEnv(_repository, nameSpace);

        // initialize association class
        ::CIMObjectPath _assoc = { (void *)assocName.toBlob() };
		OW_BlobFreer bf1(static_cast<OW_Blob*>(_assoc.ptr));

		// initialize path
        ::CIMObjectPath _path = {(void *)objectName.toBlob() };
		OW_BlobFreer bf2(static_cast<OW_Blob*>(_path.ptr));

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
                role.c_str(), includeQualifiers, includeClassOrigin,
				_plLen > 0 ? &_propertyList[0] : 0, _plLen);

		// free the strings in _propertyList
		for (std::vector<const char*>::iterator i = _propertyList.begin();
			 i != _propertyList.end(); ++i)
		{
			free((void*)(*i));
		}

		OW_NPIVectorFreer vf1(v);

		if (_npiHandle.errorOccurred)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, _npiHandle.providerError);
		}

        int n = ::VectorSize(&_npiHandle,v);
        env->getLogger()->logDebug(format("OW_NPIAssociatorProviderProxy::"
			"associators() got %1 associator instances", n - 1));

        ::CIMInstance npici;
        for (int i=0; i < n; i++)
		{
            npici.ptr = _VectorGet(&_npiHandle,v,i);
			OW_CIMInstance ci;
			ci.fromBlob(static_cast<OW_Blob*>(npici.ptr));
			rval.addElement(ci);
        }
		
    }
    else
	{
		OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
    }


	return rval;
}

/////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_NPIAssociatorProviderProxy::referenceNames(
    const OW_ProviderEnvironmentIFCRef &env, OW_CIMObjectPath assocName,
    OW_CIMObjectPath objectName, OW_String role)
{
	OW_CIMObjectPathEnumeration rval;

    env->getLogger()->logDebug("OW_NPIAssociatorProviderProxy::associators()");

    if (m_ftable->fp_referenceNames != NULL)
	{
		::NPIHandle _npiHandle = { 0, 0, 0, 0 };
		_npiHandle.thisObject = 0; // fix this (void *)createEnv(_repository, nameSpace);

        // initialize association class
        ::CIMObjectPath _assoc = { static_cast<void *>(assocName.toBlob()) };
		OW_BlobFreer bf1(static_cast<OW_Blob*>(_assoc.ptr));

		// initialize path
        ::CIMObjectPath _path = { static_cast<void *>(objectName.toBlob()) };
		OW_BlobFreer bf2(static_cast<OW_Blob*>(_path.ptr));

        ::Vector v =
            m_ftable->fp_referenceNames(&_npiHandle, _assoc, _path,
				role.c_str());

		OW_NPIVectorFreer vf1(v);

		if (_npiHandle.errorOccurred)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED, _npiHandle.providerError);
		}

        ::CIMObjectPath cop;
        for (int i=::VectorSize(&_npiHandle, v) - 1; i >= 0; i--)
		{
            cop.ptr = ::_VectorGet(&_npiHandle,v,i);
            OW_CIMObjectPath mycop;
			mycop.fromBlob(static_cast<OW_Blob*>(cop.ptr));
			rval.addElement(mycop);
        }
    }
    else
	{
        OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
    }
	return rval;
}

