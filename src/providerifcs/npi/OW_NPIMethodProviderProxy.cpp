/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
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
#include "OW_NPIMethodProviderProxy.hpp"
#include "NPIExternal.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_NPIProviderIFCUtils.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_Logger.hpp"

namespace OpenWBEM
{

namespace
{
	const String COMPONENT_NAME("ow.provider.npi.ifc");
}

/////////////////////////////////////////////////////////////////////////////
NPIMethodProviderProxy::~NPIMethodProviderProxy()
{
}
/////////////////////////////////////////////////////////////////////////////
CIMValue
NPIMethodProviderProxy::invokeMethod(
	const ProviderEnvironmentIFCRef &env,
	const String& ns,
	const CIMObjectPath& path,
	const String &methodName,
	const CIMParamValueArray &in, CIMParamValueArray &out)
{
		CIMValue rval(CIMNULL);
		OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), "NPIInstanceProviderProxy::invokeMethod()");
		if (m_ftable->fp_invokeMethod != NULL)
		{
		::NPIHandle _npiHandle = { 0, 0, 0, 0, m_ftable->npicontext};
			NPIHandleFreer nhf(_npiHandle);
			ProviderEnvironmentIFCRef env2(env);
			_npiHandle.thisObject = static_cast<void *>(&env2);
			//  may the arguments must be copied verbatim
			//  to avoid locking problems
			CIMObjectPath owcop = path;
			owcop.setNameSpace(ns);
			::CIMObjectPath _cop= {static_cast<void *> (&owcop)};
			::Vector parm_in = VectorNew(&_npiHandle);
			::Vector parm_out = VectorNew(&_npiHandle);
			for (int i = 0, n = in.size(); i < n; i++)
			{
				CIMParamValue * owpv = new CIMParamValue(in[i]);
		_NPIGarbageCan(&_npiHandle, owpv, CIM_PARAMVALUE);
				_VectorAddTo(
					&_npiHandle, parm_in, static_cast<void *> (owpv) );
			}
			for (int i = 0, n = out.size(); i < n; i++)
			{
				CIMParamValue * owpv = new CIMParamValue(out[i]);
		_NPIGarbageCan(&_npiHandle, owpv, CIM_PARAMVALUE);
				_VectorAddTo(
					&_npiHandle, parm_out, static_cast<void *> (owpv) );
			}
			::CIMValue cv = m_ftable->fp_invokeMethod(
				&_npiHandle, _cop , methodName.c_str(), parm_in, parm_out);
			if (_npiHandle.errorOccurred)
			{
				OW_THROWCIMMSG(CIMException::FAILED,
					_npiHandle.providerError);
			}
			rval = * static_cast<CIMValue *> (cv.ptr);
			out.clear();
			for (int i = 0, n = VectorSize(&_npiHandle, parm_out); i < n; i++)
			{
				CIMParamValue owpv = * static_cast<CIMParamValue *>
					(_VectorGet(&_npiHandle, parm_out, i));
				out.append(owpv);
			}
		}
		return rval;
}

} // end namespace OpenWBEM

