/*******************************************************************************
* Copyright (C) 2005 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Dan Nuffer
 */

#ifndef OW_PROVIDER_AGENT_LIFECYCLE_CALLBACK_IFC_HPP_INCLUDE_GUARD_
#define OW_PROVIDER_AGENT_LIFECYCLE_CALLBACK_IFC_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "blocxx/IntrusiveCountableBase.hpp"
#include "OW_CommonFwd.hpp"

namespace OW_NAMESPACE
{

/**
 * The implementation must be thread safe.
 */
class OW_PROVIDERAGENT_API ProviderAgentLifecycleCallbackIFC : public blocxx::IntrusiveCountableBase
{
public:
	ProviderAgentLifecycleCallbackIFC();
	virtual ~ProviderAgentLifecycleCallbackIFC();

	// will be called when the provider agent is ready to begin processing incoming connections
	virtual void started();
	// will be called when the provider agent is beginning the process of shutting down.
	virtual void shuttingDown();
	// will be called if the provider agent has encountered an unrecoverable error and has stopped functioning.
	virtual void fatalError(const blocxx::String& errorDescription);
};

} // end namespace OW_NAMESPACE

#endif

