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
#include "NPIProvider.hpp"
#include "OW_NPIInstanceProviderProxy.hpp"
#include "NPIExternal.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_NPIProviderIFCUtils.hpp"

// debugging
#define DDD(X) // X

/////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPathEnumeration
OW_NPIInstanceProviderProxy::enumInstances(
        const OW_ProviderEnvironmentIFCRef& env,
        OW_CIMObjectPath cop,
        OW_Bool deep,
        OW_CIMClass cimClass )
{
        OW_CIMObjectPathEnumeration rval;
		  (void)deep;

        env->getLogger()->
            logDebug("OW_NPIInstanceProviderProxy::enumInstanceNames()");

        if (m_ftable->fp_enumInstanceNames!= NULL)
        {
            ::NPIHandle _npiHandle = { 0,0,0,0};

            _npiHandle.thisObject = (void *) static_cast<const void *>(&env);

            //  may the arguments must be copied verbatim
            //  to avoid locking problems

            CIMClass _cc = { static_cast<void *> (&cimClass)};

            CIMObjectPath _cop = { static_cast<void *> (&cop)};

            ::Vector v =
                m_ftable->fp_enumInstanceNames(&_npiHandle,_cop,true,_cc);

            OW_NPIVectorFreer vf1(v);

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

// FIXME
                ow_cop.setObjectName(cimClass.getName());
                rval.addElement(ow_cop);
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
OW_NPIInstanceProviderProxy::enumInstances(
        const OW_ProviderEnvironmentIFCRef& env,
        OW_CIMObjectPath cop,
        OW_Bool deep,
        OW_CIMClass cimClass,
        OW_Bool localOnly )
{
        OW_CIMInstanceEnumeration rval;

        env->getLogger()->
            logDebug("OW_NPIInstanceProviderProxy::enumInstances()");

        if (m_ftable->fp_enumInstances == NULL)
		  {
			  OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
		  }
	 
			::NPIHandle _npiHandle = { 0,0,0,0};

			_npiHandle.thisObject = (void *) static_cast<const void *>(&env);

			//  may the arguments must be copied verbatim
			//  to avoid locking problems

			CIMClass _cc = { static_cast<void *> (&cimClass)};

			CIMObjectPath _cop = { static_cast<void *> (&cop)};

			int de = deep;
			int lo = localOnly;
			::Vector v =
				 m_ftable->fp_enumInstances(&_npiHandle, _cop, de, _cc, lo);

			OW_NPIVectorFreer vf1(v);

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

				 rval.addElement(ow_inst);
			}

        return rval;
}
	
/////////////////////////////////////////////////////////////////////////////
void
OW_NPIInstanceProviderProxy::deleteInstance(const OW_ProviderEnvironmentIFCRef &env,
    OW_CIMObjectPath cop)
{
        env->getLogger()->
            logDebug("OW_NPIInstanceProviderProxy::deleteInstance()");

        if (m_ftable->fp_deleteInstance!= NULL)
        {
            ::NPIHandle _npiHandle = { 0,0,0,0};

            _npiHandle.thisObject = (void *) static_cast<const void *>(&env);

            //  may the arguments must be copied verbatim
            //  to avoid locking problems

            CIMObjectPath _cop = { static_cast<void *> (&cop)};

            m_ftable->fp_deleteInstance(&_npiHandle, _cop);

            if (_npiHandle.errorOccurred)
            {
                OW_THROWCIMMSG(OW_CIMException::FAILED,
                    _npiHandle.providerError);
            }
        }
        else
        {
            OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
        }
}

/////////////////////////////////////////////////////////////////////////////
OW_CIMInstance
OW_NPIInstanceProviderProxy::getInstance(const OW_ProviderEnvironmentIFCRef &env,
    OW_CIMObjectPath cop, OW_CIMClass cimClass, OW_Bool localOnly)
{
        OW_CIMInstance rval;

        env->getLogger()->
            logDebug("OW_NPIInstanceProviderProxy::getInstance()");

        if (m_ftable->fp_getInstance != NULL)
        {
            ::NPIHandle _npiHandle = { 0,0,0,0};

            _npiHandle.thisObject = (void *) static_cast<const void *>(&env);

            //  may the arguments must be copied verbatim
            //  to avoid locking problems

            CIMClass _cc = { static_cast<void *> (&cimClass)};

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
        }
        else
        {
            OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
        }

        return rval;
}

/////////////////////////////////////////////////////////////////////////////
OW_CIMObjectPath
OW_NPIInstanceProviderProxy::createInstance(
    const OW_ProviderEnvironmentIFCRef &env, OW_CIMObjectPath cop,
    OW_CIMInstance cimInstance)
{
        OW_CIMObjectPath rval;

        env->getLogger()->
            logDebug("OW_NPIInstanceProviderProxy::createInstance()");

        if (m_ftable->fp_createInstance != NULL)
        {
            ::NPIHandle _npiHandle = { 0,0,0,0};

            _npiHandle.thisObject = (void *) static_cast<const void *>(&env);

            //  may the arguments must be copied verbatim
            //  to avoid locking problems

            CIMInstance _ci = { static_cast<void *> (&cimInstance)};

            CIMObjectPath _cop = { static_cast<void *> (&cop)};

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
            OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
        }

        return rval;
}

/////////////////////////////////////////////////////////////////////////////
void
OW_NPIInstanceProviderProxy::setInstance(const OW_ProviderEnvironmentIFCRef &env,
    OW_CIMObjectPath cop, OW_CIMInstance cimInstance)
{
        env->getLogger()->
            logDebug("OW_NPIInstanceProviderProxy::setInstance()");

        if (m_ftable->fp_setInstance != NULL)
        {
            ::NPIHandle _npiHandle = { 0,0,0,0};

            _npiHandle.thisObject = (void *) static_cast<const void *>(&env);

            //  may the arguments must be copied verbatim
            //  to avoid locking problems

            CIMInstance _ci = { static_cast<void *> (&cimInstance)};

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
            OW_THROWCIM(OW_CIMException::NOT_SUPPORTED);
        }
}


