/*******************************************************************************
* Copyright (C) 2002 Center 7, Inc All rights reserved.
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
#ifndef OW_NPI_INDICATION_PROVIDER_PROXY_HPP_
#define OW_NPI_INDICATION_PROVIDER_PROXY_HPP_
#include "OW_config.h"
#include "OW_IndicationProviderIFC.hpp"
#include "OW_FTABLERef.hpp"

namespace OpenWBEM
{

class NPIIndicationProviderProxy : public IndicationProviderIFC
{
public:
	NPIIndicationProviderProxy(const FTABLERef& f)
	: m_ftable(f)
    , m_activationCount(0)
	{
	}
	virtual ~NPIIndicationProviderProxy()
	{
	}
	virtual void deActivateFilter(
		const ProviderEnvironmentIFCRef &env, 
		const WQLSelectStatement &filter, 
		const String &eventType, 
		const String& nameSpace,
		const StringArray& classes); 
	virtual void activateFilter(
		const ProviderEnvironmentIFCRef &env, 
		const WQLSelectStatement &filter, 
		const String &eventType, 
		const String& nameSpace,
		const StringArray& classes);
	virtual void authorizeFilter(
		const ProviderEnvironmentIFCRef &env, 
		const WQLSelectStatement &filter, 
		const String &eventType, 
		const String& nameSpace,
		const StringArray& classes, 
		const String &owner);
	virtual int mustPoll(
		const ProviderEnvironmentIFCRef& env,
		const WQLSelectStatement& filter, 
		const String& eventType, 
		const String& nameSpace,
		const StringArray& classes
		);
private:
	FTABLERef m_ftable;
    unsigned int m_activationCount;
};

} // end namespace OpenWBEM

#endif
