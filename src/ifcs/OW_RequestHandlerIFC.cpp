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
#include "OW_RequestHandlerIFC.hpp"
#include "OW_ServiceEnvironmentIFC.hpp"

///////////////////////////////////////////////////////////////////////////////
OW_RequestHandlerIFC::OW_RequestHandlerIFC()
	: m_cimError()
	, m_hasError(false)
	, m_errorCode(0)
	, m_errorDescription()
	, m_env(0)
{
}

///////////////////////////////////////////////////////////////////////////////
OW_RequestHandlerIFC::~OW_RequestHandlerIFC()
{
}

///////////////////////////////////////////////////////////////////////////////
void OW_RequestHandlerIFC::process(std::istream* istr, std::ostream* ostrEntity,
	std::ostream* ostrError, const OW_SortedVectorMap<OW_String, OW_String>& handlerVars)
{
	doProcess(istr, ostrEntity, ostrError, handlerVars);
}

///////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_RequestHandlerIFC::hasError(OW_Int32& errCode, OW_String& errDescr)
{
	return doHasError(errCode, errDescr);
}

///////////////////////////////////////////////////////////////////////////////
OW_Bool
OW_RequestHandlerIFC::doHasError(OW_Int32& errCode, OW_String& errDescr)
{
	if (m_hasError)
	{
		errCode = m_errorCode;
		errDescr = m_errorDescription;
	}
	return m_hasError;
}

///////////////////////////////////////////////////////////////////////////////
OW_ServiceEnvironmentIFCRef
OW_RequestHandlerIFC::getEnvironment() const
{
	return m_env;
}

///////////////////////////////////////////////////////////////////////////////
void
OW_RequestHandlerIFC::setEnvironment(OW_ServiceEnvironmentIFCRef env)
{
	m_env = env;
}

