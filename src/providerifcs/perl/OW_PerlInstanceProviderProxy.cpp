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
#include "OW_FTABLERef.hpp"
#include "OW_PerlInstanceProviderProxy.hpp"
#include "NPIExternal.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_NPIProviderIFCUtils.hpp"

// debugging
#define DDD(X) // X

/////////////////////////////////////////////////////////////////////////////
void
OW_PerlInstanceProviderProxy::enumInstanceNames(
        const OW_ProviderEnvironmentIFCRef& env,
		const OW_String& ns,
		const OW_String& className,
		OW_CIMObjectPathResultHandlerIFC& result,
        const OW_CIMClass& cimClass )
{
        env->getLogger()->
            logDebug("OW_PerlInstanceProviderProxy::enumInstanceNames()");

        if (m_ftable->fp_enumInstanceNames!= NULL)
        {
            ::NPIHandle _npiHandle = { 0, 0, 0, 0, m_ftable->npicontext};

	    OW_NPIHandleFreer nhf(_npiHandle);

	    OW_ProviderEnvironmentIFCRef env2(env);

            _npiHandle.thisObject = static_cast<void *>(&env2);

            //  may the arguments must be copied verbatim
            //  to avoid locking problems
			OW_CIMClass cimClass2(cimClass);
            CIMClass _cc = { static_cast<void *> (&cimClass2)};

	    OW_CIMObjectPath cop(className, ns);
            CIMObjectPath _cop = { static_cast<void *> (&cop)};

            ::Vector v =
                m_ftable->fp_enumInstanceNames(&_npiHandle,_cop,true,_cc);

	    // the vector and its contents are
            //  deleted by the global garbage collector (npiHandle)

            if (_npiHandle.errorOccurred)
            {
                OW_THROWCIMMSG(OW_CIMException::FAILED,
                    _npiHandle.providerError);
            }

            CIMObjectPath my_cop;
            for (int i=0,n=VectorSize(&_npiHandle,v); i < n; i++)
            {
                my_cop.ptr = _VectorGet(&_npiHandle,v,i);
                OW_CIMObjectPath ow_cop(*
                    static_cast<OW_CIMObjectPath *>(my_cop.ptr) );

                ow_cop.setObjectName(cimClass.getName());
		result.handle(ow_cop);
            }
            //printf("Leaving enumInstanceNames\n");

        }
        else
        {
			OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support enumInstanceNames");
        }
}

/////////////////////////////////////////////////////////////////////////////
void
OW_PerlInstanceProviderProxy::enumInstances(
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
	env->getLogger()->
	logDebug("OW_PerlInstanceProviderProxy::enumInstances()");

	if (m_ftable->fp_enumInstances == NULL)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support enumInstances");
	}

	::NPIHandle _npiHandle = { 0, 0, 0, 0, m_ftable->npicontext};
	OW_NPIHandleFreer nhf(_npiHandle);

	OW_ProviderEnvironmentIFCRef env2(env);
	_npiHandle.thisObject = static_cast<void *>(&env2);

	//  may the arguments must be copied verbatim
	//  to avoid locking problems

	OW_CIMClass cimClass2(cimClass);
	CIMClass _cc = { static_cast<void *> (&cimClass2)};

	OW_CIMObjectPath cop(className, ns);
	CIMObjectPath _cop = { static_cast<void *> (&cop)};

	int de = deep;
	int lo = localOnly;
	::Vector v =
	m_ftable->fp_enumInstances(&_npiHandle, _cop, de, _cc, lo);

	//OW_NPIVectorFreer vf1(v);

	if (_npiHandle.errorOccurred)
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED,
		_npiHandle.providerError);
	}

	CIMInstance my_inst;
	for (int i=0,n=VectorSize(&_npiHandle,v); i < n; i++)
	{
		my_inst.ptr = _VectorGet(&_npiHandle,v,i);
		OW_CIMInstance ow_inst(*
		static_cast<OW_CIMInstance *>(my_inst.ptr) );

// FIXME
		ow_inst.setClassName(cimClass.getName());

		result.handle(ow_inst.clone(localOnly,deep,includeQualifiers,
			includeClassOrigin,propertyList,requestedClass,cimClass));
	}
}
	
/////////////////////////////////////////////////////////////////////////////
void
OW_PerlInstanceProviderProxy::deleteInstance(const OW_ProviderEnvironmentIFCRef &env,
	const OW_String& ns, const OW_CIMObjectPath& cop)
{
	env->getLogger()->
		logDebug("OW_PerlInstanceProviderProxy::deleteInstance()");

	if (m_ftable->fp_deleteInstance!= NULL)
	{
		::NPIHandle _npiHandle = { 0, 0, 0, 0, m_ftable->npicontext};
		OW_NPIHandleFreer nhf(_npiHandle);

		OW_ProviderEnvironmentIFCRef env2(env);
		_npiHandle.thisObject = static_cast<void *>(&env2);

		//  may the arguments must be copied verbatim
		//  to avoid locking problems

		OW_CIMObjectPath copWithNS(cop);
		copWithNS.setNameSpace(ns);
		CIMObjectPath _cop = { static_cast<void *> (&copWithNS)};

		m_ftable->fp_deleteInstance(&_npiHandle, _cop);

		if (_npiHandle.errorOccurred)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				_npiHandle.providerError);
		}
	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support deleteInstance");
	}
}

/////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_PerlInstanceProviderProxy::getInstance(const OW_ProviderEnvironmentIFCRef &env,
	const OW_String& ns,
	const OW_CIMObjectPath& instanceName,
	OW_Bool localOnly,
	OW_Bool includeQualifiers, 
	OW_Bool includeClassOrigin,
	const OW_StringArray* propertyList, 
	const OW_CIMClass& cimClass)
{
        OW_CIMInstance rval;

        env->getLogger()->
            logDebug("OW_PerlInstanceProviderProxy::getInstance()");

        if (m_ftable->fp_getInstance != NULL)
        {
	    ::NPIHandle _npiHandle = { 0, 0, 0, 0, m_ftable->npicontext};
			OW_NPIHandleFreer nhf(_npiHandle);

			OW_ProviderEnvironmentIFCRef env2(env);
            _npiHandle.thisObject = static_cast<void *>(&env2);

            //  may the arguments must be copied verbatim
            //  to avoid locking problems

			OW_CIMClass cimClass2(cimClass);
            CIMClass _cc = { static_cast<void *> (&cimClass2)};

			OW_CIMObjectPath cop(instanceName);
			cop.setNameSpace(ns);
            CIMObjectPath _cop = { static_cast<void *> (&cop)};

            int lo = localOnly;

            CIMInstance my_inst =
                m_ftable->fp_getInstance(&_npiHandle, _cop, _cc, lo);

            if (_npiHandle.errorOccurred)
            {
                OW_THROWCIMMSG(OW_CIMException::FAILED,
                    _npiHandle.providerError);
            }

            OW_CIMInstance ow_inst(*
                static_cast<OW_CIMInstance *>(my_inst.ptr));

// FIXME:
            ow_inst.setClassName(cimClass.getName());

            rval = ow_inst;

			rval = rval.clone(localOnly,includeQualifiers,includeClassOrigin,propertyList);
        }
        else
        {
			OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support getInstance");
        }

        return rval;
}

/////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_PerlInstanceProviderProxy::createInstance(
    const OW_ProviderEnvironmentIFCRef &env, const OW_String& ns,
    const OW_CIMInstance& cimInstance)
{
        OW_CIMObjectPath rval;

        env->getLogger()->
            logDebug("OW_PerlInstanceProviderProxy::createInstance()");

        if (m_ftable->fp_createInstance != NULL)
        {
	    ::NPIHandle _npiHandle = { 0, 0, 0, 0, m_ftable->npicontext};
			OW_NPIHandleFreer nhf(_npiHandle);

			OW_ProviderEnvironmentIFCRef env2(env);
            _npiHandle.thisObject = static_cast<void *>(&env2);

            //  may the arguments must be copied verbatim
            //  to avoid locking problems

			OW_CIMInstance cimInstance2(cimInstance);
            CIMInstance _ci = { static_cast<void *> (&cimInstance2)};

			OW_CIMObjectPath cop(cimInstance);
			cop.setNameSpace(ns);
            CIMObjectPath _cop = { static_cast<void *> (const_cast<OW_CIMObjectPath*>(&cop))};

            CIMObjectPath _rcop =
                m_ftable->fp_createInstance(&_npiHandle, _cop, _ci);

            if (_npiHandle.errorOccurred)
            {
                OW_THROWCIMMSG(OW_CIMException::FAILED,
                    _npiHandle.providerError);
            }

            rval = *(static_cast<OW_CIMObjectPath *>(_rcop.ptr) );
        }
        else
        {
			OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support createInstance");
        }

        return rval;
}

/////////////////////////////////////////////////////////////////////////////
void
OW_PerlInstanceProviderProxy::modifyInstance(const OW_ProviderEnvironmentIFCRef &env,
	const OW_String& ns,
	const OW_CIMInstance& modifiedInstance,
	const OW_CIMInstance& previousInstance,
	OW_Bool includeQualifiers,
	const OW_StringArray* propertyList,
	const OW_CIMClass& theClass)
{
	env->getLogger()->
	logDebug("OW_PerlInstanceProviderProxy::modifyInstance()");

	if (m_ftable->fp_setInstance != NULL)
	{
	        ::NPIHandle _npiHandle = { 0, 0, 0, 0, m_ftable->npicontext};
		OW_NPIHandleFreer nhf(_npiHandle);

		OW_ProviderEnvironmentIFCRef env2(env);
		_npiHandle.thisObject = static_cast<void *>(&env2);

		//  may the arguments must be copied verbatim
		//  to avoid locking problems
		OW_CIMInstance newInst(modifiedInstance.createModifiedInstance(
			previousInstance, includeQualifiers, propertyList, theClass));

		CIMInstance _ci = { static_cast<void *> (&newInst)};

		OW_CIMObjectPath cop(modifiedInstance);
		cop.setNameSpace(ns);
		CIMObjectPath _cop = { static_cast<void *> (&cop)};

		m_ftable->fp_setInstance(&_npiHandle, _cop, _ci);

		if (_npiHandle.errorOccurred)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				_npiHandle.providerError);
		}


	}
	else
	{
		OW_THROWCIMMSG(OW_CIMException::FAILED, "Provider does not support modifyInstance");
	}
}


