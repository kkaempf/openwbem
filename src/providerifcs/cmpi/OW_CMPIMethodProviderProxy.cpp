/*******************************************************************************
* Copyright (C) 2003-2004 Vintela, Inc., IBM Corp. All rights reserved.
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
*
* Author:        Markus Mueller <sedgewick_de@yahoo.de>
*
*******************************************************************************/
#include "OW_config.h"
#include "OW_CMPIMethodProviderProxy.hpp"
#include "OW_CIMClass.hpp"
#include "OW_CIMException.hpp"
#include "OW_Format.hpp"
#include "OW_CMPIProviderIFCUtils.hpp"
#include "OW_CIMValue.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_CIMParamValue.hpp"
#include "OW_Logger.hpp"

namespace OpenWBEM
{

namespace
{
	const String COMPONENT_NAME("ow.provider.cmpi.ifc");
}

/////////////////////////////////////////////////////////////////////////////
CMPIMethodProviderProxy::CMPIMethodProviderProxy(const CMPIFTABLERef& f)
	: m_ftable(f)
{
}
/////////////////////////////////////////////////////////////////////////////
CMPIMethodProviderProxy::~CMPIMethodProviderProxy()
{
}
/////////////////////////////////////////////////////////////////////////////
CIMValue
CMPIMethodProviderProxy::invokeMethod(const ProviderEnvironmentIFCRef &env,
	const String& ns,
	const CIMObjectPath& path,
	const String &methodName,
	const CIMParamValueArray &in, CIMParamValueArray &out)
{
	OW_LOG_DEBUG(env->getLogger(COMPONENT_NAME), "CMPIMethodProviderProxy::invokeMethod()");

	if (m_ftable->miVector.methMI->ft->invokeMethod != NULL)
	{
		CMPIStatus rc = {CMPI_RC_OK, NULL};
		::CMPIOperationContext context;
		ProviderEnvironmentIFCRef env2(env);
		::CMPI_Broker localBroker(m_ftable->broker);
		localBroker.hdl = static_cast<void *>(&env2);
																				
		CMPI_ContextOnStack eCtx(context);
		CMPI_ThreadContext thr(&localBroker, &eCtx);
		CIMObjectPath objectReference = path;
		objectReference.setNameSpace(ns);
		CMPI_ObjectPathOnStack eRef(objectReference);
		CMPI_ArgsOnStack eArgsIn(in);
		CMPI_ArgsOnStack eArgsOut(out);
		CMPIValueValueResultHandler handler;
		CMPI_ResultOnStack eRes(handler);
		char* mName = const_cast<char*>(methodName.c_str());
		CMPIFlags flgs=0;
		eCtx.ft->addEntry(&eCtx, const_cast<char*>(CMPIInvocationFlags), (CMPIValue*)&flgs,
			CMPI_uint32);

		rc=m_ftable->miVector.methMI->ft->invokeMethod(
			m_ftable->miVector.methMI, &eCtx, &eRes, &eRef, mName, &eArgsIn,
			&eArgsOut);

		if (rc.rc == CMPI_RC_OK)
		{
			return handler.getValue();
		}
		else
		{
			OW_THROWCIMMSG(CIMException::ErrNoType(rc.rc), rc.msg ?
				CMGetCharPtr(rc.msg) : "");
		}
	}

	return CIMValue(CIMNULL);
}

} // end namespace OpenWBEM

