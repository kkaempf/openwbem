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
#include "OW_NPIPolledProviderProxy.hpp"
#include "NPIExternal.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_NPIProviderIFCUtils.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMParamValue.hpp"

/////////////////////////////////////////////////////////////////////////////
OW_Int32
OW_NPIPolledProviderProxy::getInitialPollingInterval(
      const OW_ProviderEnvironmentIFCRef& env)
{
        env->getLogger()->logDebug("OW_NPIPolledProviderIFC::getInitialPollingInterval()");
	return 1;
}


OW_Int32
OW_NPIPolledProviderProxy::poll(const OW_ProviderEnvironmentIFCRef &env)
{
        OW_CIMValue rval;

        env->getLogger()->
            logDebug("OW_NPIPolledProviderIFC::poll()");

        if (m_ftable->fp_mustPoll != NULL)
	{
            ::NPIHandle _npiHandle = { 0, 0, 0, 0, NULL };
			OW_NPIHandleFreer nhf(_npiHandle);

            _npiHandle.thisObject = (void *) static_cast<const void *>(&env);

	    char * expo = "SourceInstance.PercentageSpaceUse 80";
	    SelectExp exp = {expo};
	    CIMObjectPath cop = {NULL};

            m_ftable->fp_mustPoll( &_npiHandle, exp, expo, cop);

		if (_npiHandle.errorOccurred)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				_npiHandle.providerError);
		}
	}
	return 0;
}

void OW_NPIPolledProviderProxy::activateFilter(
	const OW_ProviderEnvironmentIFCRef& env, const OW_String& query,
	const OW_String& Type)
{
	env->getLogger()->logDebug("activateFilter");
	if (m_ftable->fp_activateFilter != NULL)
	{
            env->getLogger()->logDebug("activateFilter2");
            ::NPIHandle _npiHandle = { 0, 0, 0, 0, NULL };
			OW_NPIHandleFreer nhf(_npiHandle);

            _npiHandle.thisObject = (void *) static_cast<const void *>(&env);

	    char * expo = query.allocateCString();
	    char * _type = Type.allocateCString();
	    SelectExp exp = {expo};
	    CIMObjectPath cop = {NULL};

            m_ftable->fp_activateFilter( &_npiHandle, exp, _type, cop, 0);
	    free(expo);
	    free(_type);

		if (_npiHandle.errorOccurred)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				_npiHandle.providerError);
		}
	}
}

void OW_NPIPolledProviderProxy::deactivateFilter(
	const OW_ProviderEnvironmentIFCRef& env, const OW_String& query,
	const OW_String& Type)
{
	env->getLogger()->logDebug("deactivateFilter");
	if (m_ftable->fp_deActivateFilter != NULL)
	{
            ::NPIHandle _npiHandle = { 0, 0, 0, 0, NULL };
			OW_NPIHandleFreer nhf(_npiHandle);

            env->getLogger()->logDebug("deactivateFilter2");

            _npiHandle.thisObject = (void *) static_cast<const void *>(&env);

	    char * expo = query.allocateCString();
	    char * _type = Type.allocateCString();
	    SelectExp exp = {expo};
	    CIMObjectPath cop = {NULL};
	    char * type = NULL;

            m_ftable->fp_deActivateFilter( &_npiHandle, exp, _type, cop, 0);
	    free(type);
	    free(expo);

		if (_npiHandle.errorOccurred)
		{
			OW_THROWCIMMSG(OW_CIMException::FAILED,
				_npiHandle.providerError);
		}
	}
}

#if 0


            //  may the arguments must be copied verbatim
            //  to avoid locking problems

            OW_CIMObjectPath owcop = path;
			owcop.setNameSpace(ns);
            CIMObjectPath _cop= {static_cast<void *> (&owcop)};

            Vector parm_in = VectorNew(&_npiHandle);
            Vector parm_out = VectorNew(&_npiHandle);

            for (int i = 0, n = in.size(); i < n; i++)
            {
                OW_CIMParamValue * owpv = new OW_CIMParamValue(in[i]);
                _VectorAddTo(
                    &_npiHandle, parm_in, static_cast<void *> (owpv) );
            }

            OW_NPIVectorFreer vf1(parm_in);
            OW_NPIVectorFreer vf2(parm_out);

            CIMValue cv = m_ftable->fp_invokeMethod(
                &_npiHandle, _cop , methodName.c_str(), parm_in, parm_out);

			if (_npiHandle.errorOccurred)
			{
				OW_THROWCIMMSG(OW_CIMException::FAILED,
					_npiHandle.providerError);
			}

            rval = * static_cast<OW_CIMValue *> (cv.ptr);

            for (int i = 0, n = VectorSize(&_npiHandle, parm_out); i < n; i++)
            {
                OW_CIMParamValue owpv = * static_cast<OW_CIMParamValue *>
                    (_VectorGet(&_npiHandle, parm_out, i));
                out.append(owpv);
            }
        }

        return rval;
}
#endif


