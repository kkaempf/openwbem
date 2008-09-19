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
#include "OW_UnloaderProvider.hpp"
#include "OW_CIMOMEnvironment.hpp"
#include "OW_Format.hpp"

#define POLLING_INTERVAL 60

	/**
	 * Called by the CIMOM to give this OW_CppPolledProviderIFC to
	 * opportunity to export indications if needed.
	 * @param lch	A local CIMOM handle the provider can use to export
	 *					indications if needed.
	 * @return How many seconds before the next call to the poll method. If this
	 * method returns -1 then the last polling interval will be used. If it
	 * returns 0 then the poll method will never be called again.
	 */
OW_Int32
OW_UnloaderProvider::poll(const OW_ProviderEnvironmentIFCRef &/*env*/)
{
	//env->getLogger()->logDebug( "Polling OW_UnloaderProvider");
	m_pcenv->unloadProviders();
	m_pcenv->unloadReqHandlers();
	return -1;
}

	/**
	 * @return The amount of seconds before the first call to the poll method.
	 * If this method returns zero, then the poll method is never called.
	 */
OW_Int32
OW_UnloaderProvider::getInitialPollingInterval(const
		OW_ProviderEnvironmentIFCRef &env)
{
	env->getLogger()->logDebug(format(
		"Calling getInitialPollingInterval in OW_ProviderUnloader; returning %1",
		POLLING_INTERVAL));
	return POLLING_INTERVAL;
}


