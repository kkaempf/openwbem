/*******************************************************************************
* Copyright (C) 2002 Caldera International, Inc All rights reserved.
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

#ifndef OW_Perl_INDICATION_PROVIDER_PROXY_HPP_
#define OW_Perl_INDICATION_PROVIDER_PROXY_HPP_

#include "OW_config.h"
#include "OW_IndicationProviderIFC.hpp"
#include "OW_FTABLERef.hpp"

class OW_PerlIndicationProviderProxy : public OW_IndicationProviderIFC
{
public:
	OW_PerlIndicationProviderProxy(const OW_FTABLERef& f)
	: m_ftable(f)
	{
	}

	virtual ~OW_PerlIndicationProviderProxy()
	{
	}

	virtual void deActivateFilter(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_WQLSelectStatement& filter, 
		const OW_String& eventType, 
		const OW_String& nameSpace,
		const OW_StringArray& classes, 
		bool lastActivation
		); 

	virtual void activateFilter(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_WQLSelectStatement& filter, 
		const OW_String& eventType, 
		const OW_String& nameSpace,
		const OW_StringArray& classes, 
		bool firstActivation
		);

	virtual void authorizeFilter(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_WQLSelectStatement& filter, 
		const OW_String& eventType, 
		const OW_String& nameSpace,
		const OW_StringArray& classes, 
		const OW_String& owner
		);

	virtual int mustPoll(
		const OW_ProviderEnvironmentIFCRef& env,
		const OW_WQLSelectStatement& filter, 
		const OW_String& eventType, 
		const OW_String& nameSpace,
		const OW_StringArray& classes
		);

private:
	OW_FTABLERef m_ftable;
};

#endif

