/*******************************************************************************
* Copyright (C) 2005 Novell, Inc. All rights reserved.
* Copyright (C) 2001-2004 Quest Software, Inc. All rights reserved.
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
*  - Neither the name of Quest Software, Inc., Novell, Inc., nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Quest Software, Inc., Novell, Inc., OR THE CONTRIBUTORS
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

#ifndef OW_CIMXMLLISTENER_HPP_
#define OW_CIMXMLLISTENER_HPP_
#include "OW_config.h"
#include "blocxx/String.hpp"
#include "blocxx/Map.hpp"
#include "blocxx/IntrusiveReference.hpp"
#include "OW_CIMListenerCallback.hpp"
#include "OW_URL.hpp"
#include "OW_CIMObjectPath.hpp"
#include "OW_RequestHandlerIFC.hpp"
#include "blocxx/Logger.hpp"
#ifndef OW_WIN32
#include "blocxx/UnnamedPipe.hpp"
#endif
#include "OW_ClientAuthCBIFC.hpp"
#include "OW_CommonFwd.hpp"
#include "OW_CIMXMLListenerFwd.hpp"
#include "OW_ServicesHttpFwd.hpp"
#include "OW_ConfigFile.hpp"
#include "OW_AuthenticatorIFC.hpp"

namespace OW_NAMESPACE
{

class OW_LISTENER_API CIMXMLListener : public blocxx::IntrusiveCountableBase
{
public:
	/**
	 * @param configItems Configuration settings for the HTTP server
	 *     and such.
	 * @param callback Callback to be called when indications are received.
	 * @param authenticator HTTP authenticator.  If none is provided, then
	 *     allow_anonymous or enable_digest should be set in the config settings
	 */
	CIMXMLListener(const ConfigFile::ConfigMap& configItems,
				   const CIMListenerCallbackRef& callback,
				   const AuthenticatorIFCRef& authenticator = AuthenticatorIFCRef(blocxx::SharedLibraryRef(0), 0));
	virtual ~CIMXMLListener();

	/**
	 * Shut down the http server that is listening for indications.
	 *	This function blocks until all threads that are running callbacks
	 *	have terminated.
	 */
	void shutdownHttpServer();
protected:
	/*
	virtual void doIndicationOccurred( CIMInstance& ci,
		const String& listenerPath );
		*/
private:

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	RequestHandlerIFCRef m_XMLListener;
	blocxx::IntrusiveReference<HTTPServer> m_httpServer;
	blocxx::UInt16 m_httpListenPort;
	blocxx::UInt16 m_httpsListenPort;
	blocxx::IntrusiveReference<blocxx::Thread> m_httpThread;
#ifndef OW_WIN32
	blocxx::UnnamedPipeRef m_stopHttpPipe;
#else
#pragma warning (pop)
#endif
};

} // end namespace OW_NAMESPACE

#endif
