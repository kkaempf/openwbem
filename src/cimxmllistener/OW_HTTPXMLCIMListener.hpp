/*******************************************************************************
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

#ifndef OW_HTTPXMLCIMLISTENER_HPP_
#define OW_HTTPXMLCIMLISTENER_HPP_
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
#include "blocxx/Mutex.hpp"
#include "OW_ClientAuthCBIFC.hpp"
#include "OW_CommonFwd.hpp"
#include "OW_CIMXMLListenerFwd.hpp"
#include "OW_ServicesHttpFwd.hpp"

namespace OW_NAMESPACE
{
class HTTPXMLCIMListenerCallback;
typedef blocxx::IntrusiveReference<HTTPXMLCIMListenerCallback> HTTPXMLCIMListenerCallbackRef;

class OW_LISTENER_API HTTPXMLCIMListener
{
public:
	/**
	 * @param certFileName Filename of a certificate to use for HTTPS. If none specified,
	 *  listner will receive over HTTP instead.
	 * @param keyFileName Filename of a private key to use for HTTPS. If none specified,
	 *  will attempt to use a key found in the same file as the certificate.
	 */
	HTTPXMLCIMListener(const blocxx::String& certFileName = blocxx::String(), const blocxx::String& keyFileName = blocxx::String());
	~HTTPXMLCIMListener();
	/**
	 * Register for an indication.  The destructor will attempt to deregister
	 * any subscriptions which are still outstanding at the time.
	 * @param url The url identifying the cimom
	 * @param ns The namespace where the
	 *  indication subscription and related objects will be created.
	 * @param filter The filter for the indication subscription
	 * @param querylanguage The language of the filter (typically wql1)
	 * @param sourceNamespace The path to a local namespace where the Indications
	 *  originate. If empty, the namespace of the Filter registration
	 *  is assumed.
	 * @param cb An object derived from CIMListenerCallback.  When an
	 *	indication is received, the doIndicationOccured member function will be called
	 * @param authCb If authentication is necessary, and authCb != NULL, then
	 *  authCb->getCredentials() will be called to obtain credentials.
	 *
	 * @return A unique handle identifying the indication subscription and callback.
	 *		Use this handle to de-register the listener.
	 */
	blocxx::String registerForIndication(const blocxx::String& url,
		const blocxx::String& ns, const blocxx::String& filter,
		const blocxx::String& querylanguage,
		const blocxx::String& sourceNamespace,
		const CIMListenerCallbackRef& cb,
		const ClientAuthCBIFCRef& authCb = ClientAuthCBIFCRef());

	/**
	 * De-register for an indication
	 * @param handle The string returned from registerForIndication
	 * @param authCb If authentication is necessary, and authCb != NULL, then
	 *  authCb->getCredentials() will be called to obtain credentials.
	 */
	void deregisterForIndication( const blocxx::String& handle );

	/**
	 * Shut down the http server that is listening for indications.
	 *	This function blocks until all threads that are running callbacks
	 *	have terminated.
	 */
	void shutdownHttpServer();
private:

#ifdef OW_WIN32
#pragma warning (push)
#pragma warning (disable: 4251)
#endif

	blocxx::IntrusiveReference<ListenerAuthenticator> m_pLAuthenticator;
	blocxx::IntrusiveReference<HTTPServer> m_httpServer;
	blocxx::UInt16 m_httpListenPort;
	blocxx::UInt16 m_httpsListenPort;
	blocxx::IntrusiveReference<blocxx::Thread> m_httpThread;
#ifndef OW_WIN32
	blocxx::UnnamedPipeRef m_stopHttpPipe;
#else
#pragma warning (pop)
#endif
	blocxx::String m_certFileName;
	blocxx::String m_keyFileName;
	HTTPXMLCIMListenerCallbackRef m_callback;
	RequestHandlerIFCRef m_XMLListener;

};

} // end namespace OW_NAMESPACE

#endif
