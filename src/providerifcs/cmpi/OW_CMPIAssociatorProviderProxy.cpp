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
#include "OW_CMPIAssociatorProviderProxy.hpp"
#include "CMPIExternal.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_CMPIProviderIFCUtils.hpp"

/////////////////////////////////////////////////////////////////////////////
void
OW_CMPIAssociatorProviderProxy::associatorNames(
	const OW_ProviderEnvironmentIFCRef &env,
	const OW_String& ns,
	const OW_CIMObjectPath& assocName,
	const OW_CIMObjectPath& objectName,
	OW_CIMObjectPathResultHandlerIFC& result,
	const OW_String& resultClass,
	const OW_String& role,
	const OW_String& resultRole)
{
    env->getLogger()->
        logDebug("OW_CMPIAssociatorProviderProxy::associatorNames()");

    if (m_ftable->ft->associatorNames != NULL)
    {
	CMPI_ThreadContext thr;
	CMPIStatus rc;

	CMPI_ContextOnStack eCtx(context);

        // initialize path
	OW_CIMObjectPath objectNameWithNS(objectName);
	objectNameWithNS.setNameSpace(ns);
	CMPI_ObjectPathOnStack eRef(objectNameWithNS);

	CMPI_ResultOnStack eRes(result);

	char * aClass = assocClass.getObjectName().allocateCString();
	CMPI_Flags flgs = 0;

	eCtx.ft->addEntry(&eCtx, CMPIInvocationFlags, (CMPIValue)&flgs,
		CMPI_unit32);

	CMPIAssociationMI * mi = (CMPIAssociationMI *)(cProv);

	rc=mi->ft->associatorNames(mi,&eCtx,&eRes,&eRef, aClass,
                resultClass.empty() ? 0 : resultClass.c_str(),
		role.empty() ? 0 : role.c_str(),
		resultRole.empty() ? 0 : resultRole.c_str());

	if (aClass) delete aClass;

        if (rc.rc != CMPI_RC_OK)
        {
            OW_THROWCIMMSG(OW_CIMException::FAILED, _npiHandle.providerError);
        }

    }
    else
    {
        OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support associatorNames");
    }
}

/////////////////////////////////////////////////////////////////////////////
void
OW_CMPIAssociatorProviderProxy::associators(
		const OW_ProviderEnvironmentIFCRef &env,
		const OW_String& ns,
		const OW_CIMObjectPath& assocName,
		const OW_CIMObjectPath& objectName,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String& resultClass,
		const OW_String& role,
		const OW_String& resultRole,
		const OW_Bool& includeQualifiers,
		const OW_Bool& includeClassOrigin,
		const OW_StringArray *propertyList)
{
    env->getLogger()->
        logDebug("OW_CMPIAssociatorProviderProxy::associators()");

    if (m_ftable->fp_associators != NULL)
    {
        ::CMPIHandle _npiHandle = { 0, 0, 0, 0, NULL };
		OW_CMPIHandleFreer nhf(_npiHandle);

        _npiHandle.thisObject = (void *) static_cast<const void *>(&env);

        //  may the arguments must be copied verbatim
        //  to avoid locking problems

        // initialize association class
        CIMObjectPath _assoc = { (void*)static_cast<const void *> (&assocName)};

        // initialize path
		OW_CIMObjectPath objectNameWithNS(objectName);
		objectNameWithNS.setNameSpace(ns);
        CIMObjectPath _path = { (void*)static_cast<const void *> (&objectNameWithNS)};

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
            free((void*)(*i));
        }

        OW_CMPIVectorFreer vf1(v);

        if (_npiHandle.errorOccurred)
        {
            OW_THROWCIMMSG(OW_CIMException::FAILED, _npiHandle.providerError);
        }

        int n = ::VectorSize(&_npiHandle,v);
        env->getLogger()->logDebug(format("OW_CMPIAssociatorProviderProxy::"
			"associators() got %1 associator instances", n - 1));

        CIMInstance my_inst;
        for (int i=0; i < n; i++)
        {
            my_inst.ptr = _VectorGet(&_npiHandle,v,i);
            OW_CIMInstance ow_inst(*
                static_cast<OW_CIMInstance *>(my_inst.ptr) );

			result.handle(ow_inst);
        }
    }
    else
    {
        OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support associators");
    }
}

/////////////////////////////////////////////////////////////////////////////
void
OW_CMPIAssociatorProviderProxy::references(
		const OW_ProviderEnvironmentIFCRef &env,
		const OW_String& ns,
		const OW_CIMObjectPath& assocName,
		const OW_CIMObjectPath& objectName,
		OW_CIMInstanceResultHandlerIFC& result,
		const OW_String& role,
		const OW_Bool& includeQualifiers,
		const OW_Bool& includeClassOrigin,
		const OW_StringArray *propertyList)
{
    env->getLogger()->logDebug("OW_CMPIAssociatorProviderProxy::references()");

    if (m_ftable->fp_references != NULL)
    {
        ::CMPIHandle _npiHandle = { 0, 0, 0, 0, NULL };
		OW_CMPIHandleFreer nhf(_npiHandle);

        _npiHandle.thisObject = (void *) static_cast<const void *>(&env);

        //  may the arguments must be copied verbatim
        //  to avoid locking problems

        // initialize association class
        CIMObjectPath _assoc = { (void*)static_cast<const void *> (&assocName)};

        // initialize path
		OW_CIMObjectPath objectNameWithNS(objectName);
		objectNameWithNS.setNameSpace(ns);
        CIMObjectPath _path = { (void*)static_cast<const void *> (&objectNameWithNS)};

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
            free((void*)(*i));
        }

        OW_CMPIVectorFreer vf1(v);

        if (_npiHandle.errorOccurred)
        {
            OW_THROWCIMMSG(OW_CIMException::FAILED, _npiHandle.providerError);
        }

        int n = ::VectorSize(&_npiHandle,v);
        env->getLogger()->logDebug(format("OW_CMPIAssociatorProviderProxy::"
			"references() got %1 associator instances", n - 1));

        CIMInstance my_inst;
        for (int i=0; i < n; i++)
        {
            my_inst.ptr = _VectorGet(&_npiHandle,v,i);
            OW_CIMInstance ow_inst(*
                static_cast<OW_CIMInstance *>(my_inst.ptr) );

			result.handle(ow_inst);
        }
    }
    else
    {
        OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support references");
    }
}

/////////////////////////////////////////////////////////////////////////////
void
OW_CMPIAssociatorProviderProxy::referenceNames(
		const OW_ProviderEnvironmentIFCRef &env,
		const OW_String& ns,
		const OW_CIMObjectPath& assocName,
		const OW_CIMObjectPath& objectName,
		OW_CIMObjectPathResultHandlerIFC& result,
		const OW_String& role)
{
    env->getLogger()->
        logDebug("OW_CMPIAssociatorProviderProxy::referenceNames()");

    if (m_ftable->fp_referenceNames != NULL)
    {
        ::CMPIHandle _npiHandle = { 0, 0, 0, 0, NULL };
		OW_CMPIHandleFreer nhf(_npiHandle);

        _npiHandle.thisObject = (void *) static_cast<const void *>(&env);

        //  may the arguments must be copied verbatim
        //  to avoid locking problems

        // initialize association class
        CIMObjectPath _assoc = { (void*)static_cast<const void *> (&assocName)};

        // initialize path
		OW_CIMObjectPath objectNameWithNS(objectName);
		objectNameWithNS.setNameSpace(ns);
        CIMObjectPath _path = { (void*)static_cast<const void *> (&objectNameWithNS)};

        ::Vector v =
            m_ftable->fp_referenceNames(&_npiHandle, _assoc, _path,
                    role.empty() ? 0 : role.c_str());

        OW_CMPIVectorFreer vf1(v);

        if (_npiHandle.errorOccurred)
        {
            OW_THROWCIMMSG(OW_CIMException::FAILED, _npiHandle.providerError);
        }


        CIMObjectPath my_cop;

        for (int i=::VectorSize(&_npiHandle, v) - 1; i >= 0; i--)
        {
            my_cop.ptr = _VectorGet(&_npiHandle,v,i);
            OW_CIMObjectPath ow_cop(*
                static_cast<OW_CIMObjectPath*>(my_cop.ptr) );

			result.handle(ow_cop);
        }
    }
    else
    {
        OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support referenceNames");
    }
}

