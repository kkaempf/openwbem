/*******************************************************************************
* Copyright (C) 2003 Center 7, Inc All rights reserved.
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
#include "OW_ProviderIFCBaseIFC.hpp"

///////////////////////////////////////////////////////////////////////////////
OW_ProviderIFCBaseIFC::OW_ProviderIFCBaseIFC()
: signature(0xABCDEFA0)
{
}

///////////////////////////////////////////////////////////////////////////////
OW_ProviderIFCBaseIFC::~OW_ProviderIFCBaseIFC() 
{
}

///////////////////////////////////////////////////////////////////////////////
void
OW_ProviderIFCBaseIFC::init(const OW_ProviderEnvironmentIFCRef& env,
	OW_InstanceProviderInfoArray& i,
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
	OW_AssociatorProviderInfoArray& a,
#endif
	OW_MethodProviderInfoArray& m,
#ifdef OW_ENABLE_PROPERTY_PROVIDERS
	OW_PropertyProviderInfoArray& p,
#endif
	OW_IndicationProviderInfoArray& ind)
{
	doInit(env, i, 
#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
		a, 
#endif
		m, 
#ifdef OW_ENABLE_PROPERTY_PROVIDERS
		p, 
#endif
		ind);
}

///////////////////////////////////////////////////////////////////////////////
OW_InstanceProviderIFCRef 
OW_ProviderIFCBaseIFC::getInstanceProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	return doGetInstanceProvider(env, provIdString);
}

///////////////////////////////////////////////////////////////////////////////
OW_MethodProviderIFCRef 
OW_ProviderIFCBaseIFC::getMethodProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	return doGetMethodProvider(env, provIdString);
}

#ifdef OW_ENABLE_PROPERTY_PROVIDERS
///////////////////////////////////////////////////////////////////////////////
OW_PropertyProviderIFCRef 
OW_ProviderIFCBaseIFC::getPropertyProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	return doGetPropertyProvider(env, provIdString);
}
#endif

#ifndef OW_DISABLE_ASSOCIATION_TRAVERSAL
///////////////////////////////////////////////////////////////////////////////
OW_AssociatorProviderIFCRef 
OW_ProviderIFCBaseIFC::getAssociatorProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	return  doGetAssociatorProvider(env, provIdString);
}
#endif

///////////////////////////////////////////////////////////////////////////////
OW_IndicationExportProviderIFCRefArray 
OW_ProviderIFCBaseIFC::getIndicationExportProviders(
	const OW_ProviderEnvironmentIFCRef& env
	)
{
	return doGetIndicationExportProviders(env);
}

///////////////////////////////////////////////////////////////////////////////
OW_PolledProviderIFCRefArray 
OW_ProviderIFCBaseIFC::getPolledProviders(const OW_ProviderEnvironmentIFCRef& env
	)
{
	return doGetPolledProviders(env);
}

///////////////////////////////////////////////////////////////////////////////
void 
OW_ProviderIFCBaseIFC::unloadProviders(const OW_ProviderEnvironmentIFCRef& env)
{
	doUnloadProviders(env);
}

///////////////////////////////////////////////////////////////////////////////
OW_IndicationProviderIFCRef 
OW_ProviderIFCBaseIFC::getIndicationProvider(const OW_ProviderEnvironmentIFCRef& env,
	const char* provIdString)
{
	return  doGetIndicationProvider(env, provIdString);
}

