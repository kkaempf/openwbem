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
#include "OW_Reference.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "OW_Logger.hpp"
#include "OW_ConfigFile.hpp"
#include "OW_UnnamedPipe.hpp"
#include "OW_CppProviderBaseIFC.hpp"
#include "OW_AuthenticatorIFC.hpp"

namespace OpenWBEM
{

class HTTPServer;
class Thread;
class ProviderAgent
{
public:
	static const char* const LockingType_opt;  
	static const char* const LockingTimeout_opt; 
	static const char* const DynamicClassRetieval_opt; 

	/**
	 * Create a new provider agent, and start the HTTP server. 
	 * The ProviderAgent can be used as a stand alone process by 
	 * creating a ProviderAgent within a fairly simple main(), or 
	 * it can be embedded within an existing daemon or service. 
	 * 
	 * @param configMap The configuration parameters for the ProviderAgent
	 *        and its embedded HTTP server.  This could possibly come from
	 *        parsing a config file. 
	 * @param providers An array of providers to be embedded in the 
	 *        ProviderAgent.  These should be a CppInstanceProvider, 
	 *        CppSecondaryInstanceProvider, CppMethodProvider, or
	 *        CppAssociatorProvider, or a combination of these. 
	 * @param classes An array of CIMClasses.  If the providers require
	 *        CIMClasses to be passed into instance and associator calls, 
	 *        the appropriate classes should be placed in this array. 
	 *        The ProviderAgent will look up the appropriate class and 
	 *        pass it to the provider method calls as needed.  NULL CIMClasses
	 *        will be passed if the requested class is not found in the 
	 *        array.  This is only a problem is the provider expects a 
	 *        real class. 
	 * @param requestHandlers An array of request handlers.  The appropriate
	 *        one will be used for each client request depending on the 
	 *        HTTP headers of the request.  You can just put a single
	 *        request handler in the array if you only wish to handle one
	 *        type of encoding (CIM-XML, for example). 
	 * @param authenticator A reference to the authenticator to be used 
	 *        by the embedded HTTP server. 
	 * @param logger A reference to a logger to be used by the ProviderAgent
	 *        (and passed to the embedded Provider). 
	 * @param callbackURL A URL to a CIMOM for providers to be able to make
	 *        "upcalls" to the CIMOM.  If the CIMOM requires authentication, 
	 *        the authentication credentials must be in the URL.  If no 
	 *        callbackURL is provided, providers will be unable to callback
	 *        to the CIMOM. 
	 */
	ProviderAgent(ConfigFile::ConfigMap configMap, 
				  Array<CppProviderBaseIFCRef> providers, 
				  Array<CIMClass> classes, 
				  Array<RequestHandlerIFCRef> requestHandlers, 
				  Reference<AuthenticatorIFC> authenticator,
				  LoggerRef logger = LoggerRef(0), 
				  const String& callbackURL = String("")); 
	~ProviderAgent();
	/**
	 * Shut down the http server embedded within the ProviderAgent. 
	 * This function blocks until all threads which may be processing
	 * requests have terminated.
	 */
	void shutdownHttpServer();
private:
	Reference<HTTPServer> m_httpServer;
	Reference<Thread> m_httpThread;
	UnnamedPipeRef m_stopHttpPipe;
};

} // end namespace OpenWBEM

#endif //#ifndef OW_PROVIDERAGENT_HPP_INCLUDE_GUARD_
