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

/**
 * @author Bart Whiteley
 * @author Dan Nuffer
 */

#include "OW_config.h"
#include "OW_UnloaderProvider.hpp"
#include "OW_CIMOMEnvironment.hpp"
#include "OW_Format.hpp"

#define OW_POLLING_INTERVAL 60 // 1 min is the smallest interval for unloading

namespace OpenWBEM
{

namespace
{
	const String COMPONENT_NAME("ow.provider.Unloader");
}

UnloaderProvider::UnloaderProvider()
{
}
UnloaderProvider::~UnloaderProvider()
{
}
/**
 * Called by the CIMOM to give this CppPolledProviderIFC to
 * opportunity to export indications if needed.
 * @param lch	A local CIMOM handle the provider can use to export
 *					indications if needed.
 * @return How many seconds before the next call to the poll method. If this
 * method returns -1 then the last polling interval will be used. If it
 * returns 0 then the poll method will never be called again.
 */
Int32
UnloaderProvider::poll(const ProviderEnvironmentIFCRef &/*env*/)
{
	//env->getLogger(COMPONENT_NAME)->logDebug( "Polling UnloaderProvider");
	CIMOMEnvironment::g_cimomEnvironment->unloadProviders();
	CIMOMEnvironment::g_cimomEnvironment->unloadReqHandlers();
	return -1;
}
/**
 * @return The amount of seconds before the first call to the poll method.
 * If this method returns zero, then the poll method is never called.
 */
Int32
UnloaderProvider::getInitialPollingInterval(const
		ProviderEnvironmentIFCRef &env)
{
	env->getLogger(COMPONENT_NAME)->logDebug(Format(
		"Calling getInitialPollingInterval in ProviderUnloader; returning %1",
		OW_POLLING_INTERVAL));
	return OW_POLLING_INTERVAL;
}
} // end namespace OpenWBEM

OW_PROVIDERFACTORY_NOID(OpenWBEM::UnloaderProvider,Unloader)

