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
#ifndef OW_PROVIDER_ENVIRONMENT_HPP_
#define OW_PROVIDER_ENVIRONMENT_HPP_

#include "OW_config.h"
#include "OW_Logger.hpp"
#include "OW_CIMOMHandleIFC.hpp"
#include "OW_Reference.hpp"

class OW_ProviderEnvironmentIFC
{
public:
	virtual ~OW_ProviderEnvironmentIFC(){}

	// this is a regular cimom handle that does access checking and may call providers
	virtual OW_CIMOMHandleIFCRef getCIMOMHandle() const = 0;

	// this is a cimom handle that directly accesses the repository (OW_CIMServer is bypassed).
	// no providers will be called.  This function should only be called if getCIMOMHandle()
	// is insufficent.
	virtual OW_CIMOMHandleIFCRef getRepositoryCIMOMHandle() const = 0;
	virtual OW_LoggerRef getLogger() const = 0;
	virtual OW_String getConfigItem(const OW_String &name) const = 0;
    virtual OW_String getUserName() const = 0;
};

typedef OW_Reference<OW_ProviderEnvironmentIFC> OW_ProviderEnvironmentIFCRef;
									
#endif									
