/*******************************************************************************
* Copyright (C) 2005 Quest Software, Inc. All rights reserved.
* Copyright (C) 2006 Novell, Inc. All rights reserved.
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

#ifndef OW_OOPCPP1PR_HPP_INCLUDE_GUARD_
#define OW_OOPCPP1PR_HPP_INCLUDE_GUARD_

#include "OW_config.h"
#include "OW_CommonFwd.hpp"
#include "OW_ProviderFwd.hpp"
#include "OW_IOIFCStreamBuffer.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_Exception.hpp"

namespace OW_NAMESPACE
{

OW_DECLARE_EXCEPTION(OOPCpp1);

class OOPCpp1ProviderRunner
{
public:
	static const char* const COMPONENT_NAME;

	class InitializeCallback
	{
	public:
		InitializeCallback()
			: m_initialized(false)
		{
		}

		virtual ~InitializeCallback()
		{
		}
	
	public:
		void init(const ProviderEnvironmentIFCRef& provenv)
		{
			if (!m_initialized)
			{
				doInit(provenv);
				m_initialized = true;
			}
		}

	private:
		virtual void doInit(const ProviderEnvironmentIFCRef& provenv) = 0;
		bool m_initialized;
	};

	OOPCpp1ProviderRunner(const UnnamedPipeRef& IOPipe, const String& logFile, const String& logLevel);
	int runProvider(ProviderBaseIFCRef& provider, const String& sourceLib, InitializeCallback& initializeCallback);
	ProviderEnvironmentIFCRef getProviderEnvironment();

private:
	UnnamedPipeRef m_IOPipe;
	IOIFCStreamBuffer m_inbuf;
	IOIFCStreamBuffer m_outbuf;
};

} // end namespace OW_NAMESPACE

#endif


