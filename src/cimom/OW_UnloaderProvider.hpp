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

#ifndef OW_UNLOADERPROVIDER_HPP_
#define OW_UNLOADERPROVIDER_HPP_

#include "OW_config.h"
#include "OW_CppPolledProviderIFC.hpp"
#include "OW_Reference.hpp"

class OW_CIMOMEnvironment;
typedef OW_Reference<OW_CIMOMEnvironment> OW_CIMOMEnvironmentRef;

class OW_UnloaderProvider : public OW_CppPolledProviderIFC
{
public:
	OW_UnloaderProvider(OW_CIMOMEnvironmentRef pcenv);
	virtual ~OW_UnloaderProvider();

	/**
	 * Called by the CIMOM to give this OW_CppPolledProviderIFC to
	 * opportunity to export indications if needed.
	 * @param lch	A local CIMOM handle the provider can use to export
	 *					indications if needed.
	 * @return How many seconds before the next call to the poll method. If this
	 * method returns -1 then the last polling interval will be used. If it
	 * returns 0 then the poll method will never be called again.
	 */
	virtual OW_Int32 poll(const OW_ProviderEnvironmentIFCRef &env);

	/**
	 * @return The amount of seconds before the first call to the poll method.
	 * If this method returns zero, then the poll method is never called.
	 */
	virtual OW_Int32 getInitialPollingInterval(const
		OW_ProviderEnvironmentIFCRef &env);

private:
	OW_CIMOMEnvironmentRef m_pcenv;
};



#endif

