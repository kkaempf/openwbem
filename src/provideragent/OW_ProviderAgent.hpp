/*******************************************************************************
* Copyright (C) 2001-2004 Vintela, Inc. All rights reserved.
* Copyright (C) 2004 Novell, Inc. All rights reserved.
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

#ifndef OW_PROVIDERAGENT_HPP_INCLUDE_GUARD_
#define OW_PROVIDERAGENT_HPP_INCLUDE_GUARD_
#include "OW_config.h"
#include "OW_String.hpp"
#include "OW_Map.hpp"
#include "OW_Reference.hpp"
#include "OW_URL.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_Logger.hpp"
#include "OW_ConfigFile.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_CppProviderBaseIFC.hpp"
#include "OW_AuthenticatorIFC.hpp"

namespace OpenWBEM
{

class HTTPServer;
class NonAuthenticatingAuthenticator;
class Thread;
class ProviderAgent
{
public:
	/**
	 * @param logger If a logger specified then it will receive log messages, otherwise
	 *  all log messages will be discarded.
	 */
	ProviderAgent(ConfigFile::ConfigMap configMap, 
				  Reference<CppProviderBaseIFC> provider, 
				  Array<RequestHandlerIFCRef> requestHandlers, 
				  Reference<AuthenticatorIFC> authenticator,
				  LoggerRef logger = LoggerRef(0)); 
	~ProviderAgent();
	/**
	 * Shut down the http server that is listening for indications.
	 *	This function blocks until all threads that are running callbacks
	 *	have terminated.
	 */
	void shutdownHttpServer();
private:
	RequestHandlerIFCRef m_requestHandler;
	Reference<NonAuthenticatingAuthenticator> m_pLAuthenticator;
	Reference<HTTPServer> m_httpServer;
	UInt16 m_httpListenPort;
	UInt16 m_httpsListenPort;
	Reference<Thread> m_httpThread;
	UnnamedPipeRef m_stopHttpPipe;
	LoggerRef m_logger; 
	ConfigFile::ConfigMap m_configMap; 
	CIMOMHandleIFCRef m_cimomHandle; 
};

} // end namespace OpenWBEM

typedef OpenWBEM::ProviderAgent OW_ProviderAgent OW_DEPRECATED;

#endif //#ifndef OW_PROVIDERAGENT_HPP_INCLUDE_GUARD_
